/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/26 13:55
* @version: 1.0
* @description: 
********************************************************************************/


#include <stdio.h>
#include "flash.h"
#include "spi1.h"

// FLASH ���ͣ�����Ĭ��ֵ
uint16_t flash_type = NM25Q128;

// ��ȡ��������
uint8_t flash_data_buff[4096];

/********************************************************************************
* FLASH ��ʼ��
********************************************************************************/
void flash_init(void) {
    ENABLE_CS_GPIO_RCC;
    // cs Ƭѡ
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = FLASH_SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FLASH_SPI_CS_GPIO, &GPIO_InitStruct);

    SET_FLASH_SPI_CS(1);
    // spi1 ��ʼ��
    spi1_init();
    spi1_set_speed(SPI_SPEED_4);
    flash_type = read_flash_id();
    // ���Ӷ��� 4 �ֽڵ�ַ���ж�
    if (flash_type == W25Q256) {
        // ��ȡ״̬�Ĵ��� 3
        uint8_t mode = read_flash_state_sr(3);
        // ������� 4 �ֽڵ�ַ��������Ϊ 4 �ֽڵ�ַ
        if ((mode & 0x01) == 0) {
            // дʹ��
            enable_flash_write();
            // д״̬�Ĵ��� 3
            uint8_t temp;
            temp |= 1 << 1;
            write_flash_state_sr(3, temp);
            SET_FLASH_SPI_CS(0);
            // ʹ�� 4 �ֽڼĴ���ָ��
            spi1_transmit_receive_byte(ENABLE_4_BYTE_ARRAY_CMD);
            SET_FLASH_SPI_CS(1);
        }
    }
}

/********************************************************************************
* ��ȡ FLASH ID
********************************************************************************/
uint16_t read_flash_id(void) {
    uint16_t flash_id;
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(MANUFACTURE_DEVICE_ID_CMD);//���Ͷ�ȡID����
    spi1_transmit_receive_byte(0x00);
    spi1_transmit_receive_byte(0x00);
    spi1_transmit_receive_byte(0x00);
    flash_id |= ((spi1_transmit_receive_byte(0xFF) << 8) | spi1_transmit_receive_byte(0xFF));
    SET_FLASH_SPI_CS(1);
    return flash_id;
}

/********************************************************************************
* ��ȡ FLASH
 * @note      ��ָ����ַ��ʼ��ȡָ�����ȵ�����
 * @param       read_buff    : ���ݴ洢��
 * @param       address    : ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param       length : Ҫ��ȡ���ֽ���(���65535)
 * @retval      ��
********************************************************************************/
void read_flash(uint8_t *read_buff, uint32_t address, uint16_t length) {
    SET_FLASH_SPI_CS(0);
    // ���Ͷ�ȡָ��
    spi1_transmit_receive_byte(READ_FLASH_DATA_CMD);
    // ���͵�ַ
    send_flash_address(address);
    for (uint16_t i = 0; i < length; i++)
        read_buff[i] = spi1_transmit_receive_byte(0xFF);

    SET_FLASH_SPI_CS(1);
}

/********************************************************************************
* ��ҳд��
 * @brief       SPI��һҳ(0~65535)��д������256���ֽڵ�����
 *   @note      ��ָ����ַ��ʼд�����256�ֽڵ�����
 * @param       write_buff    : ���ݴ洢��
 * @param       address    : ��ʼд��ĵ�ַ(���32bit)
 * @param       length : Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
 * @retval      ��
********************************************************************************/
static void write_flash_page(uint8_t *write_buff, uint32_t address, uint16_t length) {
    // дʹ��
    enable_flash_write();

    SET_FLASH_SPI_CS(0);
    // ����ҳд����
    spi1_transmit_receive_byte(FLASH_PAGE_PROGRAM_CMD);
    send_flash_address(address);
    for (uint16_t i = 0; i < length; i++)
        spi1_transmit_receive_byte(write_buff[i]);

    SET_FLASH_SPI_CS(1);
    wait_when_busy();
}

/********************************************************************************
* �޼���д��
 * @brief       �޼���дSPI FLASH
 *   @note      ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
 *              �����Զ���ҳ����
 *              ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
 *
 * @param       write_buff    : ���ݴ洢��
 * @param       address    : ��ʼд��ĵ�ַ(���32bit)
 * @param       length : Ҫд����ֽ���(���65535)
 * @retval      ��
********************************************************************************/
static void write_flash_nocheck(uint8_t *write_buff, uint32_t address, uint16_t length) {
    // ��ҳʣ����ֽ�
    uint16_t page_residue = 256 - (address % 256);
    if (length <= page_residue) page_residue = length;
    while (1) {
        write_flash_page(write_buff, address, page_residue);
        if (page_residue == length) {
            break;
        } else {
            write_buff += page_residue;
            address += page_residue;
            length -= page_residue;
            if (length > 256) page_residue = 256;
            else page_residue = length;
        }
    }
}

/********************************************************************************
* ������д��һ������
********************************************************************************/
void write_to_flash(uint8_t *write_buff, uint32_t address, uint16_t length) {
    // ��������
    uint32_t sector_index = address / 4096;
    // ��ǰ����ƫ�ƣ�����ڳ�ʼλ�ã�
    uint16_t sector_offset = address % 4096;
    // ����ʣ�����λ��
    uint16_t sector_residue = 4096 - sector_offset;
    // ����ȫ�ֱ���ָ��ָ��
    uint8_t *flash_buff_point = flash_data_buff;
    uint16_t i = 0;
    if (length <= sector_residue) sector_residue = length;

    while (1) {
        read_flash(flash_buff_point, sector_index * 4096, 4096);
        // �ж��Ƿ���Ҫ��������Ϊ FLASH ��Ҫ 0xFF ʱ��д��
//        while (flash_buff_point[sector_offset + i] == 0xFF && i < sector_residue) i++;
        for (i = 0; i < sector_residue; i++) {
            if (flash_buff_point[sector_offset + i] != 0XFF) break;
        }
        // �����Ҫ�������������д��
        if (i < sector_residue) {
            i = 0;
            erase_flash_sector(sector_index);
//            while (i < sector_residue) flash_buff_point[sector_index + i] = write_buff[i++];
            for (i = 0; i < sector_residue; i++) {
                flash_buff_point[i + sector_offset] = write_buff[i];
            }
            write_flash_nocheck(flash_buff_point, sector_index * 4096, 4096);
        } else  {
            write_flash_nocheck(write_buff, address, sector_residue);
        }
        if (sector_residue == length) break;
        sector_index++;
        sector_offset = 0;
        write_buff += sector_residue;
        address += sector_residue;
        length -= sector_residue;
        if (length > 4096) sector_residue = 4096;
        else sector_residue = length;
    }
}

/********************************************************************************
* ��ȡ״̬�Ĵ���
 * @brief       ��ȡ25QXX��״̬�Ĵ�����25QXXһ����3��״̬�Ĵ���
 *   @note      ״̬�Ĵ���1��
 *              BIT7  6   5   4   3   2   1   0
 *              SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *              SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
 *              TB,BP2,BP1,BP0:FLASH����д��������
 *              WEL:дʹ������
 *              BUSY:æ���λ(1,æ;0,����)
 *              Ĭ��:0x00
 *
 *              ״̬�Ĵ���2��
 *              BIT7  6   5   4   3   2   1   0
 *              SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *              ״̬�Ĵ���3��
 *              BIT7      6    5    4   3   2   1   0
 *              HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param       sr_index: ״̬�Ĵ����ţ���:1~3
 * @retval      ״̬�Ĵ���ֵ
********************************************************************************/
static uint8_t read_flash_state_sr(uint8_t sr_index) {
    uint8_t cmd = READ_STATUS_REG_1_CMD;
    if (sr_index == 2)
        cmd = READ_STATUS_REG_2_CMD;
    else if (sr_index == 3)
        cmd = READ_STATUS_REG_3_CMD;
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(cmd);
    // ��ȡһ���ֽ�
    uint8_t result = spi1_transmit_receive_byte(0xFF);
    SET_FLASH_SPI_CS(1);
    return result;
}

/********************************************************************************
* д��״̬�Ĵ���
********************************************************************************/
static void write_flash_state_sr(uint8_t sr_index, uint8_t state) {
    uint8_t cmd = WRITE_STATUS_REG_1_CMD;
    if (sr_index == 2)
        cmd = WRITE_STATUS_REG_2_CMD;
    else if (sr_index == 3)
        cmd = WRITE_STATUS_REG_3_CMD;
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(cmd);
    // д��һ���ֽ�
    spi1_transmit_receive_byte(state);
    SET_FLASH_SPI_CS(1);
}

/********************************************************************************
* FLASH дʹ��
 * ��S1�Ĵ�����WEL��λ
********************************************************************************/
void enable_flash_write(void) {
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(ENABLE_FLASH_WRITE_CMD);
    SET_FLASH_SPI_CS(1);
}

/********************************************************************************
* ���͵�ַ
 * ����оƬ�ͺŵĲ�ͬ, ����24ibt / 32bit��ַ
********************************************************************************/
static void send_flash_address(uint32_t address) {
    if (flash_type == W25Q256) spi1_transmit_receive_byte((uint8_t) ((address) >> 24));
    spi1_transmit_receive_byte((uint8_t) ((address) >> 16));
    spi1_transmit_receive_byte((uint8_t) ((address) >> 8));
    spi1_transmit_receive_byte((uint8_t) address);
}

/********************************************************************************
* �ȴ�����
********************************************************************************/
static void wait_when_busy(void) {
    while ((read_flash_state_sr(1) & 0x01) == 0x01);
}

/********************************************************************************
* ��������оƬ
********************************************************************************/
void erase_whole_flash(void) {
    enable_flash_write();
    wait_when_busy();
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(ERASE_FLASH_CMD);
    SET_FLASH_SPI_CS(1);
    wait_when_busy();
}

/********************************************************************************
* ����һ������
********************************************************************************/
void erase_flash_sector(uint32_t sector_index) {
    sector_index *= 4096;
    enable_flash_write();
    wait_when_busy();
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(ERASE_SECTOR_CMD);
    send_flash_address(sector_index);
    SET_FLASH_SPI_CS(1);
    wait_when_busy();
}