#include "./USMART/usmart.h"
#include "./USMART/usmart_str.h"
#include "drv_at24cxx.h"
#include "drv_lcd.h"
#include "exfuns.h"
#include "fattester.h"

/******************************************************************************************/
/* 用户配置区
 * 这下面要包含所用到的函数所申明的头文件(用户自己添加)
 */


 /* 函数名列表初始化(用户自己添加)
  * 用户直接在这里输入要执行的函数名及其查找串
  */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* 如果使能了读写操作 */
    (void*)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void*)write_addr, "void write_addr(uint32_t addr,uint32_t val)",
#endif
    (void*)delay_ms, "void delay_ms(uint16_t nms)",
    (void*)delay_us, "void delay_us(uint32_t nus)",


    (void*)at24cxx_read_one_byte,"uint8_t at24cxx_read_one_byte(uint8_t addr)",
    (void*)at24cxx_write_one_byte,"void at24cxx_write_one_byte(uint8_t addr, uint8_t data)",
    (void*)at24cxx_write,"void at24cxx_write(uint8_t addr, uint8_t* pbuf, uint16_t datalen)",
    (void*)at24cxx_read_umsart,"void at24cxx_read_umsart(uint8_t addr, uint16_t datalen)",
    (void*)lcd_clear, "void lcd_clear(uint16_t color)",
    (void*)lcd_fill, "void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)",
    (void*)lcd_draw_line, "void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)",
    (void*)lcd_draw_hline, "void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)",
    (void*)lcd_draw_rectangle, "void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)",
    (void*)lcd_draw_circle, "void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)",
    (void*)lcd_fill_circle, "void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)",
    (void*)lcd_show_char, "void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)",
    (void*)lcd_show_num, "void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)",
    (void*)lcd_show_xnum, "void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)",
    (void*)lcd_show_string, "void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char* p, uint16_t color)",

    (void*)mf_mount, "uint8_t mf_mount(uint8_t* path,uint8_t mt)",
    (void*)mf_open, "uint8_t mf_open(uint8_t*path,uint8_t mode)",
    (void*)mf_close, "uint8_t mf_close(void)",
    (void*)mf_read, "uint8_t mf_read(uint16_t len)",
    (void*)mf_write, "uint8_t mf_write(uint8_t*dat,uint16_t len)",
    (void*)mf_opendir, "uint8_t mf_opendir(uint8_t* path)",
    (void*)mf_closedir, "uint8_t mf_closedir(void)",
    (void*)mf_readdir, "uint8_t mf_readdir(void)",
    (void*)mf_scan_files, "uint8_t mf_scan_files(uint8_t * path)",
    (void*)mf_showfree, "uint32_t mf_showfree(uint8_t *path)",
    (void*)mf_lseek, "uint8_t mf_lseek(uint32_t offset)",
    (void*)mf_tell, "uint32_t mf_tell(void)",
    (void*)mf_size, "uint32_t mf_size(void)",
    (void*)mf_mkdir, "uint8_t mf_mkdir(uint8_t*path)",
    (void*)mf_fmkfs, "uint8_t mf_fmkfs(uint8_t* path,uint8_t opt,uint16_t au)",
    (void*)mf_unlink, "uint8_t mf_unlink(uint8_t *path)",
    (void*)mf_rename, "uint8_t mf_rename(uint8_t *oldname,uint8_t* newname)",
    (void*)mf_getlabel, "void mf_getlabel(uint8_t *path)",
    (void*)mf_setlabel, "void mf_setlabel(uint8_t *path)",
    (void*)mf_gets, "void mf_gets(uint16_t size)",
    (void*)mf_putc, "uint8_t mf_putc(uint8_t c)",
    (void*)mf_puts, "uint8_t mf_puts(uint8_t *str)",
};
/******************************************************************************************/

/* 函数控制管理器初始化
 * 得到各个受控函数的名字
 * 得到函数总数量
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* 函数数量 */
    0,      /* 参数数量 */
    0,      /* 函数ID */
    1,      /* 参数显示类型,0,10进制;1,16进制 */
    0,      /* 参数类型.bitx:,0,数字;1,字符串 */
    0,      /* 每个参数的长度暂存表,需要MAX_PARM个0初始化 */
    0,      /* 函数的参数,需要PARM_LEN个0初始化 */
};



















