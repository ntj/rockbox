/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2006 by Linus Nielsen Feltzing
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
#include "system.h"
#include "backlight-target.h"
#include "backlight.h"
#include "lcd.h"
#include "power.h"
#include "mc13783.h"
#include "debug.h"

bool _backlight_init(void)
{
    return true;
}

void _backlight_on(void)
{
    /* LEDEN=1 */
    mc13783_set(MC13783_LED_CONTROL0, (1 << 0));
}

void _backlight_off(void)
{
    /* LEDEN=0 */
    mc13783_clear(MC13783_LED_CONTROL0, (1 << 0));
}

/* Assumes that the backlight has been initialized */
void _backlight_set_brightness(int brightness)
{
    (void)brightness;
}

