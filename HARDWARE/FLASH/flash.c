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

// FLASH 类型，设置默认值
uint16_t flash_type = NM25Q128;

// 读取缓冲数组
uint8_t flash_data_buff[4096];

/********************************************************************************
* FLASH 初始化
********************************************************************************/
void flash_init(void) {
    ENABLE_CS_GPIO_RCC;
    // cs 片选
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = FLASH_SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FLASH_SPI_CS_GPIO, &GPIO_InitStruct);

    SET_FLASH_SPI_CS(1);
    // spi1 初始化
    spi1_init();
    spi1_set_speed(SPI_SPEED_4);
    flash_type = read_flash_id();
    // 增加对于 4 字节地址的判断
    if (flash_type == W25Q256) {
        // 读取状态寄存器 3
        uint8_t mode = read_flash_state_sr(3);
        // 如果不是 4 字节地址，则设置为 4 字节地址
        if ((mode & 0x01) == 0) {
            // 写使能
            enable_flash_write();
            // 写状态寄存器 3
            uint8_t temp;
            temp |= 1 << 1;
            write_flash_state_sr(3, temp);
            SET_FLASH_SPI_CS(0);
            // 使能 4 字节寄存器指令
            spi1_transmit_receive_byte(ENABLE_4_BYTE_ARRAY_CMD);
            SET_FLASH_SPI_CS(1);
        }
    }
}

/********************************************************************************
* 读取 FLASH ID
********************************************************************************/
uint16_t read_flash_id(void) {
    uint16_t flash_id;
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(MANUFACTURE_DEVICE_ID_CMD);//发送读取ID命令
    spi1_transmit_receive_byte(0x00);
    spi1_transmit_receive_byte(0x00);
    spi1_transmit_receive_byte(0x00);
    flash_id |= ((spi1_transmit_receive_byte(0xFF) << 8) | spi1_transmit_receive_byte(0xFF));
    SET_FLASH_SPI_CS(1);
    return flash_id;
}

/********************************************************************************
* 读取 FLASH
 * @note      在指定地址开始读取指定长度的数据
 * @param       read_buff    : 数据存储区
 * @param       address    : 开始读取的地址(最大32bit)
 * @param       length : 要读取的字节数(最大65535)
 * @retval      无
********************************************************************************/
void read_flash(uint8_t *read_buff, uint32_t address, uint16_t length) {
    SET_FLASH_SPI_CS(0);
    // 发送读取指令
    spi1_transmit_receive_byte(READ_FLASH_DATA_CMD);
    // 发送地址
    send_flash_address(address);
    for (uint16_t i = 0; i < length; i++)
        read_buff[i] = spi1_transmit_receive_byte(0xFF);

    SET_FLASH_SPI_CS(1);
}

/********************************************************************************
* 按页写入
 * @brief       SPI在一页(0~65535)内写入少于256个字节的数据
 *   @note      在指定地址开始写入最大256字节的数据
 * @param       write_buff    : 数据存储区
 * @param       address    : 开始写入的地址(最大32bit)
 * @param       length : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @retval      无
********************************************************************************/
static void write_flash_page(uint8_t *write_buff, uint32_t address, uint16_t length) {
    // 写使能
    enable_flash_write();

    SET_FLASH_SPI_CS(0);
    // 发送页写命令
    spi1_transmit_receive_byte(FLASH_PAGE_PROGRAM_CMD);
    send_flash_address(address);
    for (uint16_t i = 0; i < length; i++)
        spi1_transmit_receive_byte(write_buff[i]);

    SET_FLASH_SPI_CS(1);
    wait_when_busy();
}

/********************************************************************************
* 无检验写入
 * @brief       无检验写SPI FLASH
 *   @note      必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 *              具有自动换页功能
 *              在指定地址开始写入指定长度的数据,但是要确保地址不越界!
 *
 * @param       write_buff    : 数据存储区
 * @param       address    : 开始写入的地址(最大32bit)
 * @param       length : 要写入的字节数(最大65535)
 * @retval      无
********************************************************************************/
static void write_flash_nocheck(uint8_t *write_buff, uint32_t address, uint16_t length) {
    // 单页剩余的字节
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
* 擦除并写入一个扇区
********************************************************************************/
void write_to_flash(uint8_t *write_buff, uint32_t address, uint16_t length) {
    // 扇区索引
    uint32_t sector_index = address / 4096;
    // 当前扇区偏移（相对于初始位置）
    uint16_t sector_offset = address % 4096;
    // 扇区剩余操作位置
    uint16_t sector_residue = 4096 - sector_offset;
    // 保持全局变量指针指向
    uint8_t *flash_buff_point = flash_data_buff;
    uint16_t i = 0;
    if (length <= sector_residue) sector_residue = length;

    while (1) {
        read_flash(flash_buff_point, sector_index * 4096, 4096);
        // 判断是否需要擦除，因为 FLASH 需要 0xFF 时能写入
//        while (flash_buff_point[sector_offset + i] == 0xFF && i < sector_residue) i++;
        for (i = 0; i < sector_residue; i++) {
            if (flash_buff_point[sector_offset + i] != 0XFF) break;
        }
        // 如果需要擦除，则擦除后写入
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
* 读取状态寄存器
 * @brief       读取25QXX的状态寄存器，25QXX一共有3个状态寄存器
 *   @note      状态寄存器1：
 *              BIT7  6   5   4   3   2   1   0
 *              SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *              SPR:默认0,状态寄存器保护位,配合WP使用
 *              TB,BP2,BP1,BP0:FLASH区域写保护设置
 *              WEL:写使能锁定
 *              BUSY:忙标记位(1,忙;0,空闲)
 *              默认:0x00
 *
 *              状态寄存器2：
 *              BIT7  6   5   4   3   2   1   0
 *              SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *              状态寄存器3：
 *              BIT7      6    5    4   3   2   1   0
 *              HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param       sr_index: 状态寄存器号，范:1~3
 * @retval      状态寄存器值
********************************************************************************/
static uint8_t read_flash_state_sr(uint8_t sr_index) {
    uint8_t cmd = READ_STATUS_REG_1_CMD;
    if (sr_index == 2)
        cmd = READ_STATUS_REG_2_CMD;
    else if (sr_index == 3)
        cmd = READ_STATUS_REG_3_CMD;
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(cmd);
    // 读取一个字节
    uint8_t result = spi1_transmit_receive_byte(0xFF);
    SET_FLASH_SPI_CS(1);
    return result;
}

/********************************************************************************
* 写入状态寄存器
********************************************************************************/
static void write_flash_state_sr(uint8_t sr_index, uint8_t state) {
    uint8_t cmd = WRITE_STATUS_REG_1_CMD;
    if (sr_index == 2)
        cmd = WRITE_STATUS_REG_2_CMD;
    else if (sr_index == 3)
        cmd = WRITE_STATUS_REG_3_CMD;
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(cmd);
    // 写入一个字节
    spi1_transmit_receive_byte(state);
    SET_FLASH_SPI_CS(1);
}

/********************************************************************************
* FLASH 写使能
 * 将S1寄存器的WEL置位
********************************************************************************/
void enable_flash_write(void) {
    SET_FLASH_SPI_CS(0);
    spi1_transmit_receive_byte(ENABLE_FLASH_WRITE_CMD);
    SET_FLASH_SPI_CS(1);
}

/********************************************************************************
* 发送地址
 * 根据芯片型号的不同, 发送24ibt / 32bit地址
********************************************************************************/
static void send_flash_address(uint32_t address) {
    if (flash_type == W25Q256) spi1_transmit_receive_byte((uint8_t) ((address) >> 24));
    spi1_transmit_receive_byte((uint8_t) ((address) >> 16));
    spi1_transmit_receive_byte((uint8_t) ((address) >> 8));
    spi1_transmit_receive_byte((uint8_t) address);
}

/********************************************************************************
* 等待空闲
********************************************************************************/
static void wait_when_busy(void) {
    while ((read_flash_state_sr(1) & 0x01) == 0x01);
}

/********************************************************************************
* 擦除整个芯片
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
* 擦除一个扇区
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