/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#define SD_CARD     0       /* SD卡,卷标为0 */
#define EX_FLASH    1       /* 外部spi flash,卷标为1 */


 /**
  * SPB滑屏占用大小：2049600=1.955MB≈501个扇区（以4K字节一个扇区计算）
  * 字库大小：6302984=6.011MB≈1539个扇区
  * 对于25Q128
  * 0X90000000 是基地址,为了满足综合实验，以基地址为偏移：
  *  主 代码区：0~4096*199，占200个扇区，800KB
  * SPB 数据区：200*4096~700*4096，占501个扇区，2004KB
  * 字库数据区：701*4096~2239*4096，占1539个扇区,6156KB
  * 文件系统区：2240*4096~4095*4096，占1856个扇区，7424KB
  */

#define SPI_FLASH_SECTOR_SIZE   512
#define SPI_FLASH_SECTOR_COUNT  7424 * 2        /* 25Q128,最后7424KB字节给FATFS占用 */
#define SPI_FLASH_BLOCK_SIZE    8               /* 每个BLOCK有8个扇区 */
#define SPI_FLASH_FATFS_BASE    2240 * 4096     /* FATFS在外部FLASH的起始地址 */

  /* Definitions of physical drive number for each drive */
#define DEV_RAM		2	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		3	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		4	/* Example: Map USB MSD to physical drive 2 */

  /*-----------------------------------------------------------------------*/
  /* Get Drive Status                                                      */
  /*-----------------------------------------------------------------------*/
  /**
   * @brief       获得磁盘状态
   * @param       pdrv : 磁盘编号0~9
   * @retval      执行结果(参见FATFS, DSTATUS的定义)
   */
DSTATUS disk_status(
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
/**
 * @brief       初始化磁盘
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_initialize(
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	uint8_t res = 0;

	switch (pdrv) {
	case SD_CARD:			/*SD卡*/
		res = sd_init();	/*SD卡初始化*/
		break;

	case EX_FLASH:			/*外部flash*/
		/* 没有初始化函数！不能调用：norflash_init！
				 * 因为在sys.c里面,开启内存映射的时候，已经初始化QSPI接口了
				 */

				 /* 读取FLASH ID, 通过读取ID, 将g_norflash_addrw修改,以支持25Q256
				  * 如果调用了norflash_init, 则不需要再读取ID了
				  */
		norflash_ex_read_id();
		break;
	default:
		res = 1;
	}
	if (res)
		return STA_NOINIT;
	else
		return 0;		/*初始化成功*/
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
/**
 * @brief       读扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 数据接收缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_read(
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE* buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	uint8_t res = 0;
	if (!count) return RES_PARERR;		/*count不能等于0，否则返回错误参数*/

	switch (pdrv)
	{
	case SD_CARD:		/*SD卡*/
		res = sd_read_disk(buff, sector, count);
		while (res)
		{
			if (res != 2)	sd_init();	/*重新初始化SD卡*/
			res = sd_read_disk(buff, sector, count);
		}

		break;
	case EX_FLASH:
		for (; count > 0; count--)
		{
			norflash_ex_read(buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
			sector++;
			buff += SPI_FLASH_SECTOR_SIZE;
		}

		res = 0;
		break;

	default:
		res = 1;
		break;
	}

	/*处理返回值，将返回值转成ff.c的返回值*/
	if (res == 0x00)
	{
		return RES_OK;
	}
	else
	{
		return RES_ERROR;
	}

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE* buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t res = 0;
	if (!count) return RES_PARERR;		/*count不能等于0，否则返回错误参数*/

	switch (pdrv)
	{
	case SD_CARD:
		res = sd_write_disk((uint8_t*)buff, sector, count);
		while (res)		/*写出错*/
		{
			sd_init();	/*重新初始化SD卡*/
			res = sd_write_disk((uint8_t*)buff, sector, count);
		}

		break;
	case EX_FLASH:
		for (; count > 0; count--)
		{
			norflash_ex_write((uint8_t*)buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
			sector++;
			buff += SPI_FLASH_SECTOR_SIZE;
		}
		res = 0;
		break;
	default:
		res = 1;
		break;
	}
	/*处理返回值，将返回值转成ff.c的返回值*/
	if (res == 0x00)
	{
		return RES_OK;
	}
	else
	{
		return RES_ERROR;
	}
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
/**
 * @brief       获取其他控制参数
 *	@note		这些都采用的是标准IO
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_ioctl(
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void* buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	if (pdrv == SD_CARD)
	{
		switch (cmd)
		{
		case CTRL_SYNC:
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE:
			*(DWORD*)buff = 512;
			res = RES_OK;

		case GET_BLOCK_SIZE:
			*(WORD*)buff = g_sd_card_info_handle.LogBlockSize;
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT:
			*(WORD*)buff = g_sd_card_info_handle.LogBlockNbr;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
			break;
		}
	}
	else if (pdrv == EX_FLASH)
	{
		switch (cmd)
		{
		case CTRL_SYNC:
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE:
			*(WORD*)buff = SPI_FLASH_SECTOR_SIZE;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE:
			*(WORD*)buff = SPI_FLASH_BLOCK_SIZE;
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT:
			*(WORD*)buff = SPI_FLASH_SECTOR_COUNT;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
			break;
		}
	}
	else
	{
		res = RES_ERROR;	/*其他的暂时不支持*/
	}

	return res;
}

