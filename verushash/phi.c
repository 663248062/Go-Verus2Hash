/*-
 * Copyright 2009 Colin Percival, 2011 ArtForz
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
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */

#include "miner.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "sph_skein.h"
#include "sph_jh.h"
#include "sph_cubehash.h"
#include "sph_fugue.h"
#include "sph_gost.h"
#include "sph_echo.h"
#include "lyra2.h"


extern char *bin2hex(const unsigned char *p, size_t len);
void phi2_hash2(void *state, const void *input)
{
	unsigned char hash[64];
	unsigned char hashA[64];
	unsigned char hashB[64];

	sph_cubehash512_context ctx_cubehash;
	sph_cubehash_context midstate;
	sph_jh512_context ctx_jh;
	sph_gost512_context ctx_gost;
	sph_echo512_context ctx_echo;
	sph_skein512_context ctx_skein;

	sph_cubehash512_init(&ctx_cubehash);
	sph_cubehash512_midstate(&ctx_cubehash,&midstate, input, 1 ? 144 : 80);
	sph_cubehash512_close(&ctx_cubehash, (void*)hashB);

	char* tmp = bin2hex(hashB, 64);
	printf("hasB:%s\n", tmp);
	free(tmp);

	LYRA2_phi(&hashA[0], 32, &hashB[0], 32, &hashB[0], 32, 1, 8, 8);
	tmp = bin2hex(hashA, 32);
	printf("hashA[0]:%s\n", tmp);
	free(tmp);
	LYRA2_phi(&hashA[32], 32, &hashB[32], 32, &hashB[32], 32, 1, 8, 8);
	tmp = bin2hex(hashA + 32, 32);
	printf("hashA[32]:%s\n", tmp);
	free(tmp);

	sph_jh512_init(&ctx_jh);
	sph_jh512(&ctx_jh, (const void*)hashA, 64);
	sph_jh512_close(&ctx_jh, (void*)hash);
	tmp = bin2hex(hash, 64);
	printf("hash1:%s\n", tmp);
	free(tmp);

	if (hash[0] & 1) {
		sph_gost512_init(&ctx_gost);
		sph_gost512(&ctx_gost, (const void*)hash, 64);
		sph_gost512_close(&ctx_gost, (void*)hash);
		tmp = bin2hex(hash, 64);
		printf("hash2:%s\n", tmp);
		free(tmp);

	}
	else {
		sph_echo512_init(&ctx_echo);
		sph_echo512(&ctx_echo, (const void*)hash, 64);
		sph_echo512_close(&ctx_echo, (void*)hash);
		tmp = bin2hex(hash, 64);
		printf("hash3:%s\n", tmp);
		free(tmp);

		sph_echo512_init(&ctx_echo);
		sph_echo512(&ctx_echo, (const void*)hash, 64);
		sph_echo512_close(&ctx_echo, (void*)hash);
		tmp = bin2hex(hash, 64);
		printf("hash4:%s\n", tmp);
		free(tmp);
	}

	sph_skein512_init(&ctx_skein);
	sph_skein512(&ctx_skein, (const void*)hash, 64);
	sph_skein512_close(&ctx_skein, (void*)hash);

	tmp = bin2hex(hash, 64);
	printf("hash5:%s\n", tmp);
	free(tmp);

	for (int i = 0; i<4; i++)
		((uint64_t*)hash)[i] ^= ((uint64_t*)hash)[i + 4];

	tmp = bin2hex(hash, 64);
	printf("hash6:%s\n", tmp);
	free(tmp);
	memcpy(state, hash, 32);
}



void phi2_hash(void *state, const void *input)
{
	unsigned char hash[64];
	unsigned char hashA[64];
	unsigned char hashB[64];
	unsigned char hashC[64];

	sph_cubehash_context midstate;
	sph_cubehash512_context ctx_cubehash;
	sph_jh512_context ctx_jh;
	sph_gost512_context ctx_gost;
	sph_echo512_context ctx_echo;
	sph_skein512_context ctx_skein;
	print_bin_reverse(input,80);
	sph_cubehash512_init(&ctx_cubehash);
	//sph_cubehash512(&ctx_cubehash, input, has_roots ? 144 : 80);
	sph_cubehash512_midstate(&ctx_cubehash, &midstate, input,  144 );
	sph_cubehash512_close(&ctx_cubehash, (void*)hashB);
	//print_bin_reverse(&hashB[0], 32);
	LYRA2_old(&hashA[0], 32, &hashB[0], 32, &hashB[0], 32, 1, 8, 8);
	//print_bin_reverse(&hashA[0],32);
	LYRA2_old(&hashA[32], 32, &hashB[32], 32, &hashB[32], 32, 1, 8, 8);
	//print_bin_reverse(&hashA, 64);
	sph_jh512_init(&ctx_jh);
	sph_jh512(&ctx_jh, (const void*)hashA, 64);
	sph_jh512_close(&ctx_jh, (void*)hash);
	//print_bin_reverse(&hash, 64);
	if (hash[0] & 1) {
		sph_gost512_init(&ctx_gost);
		sph_gost512(&ctx_gost, (const void*)hash, 64);
		sph_gost512_close(&ctx_gost, (void*)hash);
		//print_bin_reverse(&hash, 64);
	}
	else {
		sph_echo512_init(&ctx_echo);
		sph_echo512(&ctx_echo, (const void*)hash, 64);
		sph_echo512_close(&ctx_echo, (void*)hash);

		sph_echo512_init(&ctx_echo);
		sph_echo512(&ctx_echo, (const void*)hash, 64);
		sph_echo512_close(&ctx_echo, (void*)hash);
	}

	sph_skein512_init(&ctx_skein);
	sph_skein512(&ctx_skein, (const void*)hash, 64);
	sph_skein512_close(&ctx_skein, (void*)hash);
	//print_bin_reverse(&hash, 64);
	for (int i = 0; i<4; i++)
		((uint64_t*)hash)[i] ^= ((uint64_t*)hash)[i + 4];

	memcpy(state, hash, 32);
}

void phihash(void *input, const void *state)
{
    sph_skein512_context    ctx_skein;
    sph_jh512_context       ctx_jh;
    sph_cubehash512_context ctx_cubehash;
    sph_fugue512_context    ctx_fugue;
    sph_gost512_context     ctx_gost;
    sph_echo512_context     ctx_echo;

    uint32_t hashA[16];

    sph_skein512_init(&ctx_skein);
    sph_skein512 (&ctx_skein, input, 80);
    sph_skein512_close (&ctx_skein, hashA);

    sph_jh512_init(&ctx_jh);
    sph_jh512 (&ctx_jh, hashA, 64);
    sph_jh512_close(&ctx_jh, hashA);

    sph_cubehash512_init(&ctx_cubehash);
    sph_cubehash512 (&ctx_cubehash, hashA, 64);
    sph_cubehash512_close(&ctx_cubehash, hashA);

    sph_fugue512_init(&ctx_fugue);
    sph_fugue512 (&ctx_fugue, hashA, 64);
    sph_fugue512_close(&ctx_fugue, hashA);

    sph_gost512_init(&ctx_gost);
    sph_gost512 (&ctx_gost, hashA, 64);
    sph_gost512_close(&ctx_gost, hashA);

    sph_echo512_init(&ctx_echo);
    sph_echo512 (&ctx_echo, hashA, 64);
    sph_echo512_close(&ctx_echo, hashA);

    memcpy(state, hashA, 32);

}



