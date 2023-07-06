#pragma once
#ifndef BCD_H
#define BCD_H

#include "miner.h"
#include "sph_blake.h"
#include "sph_bmw.h"
#include "sph_groestl.h"
#include "sph_skein.h"
#include "sph_jh.h"
#include "sph_keccak.h"
#include "sph_cubehash.h"
#include "sph_shavite.h"
#include "sph_simd.h"
#include "sph_echo.h"
#include "sph_hamsi.h"
#include "sph_fugue.h"
#include "sph_sm3.h"

typedef struct {
	sph_blake512_context    blake1;
	sph_bmw512_context      bmw1;
	sph_groestl512_context  groestl1;
	sph_skein512_context    skein1;
	sph_jh512_context       jh1;
	sph_keccak512_context   keccak1;
	// sph_luffa512_context    luffa1;
	sph_cubehash512_context cubehash1;
	sph_shavite512_context  shavite1;
	sph_simd512_context     simd1;
	sph_echo512_context     echo1;
	sph_hamsi512_context    hamsi1;
	sph_fugue512_context    fugue1;
	sph_sm3_context         sm3_ctx;
} Dhash_context_holder;

extern void bcdhash(void *state, const void *input);
#endif /* HSR_H */