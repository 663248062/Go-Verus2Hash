/*-
 * Copyright 2014 James Lovejoy
 * Copyright 2014 phm
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "miner.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "sph_blake.h"
#include "sph_groestl.h"
#include "sph_skein.h"
#include "sph_keccak.h" 
#include "sph_bmw.h"
#include "sph_cubehash.h"
#include "lyra2.h"

/*
 * Encode a length len/4 vector of (uint32_t) into a length len vector of
 * (unsigned char) in big-endian form.  Assumes len is a multiple of 4.
 */
static inline void
be32enc_vect(uint32_t *dst, const uint32_t *src, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		dst[i] = htobe32(src[i]);
}


void lyra2Zhash(void *input, const void *state)
{
    sph_blake256_context     ctx_blake;
 
    uint32_t hashA[8], hashB[8];

    sph_blake256_init(&ctx_blake);
    sph_blake256 (&ctx_blake, input, 80);
    sph_blake256_close (&ctx_blake, hashA);
	printf("blake256:");
	print_bin(hashA, 32);
//	printf("cpu hashA %08x %08x %08x %08x  %08x %08x %08x %08x\n",
//		hashA[0], hashA[1], hashA[2], hashA[3], hashA[4], hashA[5], hashA[6], hashA[7]);

	LYRA2(hashB, 32, hashA, 32, hashA, 32, 8, 8, 8);

//printf("cpu hashB %08x %08x %08x %08x  %08x %08x %08x %08x\n",
//hashB[0],hashB[1],hashB[2],hashB[3], hashB[4], hashB[5], hashB[6], hashB[7]);

	memcpy(state, hashB, 32);
}

void lyra2ZhashWithMidstate(void *buf, const void *state)
{
	sph_blake256_context     ctx_blake;

	sph_blake256_context ctx;

	uint32_t hashA[8], hashB[8];

	uint8_t midstate[48] = { 0 };

	sph_blake256_context blake_mid;
	sph_blake256_init(&blake_mid);
	sph_blake256(&blake_mid, buf, 64);
	printf("midstate:");
	print_bin((uint8_t*)(blake_mid.H), 32);

	memcpy(&ctx, &blake_mid, sizeof(blake_mid));
	sph_blake256(&ctx, (char*)buf+64, 16);
	sph_blake256_close(&ctx, hashA);

	printf("blake256:");
	print_bin(hashA, 32);

	//	printf("cpu hashA %08x %08x %08x %08x  %08x %08x %08x %08x\n",
	//		hashA[0], hashA[1], hashA[2], hashA[3], hashA[4], hashA[5], hashA[6], hashA[7]);

	LYRA2(hashB, 32, hashA, 32, hashA, 32, 8, 8, 8);

	//printf("cpu hashB %08x %08x %08x %08x  %08x %08x %08x %08x\n",
	//hashB[0],hashB[1],hashB[2],hashB[3], hashB[4], hashB[5], hashB[6], hashB[7]);

	memcpy(state, hashB, 32);
}

static const uint32_t diff1targ = 0x0000ffff;



