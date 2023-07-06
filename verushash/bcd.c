#include "bcd.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>
static Dhash_context_holder base_contexts;
void init_Dhash_contexts()
{
	sph_blake512_init(&base_contexts.blake1);
	sph_bmw512_init(&base_contexts.bmw1);
	sph_groestl512_init(&base_contexts.groestl1);
	sph_skein512_init(&base_contexts.skein1);
	sph_jh512_init(&base_contexts.jh1);
	sph_keccak512_init(&base_contexts.keccak1);
	// sph_luffa512_init(&base_contexts.luffa1);
	sph_cubehash512_init(&base_contexts.cubehash1);
	sph_shavite512_init(&base_contexts.shavite1);
	sph_simd512_init(&base_contexts.simd1);
	sph_echo512_init(&base_contexts.echo1);
	sph_hamsi512_init(&base_contexts.hamsi1);
	sph_fugue512_init(&base_contexts.fugue1);

	sph_sm3_init(&base_contexts.sm3_ctx);
}

void bcdhash(void *state, const void *input)
{
	init_Dhash_contexts();

	Dhash_context_holder ctx;

	uint32_t hashA[16], hashB[16];
	//blake-bmw-groestl-sken-jh-meccak-sm3-cubehash-shivite-simd-echo-hamsi-fugue
	memcpy(&ctx, &base_contexts, sizeof(base_contexts));

	sph_blake512(&ctx.blake1, input, 80);
	sph_blake512_close(&ctx.blake1, hashA);

	sph_bmw512(&ctx.bmw1, hashA, 64);
	sph_bmw512_close(&ctx.bmw1, hashB);

	sph_groestl512(&ctx.groestl1, hashB, 64);
	sph_groestl512_close(&ctx.groestl1, hashA);

	sph_skein512(&ctx.skein1, hashA, 64);
	sph_skein512_close(&ctx.skein1, hashB);

	sph_jh512(&ctx.jh1, hashB, 64);
	sph_jh512_close(&ctx.jh1, hashA);

	sph_keccak512(&ctx.keccak1, hashA, 64);
	sph_keccak512_close(&ctx.keccak1, hashB);

	memset(hashA, 0, 64);
	sph_sm3_update(&ctx.sm3_ctx, hashB, 64);
	sph_sm3_close(&ctx.sm3_ctx, hashA);

	sph_cubehash512(&ctx.cubehash1, hashA, 64);
	sph_cubehash512_close(&ctx.cubehash1, hashB);

	sph_shavite512(&ctx.shavite1, hashB, 64);
	sph_shavite512_close(&ctx.shavite1, hashA);

	sph_simd512(&ctx.simd1, hashA, 64);
	sph_simd512_close(&ctx.simd1, hashB);

	sph_echo512(&ctx.echo1, hashB, 64);
	sph_echo512_close(&ctx.echo1, hashA);

	sph_hamsi512(&ctx.hamsi1, hashA, 64);
	sph_hamsi512_close(&ctx.hamsi1, hashB);

	sph_fugue512(&ctx.fugue1, hashB, 64);
	sph_fugue512_close(&ctx.fugue1, hashA);


	memcpy(state, hashA, 32);
}