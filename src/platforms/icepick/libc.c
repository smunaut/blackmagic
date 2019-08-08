
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

//#include "mini-printf.h"


struct wb_uart {
        uint32_t data;
        uint32_t clkdiv;
} __attribute__((packed,aligned(4)));

static volatile struct wb_uart * const uart_regs = (void*)(0x04000000);




int putchar(int c)
{
	uart_regs->data = c & 0xff;
	return c;
}

int puts(const char *p)
{
	char c;
	while ((c = *(p++)) != 0x00) {
//		if (c == '\n')
//			uart_regs->data = '\r';
		uart_regs->data = c & 0xff;
	}

	return 0;
}

#if 1
static char _printf_buf[128];

int printf(const char *fmt, ...)
{
        va_list va;
        int l;

        va_start(va, fmt);
        l = vsniprintf(_printf_buf, sizeof(_printf_buf), fmt, va);
        va_end(va);

	//puts(_printf_buf);

	return l;
}
#endif

void abort() { while(1); };


#include <errno.h>

#define STACK_BUFFER  4096     /* Reserved stack space in bytes. */

void *_sbrk (int nbytes)
{
	/* Symbol defined by linker map */
	extern int  _heap_start;              /* start of free memory (as symbol) */

	/* Value set by crt0.S */
//	extern void *stack;           /* end of free memory */

	/* The statically held previous end of the heap, with its initialization. */
	static void *heap_ptr = (void *)&_heap_start;         /* Previous end */

//	if ((stack - (heap_ptr + nbytes)) > STACK_BUFFER )
	if (1)
	{
		void *base  = heap_ptr;
		heap_ptr   += nbytes;

		return  base;
	}
	else
	{
		errno = ENOMEM;
		return  (void *) -1;
	}
} 
