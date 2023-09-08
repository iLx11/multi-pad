#ifndef __NORFLASH_H
#define __NORFLASH_H

#include "stm32f1xx.h"

/* 引脚定义 */
#define NORFLASH_CS_GPIO_PORT           GPIOA
#define NORFLASH_CS_GPIO_PIN            GPIO_PIN_4
#define NORFLASH_CS_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

/* IO操作 */
#define NORFLASH_CS(x)                  do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(NORFLASH_CS_GPIO_PORT, NORFLASH_CS_GPIO_PIN, GPIO_PIN_SET):   \
                                            HAL_GPIO_WritePin(NORFLASH_CS_GPIO_PORT, NORFLASH_CS_GPIO_PIN, GPIO_PIN_RESET); \
                                        } while (0)

/* NOR Flash芯片ID定义 */
#define W25Q80                          0xEF13
#define W25Q16                          0xEF14
#define W25Q32                          0xEF15
#define W25Q64                          0xEF16
#define W25Q128                         0xEF17
#define W25Q256                         0xEF18
#define BY25Q64                         0x6816
#define BY25Q128                        0x6817
#define NM25Q64                         0x5216
#define NM25Q128                        0x5217

/* NOR Flash指令定义 */
#define NORFLASH_WriteEnable            0x06
#define NORFLASH_WriteDisable           0x04
#define NORFLASH_ReadStatusReg1         0x05
#define NORFLASH_ReadStatusReg2         0x35
#define NORFLASH_ReadStatusReg3         0x15
#define NORFLASH_WriteStatusReg1        0x01
#define NORFLASH_WriteStatusReg2        0x31
#define NORFLASH_WriteStatusReg3        0x11
#define NORFLASH_ReadData               0x03
#define NORFLASH_FastReadData           0x0B
#define NORFLASH_FastReadDual           0x3B
#define NORFLASH_FastReadQuad           0xEB
#define NORFLASH_PageProgram            0x02
#define NORFLASH_PageProgramQuad        0x32
#define NORFLASH_BlockErase             0xD8
#define NORFLASH_SectorErase            0x20
#define NORFLASH_ChipErase              0xC7
#define NORFLASH_PowerDown              0xB9
#define NORFLASH_ReleasePowerDown       0xAB
#define NORFLASH_DeviceID               0xAB
#define NORFLASH_ManufactDeviceID       0x90
#define NORFLASH_JedecDeviceID          0x9F
#define NORFLASH_Enable4ByteAddr        0xB7
#define NORFLASH_Exit4ByteAddr          0xE9
#define NORFLASH_SetReadParam           0xC0
#define NORFLASH_EnterQPIMode           0x38
#define NORFLASH_ExitQPIMode            0xFF

/* 函数声明 */
void norflash_init(void);                                               /* 初始化NOR Flash */
void norflash_write_enable(void);                                       /* 写使能NOR Flash */
uint8_t norflash_read_sr(uint8_t regno);                                /* 读NOR Flash的状态寄存器 */
void norflash_write_sr(uint8_t regno, uint8_t sr);                      /* 写NOR Flash的状态寄存器 */
uint16_t norflash_read_id(void);                                        /* 读NOR Flash芯片ID */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);     /* 读NOR Flash */
void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);    /* 写NOR Flash */
void norflash_erase_chip(void);                                         /* 擦除整个NOR Flash芯片 */
void norflash_erase_sector(uint32_t saddr);                             /* 擦除NOR Flash一个扇区 */
void norflash_wakeup(void);

#endif
