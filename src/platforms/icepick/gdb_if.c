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
#include "gdb_if.h"

struct wb_uart {
        uint32_t data;
        uint32_t clkdiv;
} __attribute__((packed,aligned(4)));

static volatile struct wb_uart * const uart_regs = (void*)(0x81000000);


char getchar_prompt(unsigned int timeout)
{
        uint32_t c;

        uint32_t cycles_begin, cycles_now, cycles;
        __asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	do {
                __asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
                cycles = cycles_now - cycles_begin;
                if (timeout && (cycles > timeout)) {
                        cycles_begin = cycles_now;
			return 0xff;
                }
                c = uart_regs->data;
        } while (c & 0x80000000);

        return c;
}


void gdb_if_putchar(unsigned char c, int flush)
{
	/* FIXME */
	(void)flush;
	uart_regs->data = c;
}

unsigned char gdb_if_getchar(void)
{
	/* FIXME */
	return getchar_prompt(0);
}

unsigned char gdb_if_getchar_to(int timeout)
{
	/* FIXME */
	(void)timeout;
	return getchar_prompt(timeout*12*1000);
}

