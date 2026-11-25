#include "usb_update.h"
#include "stm32f1xx_hal.h"

#define BOOT_REQUEST ((volatile uint32_t *)0x2000fff8u)
#define BOOT_REQUEST_MAGIC 0x5544504du

void usb_update_reenumerate(void) {
    // Release the board's USB pull-up long enough for the host to see a detach.
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    GPIO_InitTypeDef pin = { .Pin = GPIO_PIN_12, .Mode = GPIO_MODE_OUTPUT_OD, .Speed = GPIO_SPEED_FREQ_LOW };
    HAL_GPIO_Init(GPIOA, &pin);
    HAL_Delay(80);
    pin.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &pin);
}

#ifdef MULTIPAD_USB_APP
void usb_update_enter(void) {
    BOOT_REQUEST[0] = BOOT_REQUEST_MAGIC;
    BOOT_REQUEST[1] = ~BOOT_REQUEST_MAGIC;
    __DSB();
    NVIC_SystemReset();
}
#endif

#ifdef MULTIPAD_BOOTLOADER
#include "usbd_cdc_if.h"
#include <stdbool.h>
#include <string.h>

#define APP_BASE 0x08008000u
#define STAGE_BASE 0x08040000u
#define IMAGE_MAX (STAGE_BASE - APP_BASE)
#define ACTIVE_META 0x0807f000u
#define PENDING_META 0x0807f800u
#define IMAGE_MAGIC 0x4d504631u

_Static_assert(FLASH_PAGE_SIZE == 0x800u, "Expected STM32F103VE flash pages");
_Static_assert(STAGE_BASE + IMAGE_MAX <= ACTIVE_META, "Update overlaps metadata");

enum { HELLO = 1, BEGIN, DATA, SEAL, REBOOT };
enum { OK, BAD_FRAME, RANGE, FLASH_ERROR, CHECKSUM, STATE };
enum { FRAME_BYTES = 56, PAYLOAD_BYTES = 40, RESPONSE_BYTES = 12 };

typedef struct {
    uint32_t magic;
    uint32_t size;
    uint32_t crc;
} ImageRecord;

static uint8_t rx[FRAME_BYTES];
static volatile uint8_t rx_used;
static uint8_t response[RESPONSE_BYTES];
static bool response_pending;
static bool reboot_requested;
static bool receiving;
static uint32_t expected_size, expected_crc, received;

static uint32_t read32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static void write32(uint8_t *p, uint32_t n) {
    for (unsigned i = 0; i < 4; i++) p[i] = (uint8_t)(n >> (i * 8));
}

static uint32_t crc32(const uint8_t *data, uint32_t size) {
    uint32_t value = 0xffffffffu;
    while (size--) {
        value ^= *data++;
        for (unsigned bit = 0; bit < 8; bit++) {
            value = (value >> 1) ^ ((value & 1u) ? 0xedb88320u : 0);
        }
    }
    return ~value;
}

static bool valid_vector(uint32_t address, uint32_t size) {
    if (size < 8 || size > IMAGE_MAX) return false;
    const uint32_t *vector = (const uint32_t *)address;
    uint32_t stack = vector[0], reset = vector[1];
    return stack >= 0x20000008u && stack <= (uint32_t)BOOT_REQUEST && !(stack & 7u)
        && (reset & 1u) && reset >= APP_BASE + 1u && reset < APP_BASE + size;
}

static bool valid_image(uint32_t address, ImageRecord record) {
    return record.magic == IMAGE_MAGIC && valid_vector(address, record.size)
        && crc32((const uint8_t *)address, record.size) == record.crc;
}

static bool erase_pages(uint32_t address, uint32_t size) {
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = address,
        .NbPages = (size + FLASH_PAGE_SIZE - 1u) / FLASH_PAGE_SIZE
    };
    uint32_t error;
    HAL_FLASH_Unlock();
    bool ok = HAL_FLASHEx_Erase(&erase, &error) == HAL_OK;
    HAL_FLASH_Lock();
    return ok;
}

static bool program(uint32_t address, const uint8_t *data, uint32_t size) {
    HAL_FLASH_Unlock();
    for (uint32_t offset = 0; offset < size; offset += 2) {
        uint16_t value = data[offset] | (uint16_t)(offset + 1 < size ? data[offset + 1] : 0xff) << 8;
        if (value != 0xffffu && HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + offset, value) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }
    HAL_FLASH_Lock();
    return memcmp((const void *)address, data, size) == 0;
}

static bool record_image(uint32_t address, ImageRecord record) {
    uint8_t bytes[sizeof(record)];
    write32(bytes, record.magic);
    write32(bytes + 4, record.size);
    write32(bytes + 8, record.crc);
    // The marker is committed only after the other fields are present.
    return erase_pages(address, FLASH_PAGE_SIZE)
        && program(address + 4, bytes + 4, sizeof(bytes) - 4)
        && program(address, bytes, 4);
}

__attribute__((naked,noreturn)) static void jump(uint32_t stack, uint32_t pc) {
    __asm volatile("msr msp, r0\ncpsie i\nbx r1");
}

void usb_update_boot(void) {
    bool requested = BOOT_REQUEST[0] == BOOT_REQUEST_MAGIC && BOOT_REQUEST[1] == ~BOOT_REQUEST_MAGIC;
    BOOT_REQUEST[0] = BOOT_REQUEST[1] = 0;
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitTypeDef key = { .Pin = GPIO_PIN_10, .Mode = GPIO_MODE_INPUT, .Pull = GPIO_PULLUP };
    HAL_GPIO_Init(GPIOE, &key);
    HAL_Delay(5);
    if (requested || HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_RESET) return;

    ImageRecord pending = *(const ImageRecord *)PENDING_META;
    if (pending.magic == IMAGE_MAGIC && valid_image(STAGE_BASE, pending)) {
        if (!valid_image(APP_BASE, pending)
            && (!erase_pages(APP_BASE, pending.size)
                || !program(APP_BASE, (const uint8_t *)STAGE_BASE, pending.size))) return;
        if (!valid_image(APP_BASE, pending) || !record_image(ACTIVE_META, pending)) return;
        if (!erase_pages(PENDING_META, FLASH_PAGE_SIZE)) return;
    }

    ImageRecord active = *(const ImageRecord *)ACTIVE_META;
    if (!valid_image(APP_BASE, active)) return;
    __disable_irq();
    HAL_RCC_DeInit();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    for (unsigned i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xffffffffu;
        NVIC->ICPR[i] = 0xffffffffu;
    }
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk | SCB_ICSR_PENDSVCLR_Msk;
    SCB->VTOR = APP_BASE;
    __DSB();
    __ISB();
    const uint32_t *vector = (const uint32_t *)APP_BASE;
    jump(vector[0], vector[1]);
}

void usb_update_receive(const uint8_t *data, uint32_t size) {
    if (rx_used == FRAME_BYTES || reboot_requested) return;
    while (size-- && rx_used < FRAME_BYTES) rx[rx_used++] = *data++;
}

void usb_update_poll(void) {
    if (reboot_requested && !response_pending && CDC_TransmitReady_FS()) NVIC_SystemReset();
    if (response_pending) {
        if (CDC_Transmit_FS(response, sizeof(response)) == USBD_OK) response_pending = false;
        return;
    }
    if (rx_used != FRAME_BYTES || !CDC_TransmitReady_FS()) return;

    uint8_t cmd = rx[4], length = rx[5], status = BAD_FRAME;
    uint32_t value = read32(rx + 8), crc = read32(rx + 12), reply = value;
    if (memcmp(rx, "MPU1", 4) == 0 && length <= PAYLOAD_BYTES) {
        switch (cmd) {
        case HELLO:
            status = OK;
            reply = IMAGE_MAX;
            break;
        case BEGIN:
            receiving = false;
            if (length || value < 8 || value > IMAGE_MAX) status = RANGE;
            else if (!erase_pages(PENDING_META, FLASH_PAGE_SIZE) || !erase_pages(STAGE_BASE, value)) status = FLASH_ERROR;
            else {
                expected_size = value;
                expected_crc = crc;
                received = 0;
                receiving = true;
                status = OK;
            }
            break;
        case DATA:
            if (!receiving) status = STATE;
            else if (value != received || !length || (value & 1u)
                     || length > expected_size - received
                     || ((length & 1u) && received + length != expected_size)) status = RANGE;
            else if (!program(STAGE_BASE + value, rx + 16, length)) {
                receiving = false;
                status = FLASH_ERROR;
            } else {
                received += length;
                reply = received;
                status = OK;
            }
            break;
        case SEAL:
            if (!receiving || received != expected_size) status = STATE;
            else {
                receiving = false;
                ImageRecord record = {IMAGE_MAGIC, expected_size, expected_crc};
                if (!valid_image(STAGE_BASE, record)) status = CHECKSUM;
                else if (!record_image(PENDING_META, record)) status = FLASH_ERROR;
                else status = OK;
            }
            break;
        case REBOOT:
            if (((const ImageRecord *)PENDING_META)->magic != IMAGE_MAGIC) status = STATE;
            else {
                reboot_requested = true;
                status = OK;
            }
            break;
        default:
            break;
        }
    }
    memcpy(response, "MPA1", 4);
    response[4] = cmd;
    response[5] = status;
    response[6] = response[7] = 0;
    write32(response + 8, reply);
    rx_used = 0;
    response_pending = true;
}
#endif
