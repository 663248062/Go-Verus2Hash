/*
 * BLAKE implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 *
 * Modified for more speed by BlueDragon747 for the Blakecoin project
 */

#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "miner.h"
#include "sph_blake.h"
#include "blake256.h"

static const uint32_t diff1targ_blake256 = 0x000000ff;
static const uint32_t diff1targ = 0x0000ffff;

static void be32enc_vect(uint32_t *dst, const uint32_t *src, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		dst[i] = htobe32(src[i]);
}

void blake256hash(const void *input, void *state)
{
  sph_blake256_context ctx_blake;
  sph_blake256_init(&ctx_blake);
  sph_blake256(&ctx_blake, input, 80);
  sph_blake256_close(&ctx_blake, state);
}

void blake256_midstate(unsigned char* tdata,char * midstate)
{
  //unsigned char data[256]
  //char midstate[32];
  sph_blake256_context     ctx_blake;
  uint32_t data[20];

  be32enc_vect((uint32_t *)data, (uint32_t *)tdata, 19);//19 int32 flip to bedian

  sph_blake256_init(&ctx_blake);
  sph_blake256 (&ctx_blake, (unsigned char *)data, 64);// 16 int32 do blake256

  memcpy(midstate, ctx_blake.H, 32);//store midstate
  //endian_flip32(work->midstate, work->midstate);
}

void blake256_midstate2(unsigned char* tdata, char * midstate)
{
	//unsigned char data[256]
	//char midstate[32];
	sph_blake256_context     ctx_blake;
	uint32_t data[20];

	//be32enc_vect((uint32_t *)data, (uint32_t *)tdata, 19);//19 int32 flip to bedian
	memcpy((uint8_t*)data, (uint8_t*)tdata,64);
	sph_blake256_init(&ctx_blake);
	sph_blake256(&ctx_blake, (unsigned char *)data, 64);// 16 int32 do blake256

	memcpy(midstate, ctx_blake.H, 32);//store midstate
									  //endian_flip32(work->midstate, work->midstate);
}



