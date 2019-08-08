#include <stdint.h>

#include "general.h"

int
swdptap_init(void)
{
	return 0;
}

static bool driven = false;

uint32_t
swdptap_seq_in(int ticks)
{
	uint32_t rv;

	asm volatile (
		/* First off load pointer to GPIO and get current state */
		"li t0, 0x80000000\n"
		"lw t1, 0(t0)\n"

		/* Set return value and bit mask */
		"li t3, 1\n"
		"li %[rv], 0\n"

		/* If we're driven, we need a turn-around cycle */
		"beq zero, %[driven], 1f\n"

			/* Disable SWDIO driver and raise SWDCLK */
		"li t2, 0xffefffff\n"
		"and t1, t1, t2\n"
		"li t2, 0x00000800\n"
		"or t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Lower SWDCLK */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

		/* Main shift in loop */
		"1:"
			/* Load pin data */
		"li t2, 0x00000010\n"
		"lw t1, 0(t0)\n"
		"and t2, t1, t2\n"
		"snez t2, t2\n"
		"sub t2, zero, t2\n"
		"and t2, t2, t3\n"
		"or %[rv], %[rv], t2\n"

			/* Set Clock high */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Shift bitmask */
		"slli t3, t3, 1\n"

			/* Set Clock low */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Decount ticks */
		"addi %[ticks], %[ticks], -1\n"
		"bne zero, %[ticks], 1b\n"

		: [rv] "=&r" (rv), [ticks] "+r" (ticks)
		: [driven] "r" (driven)
		: "t0", "t1", "t2", "t3"
	);

	driven = false;
	
	return rv;
}

bool
swdptap_seq_in_parity(uint32_t *ret, int ticks)
{
	uint32_t rv;
	uint32_t parity;

	asm volatile (
		/* First off load pointer to GPIO and get current state */
		"li t0, 0x80000000\n"
		"lw t1, 0(t0)\n"

		/* Set return value and bit mask and parity */
		"li t3, 1\n"
		"li %[rv], 0\n"
		"li %[parity], 0\n"

		/* If we're driven, we need a turn-around cycle */
		"beq zero, %[driven], 1f\n"

			/* Disable SWDIO driver and raise SWDCLK */
		"li t2, 0xffefffff\n"
		"and t1, t1, t2\n"
		"li t2, 0x00000800\n"
		"or t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Lower SWDCLK */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

		/* Main shift in loop */
		"1:"
			/* Load pin data */
		"li t2, 0x00000010\n"
		"lw t1, 0(t0)\n"
		"xor %[parity], %[parity], t1\n"	/* parity update */
		"and t2, t2, t1\n"
		"snez t2, t2\n"
		"sub t2, zero, t2\n"
		"and t2, t2, t3\n"
		"or %[rv], %[rv], t2\n"

			/* Set Clock high */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Shift bitmask */
		"slli t3, t3, 1\n"

			/* Set Clock low */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Decount ticks */
		"addi %[ticks], %[ticks], -1\n"
		"bge %[ticks], zero, 1b\n"

		/* Cleanup parity */
			/* Keep parity bit */
		"li t2, 0x00000010\n"
		"and %[parity], %[parity], t2\n"
		"snez %[parity], %[parity]\n"

			/* Remove the parity bit from return value */
		"srli t3, t3, 1\n"
		"addi t3, t3, -1\n"
		"and %[rv], %[rv], t3\n"

		: [rv] "=&r" (rv), [parity] "=&r"(parity), [ticks] "+r" (ticks)
		: [driven] "r" (driven)
		: "t0", "t1", "t2", "t3", "t4"
	);

	driven = false;

	*ret = rv;

	return parity;
}

void
swdptap_seq_out(uint32_t MS, int ticks)
{
	asm volatile (
		/* First off load pointer to GPIO and get current state */
		"li t0, 0x80000000\n"
		"lw t1, 0(t0)\n"

		/* If we're not driven, we need a turn-around cycle */
		"bne zero, %[driven], 1f\n"
			/* Enable SWDIO driver and raise SWDCLK */
		"li t2, 0x00100800\n"
		"or t1, t1, t2\n"
		"sw t1, 0(t0)\n"
			/* Lower SWDCLK */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

		/* Main shift out loop */
		"1:"
			/* Set SWDIO to the bit value */
		"li t3, 0x00001000\n"
		"or t1, t1, t3\n"
		"and t2, %[data], 1\n"
		"xor t2, t2, 1\n"
		"sub t2, zero, t2\n"
		"and t2, t2, t3\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Set Clock high */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Shift data */
		"srli %[data], %[data], 1\n"

			/* Set Clock low */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Decount ticks */
		"addi %[ticks], %[ticks], -1\n"
		"bne zero, %[ticks], 1b\n"

		: [data] "+r" (MS), [ticks] "+r" (ticks)
		: [driven] "r" (driven)
		: "t0", "t1", "t2", "t3"
	);

	driven = true;
}

void
swdptap_seq_out_parity(uint32_t MS, int ticks)
{
	asm volatile (
		/* Reset parity (we need it inverted, so init to 1) */
		"li t4, 1\n"

		/* First off load pointer to GPIO and get current state */
		"li t0, 0x80000000\n"
		"lw t1, 0(t0)\n"

		/* If we're not driven, we need a turn-around cycle */
		"bne zero, %[driven], 1f\n"
			/* Enable SWDIO driver and raise SWDCLK */
		"li t2, 0x00100800\n"
		"or t1, t1, t2\n"
		"sw t1, 0(t0)\n"
			/* Lower SWDCLK */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

		/* Main shift out loop */
		"1:"
			/* Set SWDIO to the bit value */
		"li t3, 0x00001000\n"
		"or t1, t1, t3\n"
		"and t2, %[data], 1\n"
		"xor t4, t4, t2\n"	/* parity update */
		"xor t2, t2, 1\n"
		"sub t2, zero, t2\n"
		"and t2, t2, t3\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Set Clock high */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Shift data */
		"srli %[data], %[data], 1\n"

			/* Set Clock low */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Decount ticks */
		"addi %[ticks], %[ticks], -1\n"
		"bne zero, %[ticks], 1b\n"

		/* Shift out parity */
			/* Set SWDIO to the parity value */
		"li t3, 0x00001000\n"
		"or t1, t1, t3\n"
		"sub t2, zero, t4\n"
		"and t2, t2, t3\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Set Clock high */
		"li t2, 0x00000800\n"
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

			/* Set Clock low */
		"xor t1, t1, t2\n"
		"sw t1, 0(t0)\n"

		: [data] "+r" (MS), [ticks] "+r" (ticks)
		: [driven] "r" (driven)
		: "t0", "t1", "t2", "t3", "t4"
	);

	driven = true;
}
