/**
 ****************************************************************************************************
 * @file        diskio.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-23
 * @brief       FATFS�ײ�(diskio) ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220523
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./FATFS/source/diskio.h"
#include "drv_sdmmc.h"
#include "drv_norflash_ex.h"
#include "ff.h"



#define SD_CARD     0       /* SD��,����Ϊ0 */
#define EX_FLASH    1       /* �ⲿspi flash,����Ϊ1 */


 /**
  * SPB����ռ�ô�С��2049600=1.955MB��501����������4K�ֽ�һ���������㣩
  * �ֿ��С��6302984=6.011MB��1539������
  * ����25Q128
  * 0X90000000 �ǻ���ַ,Ϊ�������ۺ�ʵ�飬�Ի���ַΪƫ�ƣ�
  *  �� ��������0~4096*199��ռ200��������800KB
  * SPB ��������200*4096~700*4096��ռ501��������2004KB
  * �ֿ���������701*4096~2239*4096��ռ1539������,6156KB
  * �ļ�ϵͳ����2240*4096~4095*4096��ռ1856��������7424KB
  */

#define SPI_FLASH_SECTOR_SIZE   512
#define SPI_FLASH_SECTOR_COUNT  7424 * 2        /* 25Q128,���7424KB�ֽڸ�FATFSռ�� */
#define SPI_FLASH_BLOCK_SIZE    8               /* ÿ��BLOCK��8������ */
#define SPI_FLASH_FATFS_BASE    2240 * 4096     /* FATFS���ⲿFLASH����ʼ��ַ */


  /**
   * @brief       ��ô���״̬
   * @param       pdrv : ���̱��0~9
   * @retval      ִ�н��(�μ�FATFS, DSTATUS�Ķ���)
   */
DSTATUS disk_status(
	BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}

/**
 * @brief       ��ʼ������
 * @param       pdrv : ���̱��0~9
 * @retval      ִ�н��(�μ�FATFS, DSTATUS�Ķ���)
 */
DSTATUS disk_initialize(
	BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
	uint8_t res = 0;

	switch (pdrv)
	{
	case SD_CARD:           /* SD�� */
		res = sd_init();    /* SD����ʼ�� */
		break;

	case EX_FLASH:          /* �ⲿflash */
		/* û�г�ʼ�����������ܵ��ã�norflash_init��
		 * ��Ϊ��sys.c����,�����ڴ�ӳ���ʱ���Ѿ���ʼ��QSPI�ӿ���
		 */

		 /* ��ȡFLASH ID, ͨ����ȡID, ��g_norflash_addrw�޸�,��֧��25Q256
		  * ���������norflash_init, ����Ҫ�ٶ�ȡID��
		  */
		norflash_ex_read_id();
		break;

	default:
		res = 1;
	}

	if (res)
	{
		return  STA_NOINIT;
	}
	else
	{
		return 0; /* ��ʼ���ɹ�*/
	}
}

/**
 * @brief       ������
 * @param       pdrv   : ���̱��0~9
 * @param       buff   : ���ݽ��ջ����׵�ַ
 * @param       sector : ������ַ
 * @param       count  : ��Ҫ��ȡ��������
 * @retval      ִ�н��(�μ�FATFS, DRESULT�Ķ���)
 */
DRESULT disk_read(
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE* buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
	uint8_t res = 0;

	if (!count) return RES_PARERR;  /* count���ܵ���0�����򷵻ز������� */

	switch (pdrv)
	{
	case SD_CARD:   /* SD�� */
		res = sd_read_disk(buff, sector, count);

		while (res) /* ������ */
		{
			if (res != 2) sd_init();    /* ���³�ʼ��SD�� */

			res = sd_read_disk(buff, sector, count);
			//printf("sd rd error:%d\r\n", res);
		}

		break;

	case EX_FLASH:  /* �ⲿflash */
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
	}

	/* ��������ֵ��������ֵת��ff.c�ķ���ֵ */
	if (res == 0x00)
	{
		return RES_OK;
	}
	else
	{
		return RES_ERROR;
	}
}

/**
 * @brief       д����
 * @param       pdrv   : ���̱��0~9
 * @param       buff   : �������ݻ������׵�ַ
 * @param       sector : ������ַ
 * @param       count  : ��Ҫд���������
 * @retval      ִ�н��(�μ�FATFS, DRESULT�Ķ���)
 */
DRESULT disk_write(
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE* buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
	uint8_t res = 0;

	if (!count) return RES_PARERR;  /* count���ܵ���0�����򷵻ز������� */

	switch (pdrv)
	{
	case SD_CARD:       /* SD�� */
		res = sd_write_disk((uint8_t*)buff, sector, count);

		while (res)     /* д���� */
		{
			sd_init();  /* ���³�ʼ��SD�� */
			res = sd_write_disk((uint8_t*)buff, sector, count);
			//printf("sd wr error:%d\r\n", res);
		}

		break;

	case EX_FLASH:      /* �ⲿflash */
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
	}

	/* ��������ֵ��������ֵת��ff.c�ķ���ֵ */
	if (res == 0x00)
	{
		return RES_OK;
	}
	else
	{
		return RES_ERROR;
	}
}

/**
 * @brief       ��ȡ�������Ʋ���
 * @param       pdrv   : ���̱��0~9
 * @param       ctrl   : ���ƴ���
 * @param       buff   : ����/���ջ�����ָ��
 * @retval      ִ�н��(�μ�FATFS, DRESULT�Ķ���)
 */
DRESULT disk_ioctl(
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void* buff      /* Buffer to send/receive control data */
)
{
	DRESULT res;

	if (pdrv == SD_CARD)    /* SD�� */
	{
		switch (cmd)
		{
		case CTRL_SYNC:
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE:
			*(DWORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE:
			*(WORD*)buff = g_sd_card_info_handle.LogBlockSize;
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT:
			*(DWORD*)buff = g_sd_card_info_handle.LogBlockNbr;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
			break;
		}
	}
	else if (pdrv == EX_FLASH)  /* �ⲿFLASH */
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
			*(DWORD*)buff = SPI_FLASH_SECTOR_COUNT;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
			break;
		}
	}
	else
	{
		res = RES_ERROR;    /* �����Ĳ�֧�� */
	}

	return res;
}




















