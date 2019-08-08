/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2019  Sylvain Munaut
 * Written by Sylvain Munaut <tnt@246tNt.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "timing.h"

#ifdef ENABLE_DEBUG
#define PLATFORM_HAS_DEBUG
#endif

#define BOARD_IDENT             "iCEpick"

#define SET_RUN_STATE(state)
#define SET_IDLE_STATE(state)
#define SET_ERROR_STATE(state)

#define DEBUG		printf

#define vasprintf	vasiprintf
#define snprintf	sniprintf
#define sscanf		siscanf

static inline int platform_hwversion(void)
{
                return 0;
}


#define TMS_SET_MODE() do { \
} while(0)

#define SWDIO_MODE_FLOAT() platform_gpio_dir(SWDIO_PIN, false)
#define SWDIO_MODE_DRIVE() platform_gpio_dir(SWDIO_PIN, true)

void platform_gpio_dir(uint8_t bit, bool drive);
void platform_gpio_set(uint8_t bit, bool val);
bool platform_gpio_get(uint8_t bit);

#define gpio_set_val(port, pin, val)	platform_gpio_set((pin), (val))
#define gpio_set(port, pin)		platform_gpio_set((pin), true)
#define gpio_clear(port, pin)		platform_gpio_set((pin), false)
#define gpio_get(port, pin)		platform_gpio_get((pin))

#define TCK_PIN (1 << 3)
#define TMS_PIN	(1 << 4)
#define TDO_PIN (0)
#define TDI_PIN (0)

#define SWCLK_PIN TCK_PIN
#define SWDIO_PIN TMS_PIN


#endif /* __PLATFORM_H */
