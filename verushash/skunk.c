#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>

#include "sph_skein.h"
#include "sph_cubehash.h"
#include "sph_fugue.h"
#include "sph_gost.h"



/* Move init out of loop, so init once externally,
   and then use one single memcpy with that bigger memory block */
typedef struct {
	sph_skein512_context    skein1;
	sph_cubehash512_context cubehash1;
	sph_fugue512_context    fugue1;
	sph_gost512_context     gost1;
} Xhash_context_holder;

static Xhash_context_holder base_contexts;

static void init_Xhash_contexts()
{
	sph_skein512_init(&base_contexts.skein1);
	sph_cubehash512_init(&base_contexts.cubehash1);
	sph_fugue512_init(&base_contexts.fugue1);
	sph_gost512_init(&base_contexts.gost1);
}

void skunkhash(void *state, const void *input,char* midstate)
{
	init_Xhash_contexts();

	Xhash_context_holder ctx;

	uint32_t hash[16];

	memcpy(&ctx, &base_contexts, sizeof(base_contexts));

	sph_skein512(&ctx.skein1, input, 80);
	memcpy(midstate, &ctx.skein1.h0, 64);
	sph_skein512_close(&ctx.skein1, hash);

	sph_cubehash512(&ctx.cubehash1, hash, 64);
	sph_cubehash512_close(&ctx.cubehash1, hash);

	sph_fugue512(&ctx.fugue1, hash, 64);
	sph_fugue512_close(&ctx.fugue1, hash);

	sph_gost512(&ctx.gost1, hash, 64);
	sph_gost512_close(&ctx.gost1, hash);

	memcpy(state, hash, 32);
}




