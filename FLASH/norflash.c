#include "norflash.h"
#include "spi.h"

uint16_t g_norflash_type;

/**
 * @brief   初始化NOR Flash
 * @param   无
 * @retval  无
 */
void norflash_init(void) {
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t temp;

    /* 时钟使能 */
    NORFLASH_CS_GPIO_CLK_ENABLE();

    /* 配置CS引脚 */
    gpio_init_struct.Pin = NORFLASH_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NORFLASH_CS_GPIO_PORT, &gpio_init_struct);
    NORFLASH_CS(1);

    /* 配置SPI接口 */
    SPI2_Init();
    spi2_set_speed(SPI_BAUDRATEPRESCALER_2);

    g_norflash_type = norflash_read_id();                   /* 读取NOR Flash芯片ID */
    if (g_norflash_type == W25Q256)                         /* W25Q256需使能4字节地址模式 */
    {
        temp = norflash_read_sr(3);                         /* 读状态寄存器3，判断地址模式 */
        if ((temp & 0x01) == 0)                             /* 如果不是4字节地址模式，则需进行相应配置 */
        {
            norflash_write_enable();                        /* NOR Flash写使能 */
            temp |= (1 << 1);                               /* ADP=1，配置上电4字节地址模式 */
            norflash_write_sr(3, temp);                     /* 写状态寄存器3 */

            NORFLASH_CS(0);                                 /* 使能NOR Flash片选 */
            spi2_read_write_byte(NORFLASH_Enable4ByteAddr); /* 使能4字节地址模式 */
            NORFLASH_CS(1);                                 /* 失能NOR Flash片选 */
        }
    }
}

/**
 * @brief   等待NOR Flash空闲
 * @param   无
 * @retval  无
 */
static void norflash_wait_busy(void) {
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /* 等待BUSY位清零 */
}

/**
 * @brief   写使能NOR Flash
 * @note    将S1寄存器的WEL置位
 * @param   无
 * @retval  无
 */
void norflash_write_enable(void) {
    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(NORFLASH_WriteEnable); /* 发送写使能命令 */
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */
}

/**
 * @brief   向NOR Flash发送地址
 * @note    根据芯片型号的不同，发送3字节或4字节地址
 * @param   address: 待发送的地址
 * @retval  无
 */
static void norflash_send_address(uint32_t address) {
    if (g_norflash_type == W25Q256)                     /* 只有W25Q256支持4字节地址模式 */
    {
        spi2_read_write_byte((uint8_t) (address >> 24)); /* 发送bit31~bit24地址 */
    }
    spi2_read_write_byte((uint8_t) (address >> 16));     /* 发送bit23~bit16地址 */
    spi2_read_write_byte((uint8_t) (address >> 8));      /* 发送bit15~ bit8地址 */
    spi2_read_write_byte((uint8_t) address);             /* 发送 bit7~ bit0地址 */
}

/**
 * @brief   读NOR Flash的状态寄存器
 * @note    NOR Flash一共有3个状态寄存器
 *          状态寄存器1：
 *          BIT   7   6   5   4   3   2   1   0
 *               SPR  RV  TB BP2 BP1 BP0 WEL BUSY
 *          SPR：状态寄存器保护位，配合WP使用
 *          TB、BP2、BP1、BP0：Flash区域写保护设置
 *          WEL：写使能锁定
 *          BUSY：忙标记位（1：忙；0：空闲）
 *          默认：0x00
 *          状态寄存器2：
 *          BIT   7   6   5   4   3   2   1   0
 *               SUS CMP LB3 LB2 LB1 (R)  QE SRP1
 *          状态寄存器3：
 *          BIT   7       6    5    4   3   2   1   0
 *             HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 * @param   regno: 状态寄存器索引号，范围：1~3
 * @retval  状态寄存器的值
 */
uint8_t norflash_read_sr(uint8_t regno) {
    uint8_t byte;
    uint8_t command;

    switch (regno) {
        case 1: {
            command = NORFLASH_ReadStatusReg1;  /* 读状态寄存器1指令 */
            break;
        }
        case 2: {
            command = NORFLASH_ReadStatusReg2;  /* 读状态寄存器2指令 */
            break;
        }
        case 3: {
            command = NORFLASH_ReadStatusReg3;  /* 读状态寄存器3指令 */
            break;
        }
        default: {
            command = NORFLASH_ReadStatusReg1;  /* 默认读状态寄存器1指令 */
            break;
        }
    }

    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(command);              /* 发送读寄存器命令 */
    byte = spi2_read_write_byte(0xFF);          /* 读取一个字节 */
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */

    return byte;
}

/**
 * @brief   写NOR Flash的状态寄存器
 * @note    状态寄存器1：
 *          BIT   7   6   5   4   3   2   1   0
 *               SPR  RV  TB BP2 BP1 BP0 WEL BUSY
 *          SPR：状态寄存器保护位，配合WP使用
 *          TB、BP2、BP1、BP0：Flash区域写保护设置
 *          WEL：写使能锁定
 *          BUSY：忙标记位（1：忙；0：空闲）
 *          默认：0x00
 *          状态寄存器2：
 *          BIT   7   6   5   4   3   2   1   0
 *               SUS CMP LB3 LB2 LB1 (R)  QE SRP1
 *          状态寄存器3：
 *          BIT   7       6    5    4   3   2   1   0
 *             HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 * @param   regno: 状态寄存器索引号，范围：1~3
 * @param   sr   : 待写入状态寄存器的值
 * @retval  无
 */
void norflash_write_sr(uint8_t regno, uint8_t sr) {
    uint8_t command;

    switch (regno) {
        case 1: {
            command = NORFLASH_WriteStatusReg1; /* 写状态寄存器1指令 */
            break;
        }
        case 2: {
            command = NORFLASH_WriteStatusReg2; /* 写状态寄存器2指令 */
            break;
        }
        case 3: {
            command = NORFLASH_WriteStatusReg3; /* 写状态寄存器3指令 */
            break;
        }
        default: {
            command = NORFLASH_WriteStatusReg1; /* 默认写状态寄存器1指令 */
            break;
        }
    }

    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(command);              /* 发送读寄存器命令 */
    spi2_read_write_byte(sr);                   /* 写入一个字节 */
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */
}

/**
 * @brief   读NOR Flash芯片ID
 * @note    NOR Flash芯片ID定义见文件norflash.h中NOR Flash芯片ID定义部分
 * @param   无
 * @retval  NOR Flash芯片ID
 */
uint16_t norflash_read_id(void) {
    uint16_t deviceid;

    NORFLASH_CS(0);                                     /* 使能NOR Flash片选 */
    spi2_read_write_byte(NORFLASH_ManufactDeviceID);    /* 发送读ID命令 */
    spi2_read_write_byte(0);                            /* 写入一个字节 */
    spi2_read_write_byte(0);
    spi2_read_write_byte(0);
    deviceid = spi2_read_write_byte(0xFF) << 8;         /* 读取高8位字节 */
    deviceid |= spi2_read_write_byte(0xFF);             /* 读取低8位字节 */
    NORFLASH_CS(1);                                     /* 失能NOR Flash片选 */

    return deviceid;
}

/**
 * @brief   读NOR Flash
 * @note    从指定地址开始读取指定长度的数据
 * @param   pbuf   : 读取到数据保存的地址
 * @param   pbuf   : 指定开始读取的地址
 * @param   datalen: 指定读取数据的字节数
 * @retval  无
 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen) {
    uint16_t i;

    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(NORFLASH_ReadData);    /* 发送读取命令 */
    norflash_send_address(addr);                /* 发送地址 */
    for (i = 0; i < datalen; i++)                   /* 循环读取 */
    {
        pbuf[i] = spi2_read_write_byte(0xFF);
    }
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */
}

/**
 * @brief   从NOR Flash指定地址写入指定长度的数据
 * @note    写入数据的长度不能超过指定地址所在页的剩余字节数
 * @param   pbuf   : 待写入数据的起始地址
 * @param   addr   : 指定开始写入数据的地址
 * @param   datalen: 指定写入数据的字节数，范围：0~指定地址所在页剩余字节数
 * @retval  无
 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen) {
    uint16_t i;

    norflash_write_enable();                    /* NOR Flash写使能 */

    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(NORFLASH_PageProgram); /* 发送写页命令 */
    norflash_send_address(addr);                /* 发送地址 */
    for (i = 0; i < datalen; i++)               /* 循环写入 */
    {
        spi2_read_write_byte(pbuf[i]);
    }
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */

    norflash_wait_busy();                       /* 等待写入结束 */
}

/**
 * @brief   无检验从NOR Flash指定地址写入指定长度的数据
 * @note    必须确保所写的地址范围内的数据全部为0xFF,否则在非0xFF处写入的数据将失败
 *          具有自动换页功能
 *          在指定地址开始写入指定长度的数据，但是要确保地址不越界
 * @param   pbuf   : 待写入数据的起始地址
 * @param   addr   : 指定开始写入数据的地址
 * @param   datalen: 指定写入数据的字节数
 * @retval  无
 */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen) {
    uint16_t pageremain;

    pageremain = 256 - (addr % 256);                    /* 计算当前页剩余的字节数 */
    if (datalen <= pageremain)                          /* 不大于256个字节 */
    {
        pageremain = datalen;
    }

    /* 当写入字节比页内剩余地址还少的时候，一次性写完
     * 当写入直接比页内剩余地址还多的时候，先写完整个页内剩余地址，然后根据剩余长度进行不同处理
     */
    while (1) {
        norflash_write_page(pbuf, addr, pageremain);    /* 从NOR Flash指定地址写入指定长度的数据 */

        if (datalen == pageremain)                      /* 写入结束 */
        {
            break;
        } else                                            /* 写入未结束 */
        {
            pbuf += pageremain;                         /* 偏移pbuf指针地址pageremain个已写入的字节数 */
            addr += pageremain;                         /* 偏移写入地址pageremain个已写入的字节数 */
            datalen -= pageremain;                      /* 计算剩余写入的字节数 */
            if (datalen > 256)                          /* 剩余数据量超过256，只能先写入一页数据 */
            {
                pageremain = 256;
            } else                                        /* 剩余数据量不超过256，可一次性写入 */
            {
                pageremain = datalen;
            }
        }
    }
}

/* 扇区缓存 */
static uint8_t g_norflash_buf[4096];

/**
 * @brief   写NOR Flash
 * @note    在指定地址开始写入指定长度的数据，该函数带擦除操作
 *          NOR Flash一般是：256个字节为一个Page，4096个字节为一个Sector，16个Sector为1个Block
 *          擦除的最小单位为Sector
 * @param   pbuf   : 待写入数据的起始地址
 * @param   addr   : 指定开始写入数据的地址
 * @param   datalen: 指定写入数据的字节数
 * @retval  无
 */
void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen) {
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;

    norflash_buf = g_norflash_buf;                                      /* 扇区缓存 */
    secpos = addr / 4096;                                               /* 扇区索引 */
    secoff = addr % 4096;                                               /* 写入起始地址在当前扇区的偏移量 */
    secremain = 4096 - secoff;                                          /* 计算当扇区剩余的字节数 */
    if (datalen <= secremain)                                           /* 不大于4096个字节 */
    {
        secremain = datalen;
    }

    while (1) {
        norflash_read(norflash_buf, secpos * 4096, 4096);               /* 读出当前整个扇区的内容，用于擦除扇区后恢复非写入区域的数据 */

        for (i = 0; i < secremain; i++)                                     /* 校验数据 */
        {
            if (norflash_buf[secoff + i] != 0xFF)                       /* 写入区域中有非0xFF的数据，就需要擦除 */
            {
                break;
            }
        }

        if (i < secremain)                                              /* 需要擦除 */
        {
            norflash_erase_sector(secpos);                              /* 擦除整个扇区 */

            for (i = 0; i < secremain; i++)                                 /* 将待写入的数据，先写入扇区缓存 */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* 写入整个扇区 */
        } else                                                            /* 不需要擦除，可以直接写入 */
        {
            norflash_write_nocheck(pbuf, addr, secremain);              /* 直接写扇区剩余的空间 */
        }

        if (datalen == secremain)                                       /* 写入结束 */
        {
            break;
        } else                                                            /* 写入未结束 */
        {
            secpos++;                                                   /* 扇区索引增1 */
            secoff = 0;                                                 /* 扇区偏移位置为0 */
            pbuf += secremain;                                          /* 偏移pbuf指针地址secremain个已写入的字节数 */
            addr += secremain;                                          /* 偏移写入地址secremain个已写入的字节数 */
            datalen -= secremain;                                       /* 计算剩余写入的字节数 */
            if (datalen > 4096)                                         /* 剩余数据量超过4096，只能先写入一个扇区数据 */
            {
                secremain = 4096;
            } else                                                        /* 剩余数据量不超过4096，可一次性写入 */
            {
                secremain = datalen;
            }
        }
    }
}

/**
 * @brief   擦除整个NOR Flash芯片
 * @note    等待时间超长
 * @param   无
 * @retval  无
 */
void norflash_erase_chip(void) {
    norflash_write_enable();                    /* NOR Flash写使能 */
    norflash_wait_busy();                       /* 等待NOR Flash空闲 */
    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(NORFLASH_ChipErase);   /* 发送擦除芯片命令 */
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */
    norflash_wait_busy();                       /* 等待芯片擦除结束 */
}

/**
 * @brief   擦除NOR Flash一个扇区
 * @note    擦除一个扇区的最少时间为150毫秒
 * @param   saddr: 扇区索引号（非字节地址），范围：根据实际NOR Flash容量确定
 * @retval  无
 */
void norflash_erase_sector(uint32_t saddr) {
    saddr *= 4096;                              /* 计算扇区索引对应的字节地址 */
    norflash_write_enable();                    /* NOR Flash写使能 */
    norflash_wait_busy();                       /* 等待NOR Flash空闲 */

    NORFLASH_CS(0);                             /* 使能NOR Flash片选 */
    spi2_read_write_byte(NORFLASH_SectorErase); /* 发送擦除扇区命令 */
    norflash_send_address(saddr);               /* 发送地址 */
    NORFLASH_CS(1);                             /* 失能NOR Flash片选 */
    norflash_wait_busy();                       /* 等待扇区擦除完成 */
}

// 唤醒
void norflash_wakeup(void) {
    NORFLASH_CS(0);
    spi2_read_write_byte(NORFLASH_ReleasePowerDown);
    NORFLASH_CS(1);
}
