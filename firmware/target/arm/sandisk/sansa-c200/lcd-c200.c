/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2007 by Mark Arigo
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include "cpu.h"
#include "lcd.h"
#include "kernel.h"
#include "system.h"

/* LCD command set for Samsung S6B33B2 */

#define R_OSCILLATION_MODE     0x02
#define R_DRIVER_OUTPUT_MODE   0x10
#define R_DCDC_SET             0x20
#define R_BIAS_SET             0x22
#define R_DCDC_CLOCK_DIV       0x24
#define R_DCDC_AMP_ONOFF       0x26
#define R_TEMP_COMPENSATION    0x28
#define R_CONTRAST_CONTROL1    0x2a
#define R_CONTRAST_CONTROL2    0x2b
#define R_STANDBY_OFF          0x2c
#define R_STANDBY_ON           0x2d
#define R_DDRAM_BURST_OFF      0x2e
#define R_DDRAM_BURST_ON       0x2f
#define R_ADDRESSING_MODE      0x30
#define R_ROW_VECTOR_MODE      0x32
#define R_N_LINE_INVERSION     0x34
#define R_FRAME_FREQ_CONTROL   0x36
#define R_RED_PALETTE          0x38
#define R_GREEN_PALETTE        0x3a
#define R_BLUE_PALETTE         0x3c
#define R_ENTRY_MODE           0x40
#define R_X_ADDR_AREA          0x42
#define R_Y_ADDR_AREA          0x43
#define R_RAM_SKIP_AREA        0x45
#define R_DISPLAY_OFF          0x50
#define R_DISPLAY_ON           0x51
#define R_SPEC_DISPLAY_PATTERN 0x53
#define R_PARTIAL_DISPLAY_MODE 0x55
#define R_PARTIAL_START_LINE   0x56
#define R_PARTIAL_END_LINE     0x57
#define R_AREA_SCROLL_MODE     0x59
#define R_SCROLL_START_LINE    0x5a
#define R_DATA_FORMAT_SELECT   0x60


/* wait for LCD */
static inline void lcd_wait_write(void)
{
    while (LCD1_CONTROL & LCD1_BUSY_MASK);
}

/* send LCD data */
static void lcd_send_data(unsigned data)
{
    lcd_wait_write();
    LCD1_DATA = data >> 8;
    lcd_wait_write();
    LCD1_DATA = data & 0xff;
}

/* send LCD command */
static void lcd_send_command(unsigned cmd)
{
    lcd_wait_write();
    LCD1_CMD = cmd;
}

/* LCD init */
void lcd_init_device(void)
{
    /* This is from the c200 of bootloader beginning at offset 0xbbf4 */
    outl(inl(0x70000010) & ~0xfc000000, 0x70000010);
    outl(inl(0x70000010), 0x70000010);

    DEV_INIT2 &= ~0x400;
    udelay(10000);
    
    LCD1_CONTROL &= ~0x4;
    udelay(15);

    LCD1_CONTROL |= 0x4;
    udelay(10);

    LCD1_CONTROL = 0x4687;
    udelay(10000);

    lcd_send_command(R_STANDBY_OFF);
    udelay(20000);

    lcd_send_command(R_OSCILLATION_MODE);
    lcd_send_command(0x01);
    udelay(20000);

    lcd_send_command(R_DCDC_AMP_ONOFF);
    lcd_send_command(0x01);
    udelay(20000);

    lcd_send_command(R_DCDC_AMP_ONOFF);
    lcd_send_command(0x09);
    udelay(20000);

    lcd_send_command(R_DCDC_AMP_ONOFF);
    lcd_send_command(0x0b);
    udelay(20000);

    lcd_send_command(R_DCDC_AMP_ONOFF);
    lcd_send_command(0x0f);
    udelay(20000);

    lcd_send_command(R_DRIVER_OUTPUT_MODE);
    lcd_send_command(0x07);

    lcd_send_command(R_DCDC_SET);
    lcd_send_command(0x03);

    lcd_send_command(R_DCDC_CLOCK_DIV);
    lcd_send_command(0x03);

    lcd_send_command(R_TEMP_COMPENSATION);
    lcd_send_command(0x01);

    lcd_send_command(R_CONTRAST_CONTROL1);
    lcd_send_command(0x55);

    lcd_send_command(R_ADDRESSING_MODE);
    lcd_send_command(0x10);

    lcd_send_command(R_ROW_VECTOR_MODE);
    lcd_send_command(0x0e);

    lcd_send_command(R_N_LINE_INVERSION);
    lcd_send_command(0x0d);

    lcd_send_command(R_FRAME_FREQ_CONTROL);
    lcd_send_command(0);

    lcd_send_command(R_ENTRY_MODE);
    lcd_send_command(0x82);

    lcd_send_command(R_Y_ADDR_AREA);         /* vertical dimensions */
    lcd_send_command(0x1a);                  /* y1 + 0x1a */
    lcd_send_command(LCD_HEIGHT - 1 + 0x1a); /* y2 + 0x1a */

    lcd_send_command(R_X_ADDR_AREA);         /* horizontal dimensions */
    lcd_send_command(0);                     /* x1 */
    lcd_send_command(LCD_WIDTH - 1);         /* x2 */
    udelay(100000);

    lcd_send_command(R_DISPLAY_ON);
}

/*** hardware configuration ***/
int lcd_default_contrast(void)
{
    return DEFAULT_CONTRAST_SETTING;
}

void lcd_set_contrast(int val)
{
    lcd_send_command(R_CONTRAST_CONTROL1);
    lcd_send_command(val);
}

void lcd_set_invert_display(bool yesno)
{
    /* TODO: Implement lcd_set_invert_display() */
    (void)yesno;
}

/* turn the display upside down (call lcd_update() afterwards) */
void lcd_set_flip(bool yesno)
{
    lcd_send_command(R_DRIVER_OUTPUT_MODE);
    lcd_send_command(yesno ? 0x02 : 0x07);
}

/*** update functions ***/

/* Performance function that works with an external buffer
   note that by and bheight are in 4-pixel units! */
void lcd_blit(const fb_data* data, int x, int by, int width,
              int bheight, int stride)
{
    /* TODO: Implement lcd_blit() */
    (void)data;
    (void)x;
    (void)by;
    (void)width;
    (void)bheight;
    (void)stride;
}

/* Line write helper function for lcd_yuv_blit. Write two lines of yuv420. */
extern void lcd_write_yuv420_lines(unsigned char const * const src[3],
                                   int width,
                                   int stride);
/* Performance function to blit a YUV bitmap directly to the LCD */
void lcd_yuv_blit(unsigned char * const src[3],
                  int src_x, int src_y, int stride,
                  int x, int y, int width, int height)
{
    unsigned char const * yuv_src[3];
    off_t z;

    /* Sorry, but width and height must be >= 2 or else */
    width &= ~1;
    height >>= 1;
    
    y += 0x1a;

    z = stride*src_y;
    yuv_src[0] = src[0] + z + src_x;
    yuv_src[1] = src[1] + (z >> 2) + (src_x >> 1);
    yuv_src[2] = src[2] + (yuv_src[1] - src[1]);

    lcd_send_command(R_ENTRY_MODE);
    lcd_send_command(0x80);

    lcd_send_command(R_X_ADDR_AREA);
    lcd_send_command(x);
    lcd_send_command(x + width - 1);

    do
    {
        lcd_send_command(R_Y_ADDR_AREA);
        lcd_send_command(y);
        lcd_send_command(y + 1);

        lcd_write_yuv420_lines(yuv_src, width, stride);
        yuv_src[0] += stride << 1; /* Skip down two luma lines */
        yuv_src[1] += stride >> 1; /* Skip down one chroma line */
        yuv_src[2] += stride >> 1;
        y += 2;
    }
    while (--height > 0);
}

/* Update the display.
   This must be called after all other LCD functions that change the display. */
void lcd_update(void)
{
    lcd_update_rect(0, 0, LCD_WIDTH, LCD_HEIGHT);
}

/* Update a fraction of the display. */
void lcd_update_rect(int x0, int y0, int width, int height)
{
    unsigned short *addr;
    int c, r;
    int x1 = (x0 + width) - 1;
    int y1 = (y0 + height) - 1;

    if ((x1 <= 0) || (y1 <= 0))
        return;

    lcd_send_command(R_ENTRY_MODE);
    lcd_send_command(0x82);

    if(y1 >= LCD_HEIGHT)
        y1 = LCD_HEIGHT - 1;

    lcd_send_command(R_Y_ADDR_AREA);
    lcd_send_command(y0 + 0x1a);
    lcd_send_command(y1 + 0x1a);

    if(x1 >= LCD_WIDTH)
        x1 = LCD_WIDTH - 1;

    lcd_send_command(R_X_ADDR_AREA);
    lcd_send_command(x0);
    lcd_send_command(x1);

    addr = (unsigned short*)&lcd_framebuffer[y0][x0];

    /* for each row */
    for (r = 0; r < height; r++) {
        /* for each column */
        for (c = 0; c < width; c++) {
            /* output 1 pixel */
            lcd_send_data(*(addr++));
        }

        addr += LCD_WIDTH - width;
    }
}
