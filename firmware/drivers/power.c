/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 by Linus Nielsen Feltzing
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "sh7034.h"
#include <stdbool.h>
#include "config.h"
#include "adc.h"
#include "kernel.h"
#include "power.h"

#ifdef HAVE_CHARGE_CTRL
bool charger_enabled = 0;
#endif

#ifndef SIMULATOR

bool charger_inserted(void)
{
#ifdef ARCHOS_RECORDER
    return adc_read(ADC_EXT_POWER) > 0x100;
#else
    return (PADR & 1) == 0;
#endif
}

void charger_enable(bool on)
{
#ifdef HAVE_CHARGE_CTRL
    if(on) {
        PBDR &= ~0x20;
        charger_enabled = 1;
    } else {
        PBDR |= 0x20;
        charger_enabled = 0;
    }
#else
    on = on;
#endif
}

void ide_power_enable(bool on)
{
#ifdef ARCHOS_RECORDER
    if(on)
        PADR |= 0x20;
    else
        PADR &= ~0x20;
#else
    on = on;
#endif
}

void power_off(void)
{
    set_irq_level(15);
#ifdef ARCHOS_RECORDER
    PBDR &= ~PBDR_BTN_OFF;
    PBIOR |= PBDR_BTN_OFF;
#else
    PADR &= ~0x800;
    PAIOR |= 0x800;
#endif
    while(1);
}

#else

bool charger_inserted(void)
{
    return false;
}

void charger_enable(bool on)
{
    on = on;
}

void power_off(void)
{
}

#endif /* SIMULATOR */
