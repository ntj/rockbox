/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: button-e200v2.c 19035 2008-11-07 05:31:05Z jdgordon $
 *
 * Copyright (C) 2006 by Barry Wardell
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "system.h"
#include "button-target.h"
#include "button.h"
#include "backlight.h"
#include "powermgmt.h"


unsigned short _dbop_din    = 0;

/* in the lcd driver */
extern bool lcd_button_support(void);

static bool hold_button     = false;
#ifndef BOOTLOADER
static bool hold_button_old = false;
#endif

/* for the debug menu */
unsigned short button_dbop_data(void)
{
    return _dbop_din;
}

void button_init_device(void)
{
    GPIOA_DIR &= ~(1<<3);
}

bool button_hold(void)
{
    return hold_button;
}

static void button_read_dbop(void)
{
    /* Set up dbop for input */
    DBOP_CTRL |= (1<<19);                /* Tri-state DBOP on read cycle */
    DBOP_CTRL &= ~(1<<16);               /* disable output (1:write enabled) */
    DBOP_TIMPOL_01 = 0xe167e167;         /* Set Timing & Polarity regs 0 & 1 */
    DBOP_TIMPOL_23 = 0xe167006e;         /* Set Timing & Polarity regs 2 & 3 */

    DBOP_CTRL |= (1<<15);                /* start read */
    while (!(DBOP_STAT & (1<<16)));      /* wait for valid data */

    _dbop_din = DBOP_DIN;                /* Read dbop data*/

    /* Reset dbop for output */
    DBOP_TIMPOL_01 = 0x6e167;            /* Set Timing & Polarity regs 0 & 1 */
    DBOP_TIMPOL_23 = 0xa167e06f;         /* Set Timing & Polarity regs 2 & 3 */
    DBOP_CTRL |= (1<<16);                /* Enable output (0:write disable)  */
    DBOP_CTRL &= ~(1<<19);               /* Tri-state when no active write */
}

/*
 * Get button pressed from hardware
 */
int button_read_device(void)
{
    int btn = BUTTON_NONE;

    /* direct GPIO connections */
    if (GPIOA_PIN(3))
        btn |= BUTTON_POWER;

    if(lcd_button_support())
        button_read_dbop();

    if(!(_dbop_din & (1<<2)))
        btn |= BUTTON_LEFT;
    if(!(_dbop_din & (1<<3)))
        btn |= BUTTON_DOWN;
    if(!(_dbop_din & (1<<4)))
        btn |= BUTTON_SELECT;
    if(!(_dbop_din & (1<<5)))
        btn |= BUTTON_UP;
    if(!(_dbop_din & (1<<6)))
        btn |= BUTTON_RIGHT;

#ifndef BOOTLOADER
    /* light handling */
    if (hold_button != hold_button_old)
    {
        hold_button_old = hold_button;
        backlight_hold_changed(hold_button);
    }
#endif /* BOOTLOADER */

    return btn;
}
