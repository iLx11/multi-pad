/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/26 13:55
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_FLASH_H
#define MULTI_FLASH_H

#include "stm32f1xx_hal.h"

// FLASH 片选线
#define FLASH_SPI_CS_GPIO GPIOC
#define FLASH_SPI_CS_PIN GPIO_PIN_4

#define ENABLE_CS_GPIO_RCC  do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while(0)

#define SET_FLASH_SPI_CS(x) do{ x ? \
                                  HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO, FLASH_SPI_CS_PIN, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO, FLASH_SPI_CS_PIN, GPIO_PIN_RESET); \
                            }while(0)

/********************************************************************************
* FLASH芯片列表 读取 ID 可以参考下列数据
********************************************************************************/
#define W25Q80      0XEF13          /* W25Q80   芯片ID */
#define W25Q16      0XEF14          /* W25Q16   芯片ID */
#define W25Q32      0XEF15          /* W25Q32   芯片ID */
#define W25Q64      0XEF16          /* W25Q64   芯片ID */
#define W25Q128     0XEF17          /* W25Q128  芯片ID */
#define W25Q256     0XEF18          /* W25Q256  芯片ID */
#define BY25Q64     0X6816          /* BY25Q64  芯片ID */
#define BY25Q128    0X6817          /* BY25Q128 芯片ID */
#define NM25Q64     0X5216          /* NM25Q64  芯片ID */
#define NM25Q128    0X5217          /* NM25Q128 芯片ID */

/********************************************************************************
* FLASH 指令
********************************************************************************/
// 读取芯片信息
#define MANUFACTURE_DEVICE_ID_CMD               0x90
// 使能 4 字节地址
#define ENABLE_4_BYTE_ARRAY_CMD                 0xB7
// 读取 FLASH 数据
#define READ_FLASH_DATA_CMD                     0x03
#define READ_FAST_FLASH_DATA_CMD                0x0B
#define READ_FAST_FLASH_DUAL_CMD                0x3B
#define READ_FAST_FLASH_QUAD_CMD                0xEB
// 读取状态寄存器
#define READ_STATUS_REG_1_CMD                   0x05
#define READ_STATUS_REG_2_CMD                   0x35
#define READ_STATUS_REG_3_CMD                   0x15
// 写状态寄存器
#define WRITE_STATUS_REG_1_CMD                  0x01
#define WRITE_STATUS_REG_2_CMD                  0x31
#define WRITE_STATUS_REG_3_CMD                  0x11
// 页命令
#define FLASH_PAGE_PROGRAM_CMD                  0x02
#define FLASH_PAGE_PROGRAM_QUAD_CMD             0x32
// 读写使能
#define ENABLE_FLASH_WRITE_CMD                  0x06
#define ENABLE_FLASH_READ_CMD                   0x04
// 擦除指令
#define ERASE_FLASH_CMD                         0xC7
#define ERASE_SECTOR_CMD                        0x20
#define ERASE_BLOCK_CMD                         0xD8

void flash_init(void);

uint16_t read_flash_id(void);

void read_flash(uint8_t *read_buff, uint32_t address, uint16_t length);

static void write_flash_page(uint8_t *write_buff, uint32_t address, uint16_t length);

void write_to_flash(uint8_t *write_buff, uint32_t address, uint16_t length);

static uint8_t read_flash_state_sr(uint8_t sr_index);

static void write_flash_state_sr(uint8_t sr_index, uint8_t state_data);

void enable_flash_write(void);

static void send_flash_address(uint32_t address);

static void wait_when_busy(void);

void erase_whole_flash(void);

void erase_flash_sector(uint32_t sector_address);


#endif //MULTI_FLASH_H
