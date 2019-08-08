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

#include "general.h"
#include "platform.h"

#define reg_gpio (*(volatile uint32_t*)0x80000000)


struct wb_uart {
        uint32_t data;
        uint32_t clkdiv;
} __attribute__((packed,aligned(4)));

static volatile struct wb_uart * const uart_regs = (void*)(0x81000000);



void platform_init(void)
{
	uart_regs->clkdiv = 22;
	for (int i=0; i<10; i++)
		uart_regs->data = 'B' + i;

	reg_gpio = (3 << 16) | (3 << 8);
	platform_gpio_dir(SWCLK_PIN, true);
}

const char *platform_target_voltage(void)
{
	return "3v3";
}

void platform_delay(uint32_t ms)
{
	(void)ms;
}

uint32_t platform_time_ms(void)
{
	return 0;
}


void platform_srst_set_val(bool assert)
{
	if (assert)
		reg_gpio = (reg_gpio | (1 << 18)) & ~(1 << 10);
	else
		reg_gpio &= ~(1 << 18);
}

bool platform_srst_get_val(void)
{
	return (reg_gpio & (1 << 2)) == 0;
}

void platform_gpio_dir(uint8_t bit, bool drive)
{
	if (drive)
		reg_gpio |=   (uint32_t)bit << 16;
	else
		reg_gpio &= ~((uint32_t)bit << 16);
}

void platform_gpio_set(uint8_t bit, bool val)
{
	if (val)
		reg_gpio |=   (uint32_t)bit << 8;
	else
		reg_gpio &= ~((uint32_t)bit << 8);
}

bool platform_gpio_get(uint8_t bit)
{
	return (reg_gpio & bit) != 0;
}

