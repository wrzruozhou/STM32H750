#ifndef __DRV_MALLOC_H
#define __DRV_MALLOC_H

#include "stm32h7xx_hal.h"

/* 定义5个内存池 */
#define SRAMIN 0    /*AXI内存池，AXI共512KB*/
#define SRAM12 1    /*SRAM1/2内存池，SRAM1+SRAM2,共256KB*/
#define SRAM4  2    /*SRAM4内存池，SRAM共64KB*/
#define SRAMDTCM 3  /*DTCM内存池，共128KB，仅仅CPU和MDAM（通过AHBS）可以访问*/
#define SRAMITCM 4  /*ITCM内存池，DTCM共64KB，此部分仅CPU和MDMA(通过AHBS)可以访问*/

#define SRAMBANK 5  /*定义支持的SRAM块数*/

/*定义内存管理表类型，当外拓SDRAM的时候，必须使用uint32_t类型，否则可以定义成uint16_t，以节省内存占用*/
#define MT_TYPE uint16_t

/* 单块内存，内存管理所占用的全部空间大小计算公式如下：
* size=MEM1_MAX_SIZE+(MEM1_MAX_SIZE/MEM1_BLOCK_SIZE)*sizeof(MT_TYPE)
* 以 SRAMIN 为例， size=474*1024+(474*1024/64)*2=500544≈489KB
* 已知总内存容量(size)，最大内存池的计算公式如下：
* MEM1_MAX_SIZE=(MEM1_BLOCK_SIZE*size)/(MEM1_BLOCK_SIZE+sizeof(MT_TYPE))
* 以 SRAM12 为例,MEM1_MAX_SIZE=(64*256)/(64+2)=248.24KB≈248KB
*/
/* mem1 内存参数设定.mem1 是 H7 内部的 AXI 内存 */
#define MEM1_BLOCK_SIZE 64 /* 内存块大小为 64 字节 */
#define MEM1_MAX_SIZE 474*1024/* 最大管理内存 474K,H7 的 AXI 内存 512KB */
#define MEM1_ALLOC_TABLE_SIZE MEM1_MAX_SIZE/MEM1_BLOCK_SIZE /* 内存表大小 */
/* mem2 内存参数设定.mem3 是 H7 内部的 SRAM1+SRAM2 内存 */
#define MEM2_BLOCK_SIZE 64 /* 内存块大小为 64 字节 */
#define MEM2_MAX_SIZE 248*1024/*最大管理内存 248K,H7 的 SRAM1+2 共 256KB*/
#define MEM2_ALLOC_TABLE_SIZE MEM2_MAX_SIZE/MEM2_BLOCK_SIZE /* 内存表大小 */
/* mem3 内存参数设定.mem4 是 H7 内部的 SRAM4 内存 */
#define MEM3_BLOCK_SIZE 64 /* 内存块大小为 64 字节 */
#define MEM3_MAX_SIZE 62*1024 /* 最大管理内存 62K,H7 的 SRAM4 共 64KB */
#define MEM3_ALLOC_TABLE_SIZE MEM3_MAX_SIZE/MEM3_BLOCK_SIZE /* 内存表大小 */
/* mem4 内存参数设定.mem5 是 H7 内部的 DTCM 内存,此部分内存仅 CPU 和 MDMA 可以访问!!!!!! */
#define MEM4_BLOCK_SIZE 64 /* 内存块大小为 64 字节 */
#define MEM4_MAX_SIZE 124*1024 /* 最大管理内存 124K,H7 的 DTCM 共 128KB */
#define MEM4_ALLOC_TABLE_SIZE MEM4_MAX_SIZE/MEM4_BLOCK_SIZE /* 内存表大小 */
/* mem5 内存参数设定.mem6 是 H7 内部的 ITCM 内存,此部分内存仅 CPU 和 MDMA 可以访问!!!!!! */
#define MEM5_BLOCK_SIZE 64 /* 内存块大小为 64 字节 */
#define MEM5_MAX_SIZE 62*1024 /* 最大管理内存 62K,H7 的 ITCM 共 64KB */
#define MEM5_ALLOC_TABLE_SIZE MEM5_MAX_SIZE/MEM5_BLOCK_SIZE /* 内存表大小 */
/* 如果没有定义 NULL, 定义 NULL */
#ifndef NULL
#define NULL 0
#endif

/*内存管理控制器*/
struct _m_malloc_dev
{
    void (*init)(uint8_t);              /*初始化*/
    uint16_t(*perused)(uint8_t);        /*内存使用率*/
    uint8_t* membase[SRAMBANK];         /* 内存池 管理 SRAMBANK 个区域的内存 */
    MT_TYPE* memmap[SRAMBANK];          /* 内存管理状态表 */
    uint8_t memrdy[SRAMBANK];           /* 内存管理是否就绪 */
};

void my_mem_copy(void* des, void* src, uint32_t n);
void my_mem_set(void* s, uint8_t c, uint32_t count);
void my_mem_init(uint8_t memx);
uint16_t my_mem_perused(uint8_t memx);
void* mymalloc(uint8_t memx, uint32_t size);
void myfree(uint8_t memx, void* ptr);
void* myrealloc(uint8_t memx, void* ptr, uint32_t size);


#endif // !__DRV_MALLOC_H

