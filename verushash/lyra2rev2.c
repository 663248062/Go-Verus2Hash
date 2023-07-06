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
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>

#include "sph_blake.h"
#include "sph_groestl.h"
#include "sph_skein.h"
#include "sph_keccak.h" 
#include "sph_bmw.h"
#include "sph_cubehash.h"
#include "lyra2.h"
void lyra2rev2hash(void* state, const void *input)
{
	sph_blake256_context     ctx_blake;
	sph_bmw256_context       ctx_bmw;
	sph_keccak256_context    ctx_keccak;
	sph_skein256_context     ctx_skein;
	sph_cubehash256_context  ctx_cube;
	uint32_t hashA[8], hashB[8];

	printf("input:");
	print_bin(input, 80);

	sph_blake256_init(&ctx_blake);
	sph_blake256(&ctx_blake, input, 80);
	sph_blake256_close(&ctx_blake, hashA);

	printf("blake:");
	print_bin(hashA, 32);

	sph_keccak256_init(&ctx_keccak);
	sph_keccak256(&ctx_keccak, hashA, 32);
	sph_keccak256_close(&ctx_keccak, hashB);

	printf("kecc:");
	print_bin(hashB, 32);

	sph_cubehash256_init(&ctx_cube);
	sph_cubehash256(&ctx_cube, hashB, 32);
	sph_cubehash256_close(&ctx_cube, hashA);

	printf("cube:");
	print_bin(hashA, 32);

	LYRA2(hashB, 32, hashA, 32, hashA, 32, 1, 4, 4);
	printf("lyra2:");
	print_bin(hashB, 32);

	sph_skein256_init(&ctx_skein);
	sph_skein256(&ctx_skein, hashB, 32);
	sph_skein256_close(&ctx_skein, hashA);

	printf("skein:");
	print_bin(hashA, 32);

	sph_cubehash256_init(&ctx_cube);
	sph_cubehash256(&ctx_cube, hashA, 32);
	sph_cubehash256_close(&ctx_cube, hashB);

	printf("cube2:");
	print_bin(hashB, 32);

	sph_bmw256_init(&ctx_bmw);
	sph_bmw256(&ctx_bmw, hashB, 32);
	sph_bmw256_close(&ctx_bmw, hashA);
	printf("bmw:");
	print_bin(hashA, 32);

	//printf("cpu hash %08x %08x %08x %08x\n",hashA[0],hashA[1],hashA[2],hashA[3]);

	memcpy(state, hashA, 32);
}

static const uint32_t diff1targ = 0x0000ffff;


static void be32enc_vect(uint32_t *dst, const uint32_t *src, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		dst[i] = htobe32(src[i]);
}

void lyra2rev2_regenhash(char* tdata,uint32_t nonce,char *ohash)
{
	//unsigned char data[256];
        uint32_t data[20];
	//	char ohash[32];
		char temp[200];

        be32enc_vect(data, (const uint32_t *)tdata, 19);
        data[19] = htobe32(nonce);
		
		printf("final data:");
		print_bin((uint8_t*)(data), 80);
        lyra2rev2hash(ohash, data);
}


void lyra2rev2_regenhash_by_midstate(char* tdata, uint32_t nonce, char *ohash) {
	sph_blake256_context ctx;
	uint32_t data[20];
	be32enc_vect(data, (const uint32_t *)tdata, 19);
	data[19] = htobe32(nonce);

	sph_blake256_context blake_mid;
	sph_blake256_init(&blake_mid);
	sph_blake256(&blake_mid, data, 64);
	printf("midstate:");
	print_bin((uint8_t*)(blake_mid.H), 32);



	//sph_blake256_context     ctx_blake;
	sph_bmw256_context       ctx_bmw;
	sph_keccak256_context    ctx_keccak;
	sph_skein256_context     ctx_skein;
	sph_cubehash256_context  ctx_cube;
	uint32_t hashA[8], hashB[8];

	memcpy(&ctx, &blake_mid, sizeof(blake_mid));
	sph_blake256(&ctx, &data[16], 16);
	sph_blake256_close(&ctx, hashA);

	sph_keccak256_init(&ctx_keccak);
	sph_keccak256(&ctx_keccak, hashA, 32);
	sph_keccak256_close(&ctx_keccak, hashB);

	sph_cubehash256_init(&ctx_cube);
	sph_cubehash256(&ctx_cube, hashB, 32);
	sph_cubehash256_close(&ctx_cube, hashA);

	LYRA2(hashB, 32, hashA, 32, hashA, 32, 1, 4, 4);

	sph_skein256_init(&ctx_skein);
	sph_skein256(&ctx_skein, hashB, 32);
	sph_skein256_close(&ctx_skein, hashA);

	sph_cubehash256_init(&ctx_cube);
	sph_cubehash256(&ctx_cube, hashA, 32);
	sph_cubehash256_close(&ctx_cube, hashB);

	sph_bmw256_init(&ctx_bmw);
	sph_bmw256(&ctx_bmw, hashB, 32);
	sph_bmw256_close(&ctx_bmw, hashA);

	//printf("cpu hash %08x %08x %08x %08x\n",hashA[0],hashA[1],hashA[2],hashA[3]);

	memcpy(ohash, hashA, 32);
}



