#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>
#include "lbry.h"
#include "lyra2rev2.h"
#include "lyra2Z.h"
#include "phi.h"
#include "cryptonight.h"
#include "maxcoin.h"
#include "tribus.h"
#include "sph_sha2.h"
#include "skein.h"
#include "KeccakHash.h"
#include "haraka_portable.h"

void __bin2hex(char *s, const unsigned char *p, size_t len)
{
	int i;
	static const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	for (i = 0; i < (int)len; i++)
	{
		*s++ = hex[p[i] >> 4];
		*s++ = hex[p[i] & 0xF];
	}
	*s++ = '\0';
}

void __bin2hex_reverse(char *s, const unsigned char *p, size_t len)
{
	int i;
	static const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	for (i = (int)len-1; i >= 0; i--)
	{
		*s++ = hex[p[i] >> 4];
		*s++ = hex[p[i] & 0xF];
	}
	*s++ = '\0';
}

char *bin2hex_reverse(const unsigned char *p, size_t len)
{
	size_t slen;
	char *s;

	slen = len * 2 + 1;
	if (slen % 4)
		slen += 4 - (slen % 4);
	s = calloc(slen, 1);

	__bin2hex_reverse(s, p, len);

	return s;
}

/* Returns a malloced array string of a binary value of arbitrary length. The
* array is rounded up to a 4 byte size to appease architectures that need
* aligned array  sizes */
char *bin2hex(const unsigned char *p, size_t len)
{
	size_t slen;
	char *s;

	slen = len * 2 + 1;
	if (slen % 4)
		slen += 4 - (slen % 4);
	s = calloc(slen, 1);

	__bin2hex(s, p, len);

	return s;
}

static const int hex2bin_tbl[256] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

bool hex2bin(unsigned char *p, const char *hexstr, size_t len)
{
	int nibble1, nibble2;
	unsigned char idx;
	bool ret = false;

	while (*hexstr && len)
	{


		idx = *hexstr++;
		nibble1 = hex2bin_tbl[idx];
		idx = *hexstr++;
		nibble2 = hex2bin_tbl[idx];

		*p++ = (((unsigned char)nibble1) << 4) | ((unsigned char)nibble2);
		--len;
	}

	if (len == 0 && *hexstr == 0)
		ret = true;
	return ret;
}
void print_bin(uint8_t * cmd, size_t len)
{
	char *hex_buff = NULL;
	hex_buff = bin2hex(cmd, len);
	printf("%s\n", hex_buff);
	free(hex_buff);
	hex_buff = NULL;
}

void print_bin_reverse(uint8_t * cmd, size_t len)
{
	char *hex_buff = NULL;
	hex_buff = bin2hex_reverse(cmd, len);
	printf("%s\n", hex_buff);
	free(hex_buff);
	hex_buff = NULL;
}

static void be32enc_vect(uint32_t *dst, const uint32_t *src, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		dst[i] = htobe32(src[i]);
}



#ifndef ROTR64
#define ROTR64(x, y)  (((x) >> (y)) ^ ((x) << (64 - (y))))
#endif

// Little-endian byte access.

#define B2B_GET64(p)                            \
    (((uint64_t) ((uint8_t *) (p))[0]) ^        \
    (((uint64_t) ((uint8_t *) (p))[1]) << 8) ^  \
    (((uint64_t) ((uint8_t *) (p))[2]) << 16) ^ \
    (((uint64_t) ((uint8_t *) (p))[3]) << 24) ^ \
    (((uint64_t) ((uint8_t *) (p))[4]) << 32) ^ \
    (((uint64_t) ((uint8_t *) (p))[5]) << 40) ^ \
    (((uint64_t) ((uint8_t *) (p))[6]) << 48) ^ \
    (((uint64_t) ((uint8_t *) (p))[7]) << 56))

// G Mixing function.

#define B2B_G(a, b, c, d, x, y) {   \
    v[a] = v[a] + v[b] + x;         \
    v[d] = ROTR64(v[d] ^ v[a], 32); \
    v[c] = v[c] + v[d];             \
    v[b] = ROTR64(v[b] ^ v[c], 24); \
    v[a] = v[a] + v[b] + y;         \
    v[d] = ROTR64(v[d] ^ v[a], 16); \
    v[c] = v[c] + v[d];             \
    v[b] = ROTR64(v[b] ^ v[c], 63); }

// Initialization Vector.

static const uint64_t blake2b_iv[8] =
{
	0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
	0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
	0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
	0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179
};

// state context
typedef struct
{
	uint8_t b[128];                     // input buffer
	uint64_t h[8];                      // chained state
	uint64_t t[2];                      // total number of bytes
	size_t c;                           // pointer for b[]
	size_t outlen;                      // digest size
} blake2b_ctx;

void blake2b_update(blake2b_ctx* ctx, // context
	const void* in, size_t inlen);     // data to be hashed

									   // Compression function. "last" flag indicates last block.

static void blake2b_compress(blake2b_ctx* ctx, int last)
{
	const uint8_t sigma[12][16] =
	{
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
		{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 },
		{ 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 },
		{ 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 },
		{ 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 },
		{ 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 },
		{ 12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11 },
		{ 13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10 },
		{ 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5 },
		{ 10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
		{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 }
	};
	int i;
	uint64_t v[16], m[16];

	for (i = 0; i < 8; i++)           // init work variables
	{
		v[i] = ctx->h[i];
		v[i + 8] = blake2b_iv[i];
	}

	v[12] ^= ctx->t[0];                 // low 64 bits of offset
	v[13] ^= ctx->t[1];                 // high 64 bits
	if (last)                         // last block flag set ?
	{
		v[14] = ~v[14];
	}

	for (i = 0; i < 16; i++)          // get little-endian words
	{
		m[i] = B2B_GET64(&ctx->b[8 * i]);
	}

	for (i = 0; i < 12; i++)          // twelve rounds
	{
		B2B_G(0, 4, 8, 12, m[sigma[i][0]], m[sigma[i][1]]);
		B2B_G(1, 5, 9, 13, m[sigma[i][2]], m[sigma[i][3]]);
		B2B_G(2, 6, 10, 14, m[sigma[i][4]], m[sigma[i][5]]);
		B2B_G(3, 7, 11, 15, m[sigma[i][6]], m[sigma[i][7]]);
		B2B_G(0, 5, 10, 15, m[sigma[i][8]], m[sigma[i][9]]);
		B2B_G(1, 6, 11, 12, m[sigma[i][10]], m[sigma[i][11]]);
		B2B_G(2, 7, 8, 13, m[sigma[i][12]], m[sigma[i][13]]);
		B2B_G(3, 4, 9, 14, m[sigma[i][14]], m[sigma[i][15]]);
	}

	for (i = 0; i < 8; ++i)
	{
		ctx->h[i] ^= v[i] ^ v[i + 8];
	}
}

// Initialize the hashing context "ctx" with optional key "key".
//      1 <= outlen <= 64 gives the digest size in bytes.
//      Secret key (also <= 64 bytes) is optional (keylen = 0).

int blake2b_init(blake2b_ctx* ctx, size_t outlen,
	const void* key, size_t keylen)       // (keylen=0: no key)
{
	size_t i;
	if (outlen == 0 || outlen > 64 || keylen > 64)
	{
		return -1;    // illegal parameters
	}

	for (i = 0; i < 8; i++)           // state, "param block"
	{
		ctx->h[i] = blake2b_iv[i];
	}
	ctx->h[0] ^= 0x01010000 ^ (keylen << 8) ^ outlen;

	ctx->t[0] = 0;                      // input count low word
	ctx->t[1] = 0;                      // input count high word
	ctx->c = 0;                         // pointer within buffer
	ctx->outlen = outlen;

	for (i = keylen; i < 128; i++)    // zero input block
	{
		ctx->b[i] = 0;
	}
	if (keylen > 0)
	{
		blake2b_update(ctx, key, keylen);
		ctx->c = 128;                   // at the end
	}

	return 0;
}

// Add "inlen" bytes from "in" into the hash.

void blake2b_update(blake2b_ctx* ctx,
	const void* in, size_t inlen)      // data bytes
{
	size_t i;

	for (i = 0; i < inlen; i++)
	{
		if (ctx->c == 128)            // buffer full ?
		{
			ctx->t[0] += ctx->c;        // add counters
			if (ctx->t[0] < ctx->c)   // carry overflow ?
			{
				ctx->t[1]++;    // high word
			}
			blake2b_compress(ctx, 0); // compress (not last)
			ctx->c = 0;                 // counter to zero
		}
		ctx->b[ctx->c++] = ((const uint8_t*)in)[i];
	}
}

// Generate the message digest (size given in init).
//      Result placed in "out".

void blake2b_final(blake2b_ctx* ctx, void* out)
{
	size_t i;

	ctx->t[0] += ctx->c;                // mark last block offset
	if (ctx->t[0] < ctx->c)           // carry overflow
	{
		ctx->t[1]++;    // high word
	}

	while (ctx->c < 128)              // fill up with zeros
	{
		ctx->b[ctx->c++] = 0;
	}
	blake2b_compress(ctx, 1);        // final block flag = 1

									 // little endian convert and store
	for (i = 0; i < ctx->outlen; i++)
	{
		((uint8_t*)out)[i] =
			(ctx->h[i >> 3] >> (8 * (i & 7))) & 0xFF;
	}
}




void bcx_gen_hash(char * data, int len, char* hash) {
	blake2b_ctx ctx;
	blake2b_init(&ctx,32,NULL,0);
	blake2b_update(&ctx,data,len);
	blake2b_final(&ctx, hash);
}

static const int ascii[16] = {
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,//0-9 ascii
	0x61,0x62,0x63,0x64,0x65,0x66 //a-f ascii
};

int a2i(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	else {
		return -1;
	}
}
char* bin_convert(char *p, size_t len) {
	int i;
	int j;
	char * s = calloc(len, 1);
	for (i = 0; i<len; i++) {
		j = a2i(p[i]);
		s[i] = ascii[j];
	}
	return s;
}

char* bit2String(char* p, size_t len) {
	char * s = calloc(len * 8+1, 1);
	int i;
	for (i = 0; i<len; i++) {
		s[i*8 + 0] = (p[i] & 0x80) >> 7 ? '1' : '0';
		s[i * 8 + 1] = (p[i] & 0x40) >> 6 ? '1' : '0';
		s[i * 8 + 2] = (p[i] & 0x20) >> 5 ? '1' : '0';
		s[i * 8 + 3] = (p[i] & 0x10) >> 4 ? '1' : '0';
		s[i * 8 + 4] = (p[i] & 0x8) >> 3 ? '1' : '0';
		s[i * 8 + 5] = (p[i] & 0x4) >> 2 ? '1' : '0';
		s[i * 8 + 6] = (p[i] & 0x2) >> 1 ? '1' : '0';
		s[i * 8 + 7] = (p[i] & 0x1) >> 0 ? '1' : '0';
	}
	s[8 * len] = '\0';
	return s;
}
void bismuth_gen_hash(const unsigned char* data, unsigned int len, unsigned char* hash)
{
	sph_sha224_context ctx_sha2;

	sph_sha224_init(&ctx_sha2);
	sph_sha224(&ctx_sha2, data, len);
	sph_sha224_close(&ctx_sha2, hash);
}

#define ROL64(x, n)        (((x) << (n)) | ((x) >> (64 - (n))))
void Round1024_host(uint64_t *p0, uint64_t *p1, uint64_t *p2, uint64_t *p3, uint64_t *p4, uint64_t *p5, uint64_t *p6, uint64_t *p7,
	uint64_t *p8, uint64_t *p9, uint64_t *pA, uint64_t *pB, uint64_t *pC, uint64_t *pD, uint64_t *pE, uint64_t *pF, int ROT)
{

	static const int cpu_ROT1024[8][8] =
	{
		{ 55, 43, 37, 40, 16, 22, 38, 12 },
		{ 25, 25, 46, 13, 14, 13, 52, 57 },
		{ 33, 8, 18, 57, 21, 12, 32, 54 },
		{ 34, 43, 25, 60, 44, 9, 59, 34 },
		{ 28, 7, 47, 48, 51, 9, 35, 41 },
		{ 17, 6, 18, 25, 43, 42, 40, 15 },
		{ 58, 7, 32, 45, 19, 18, 2, 56 },
		{ 47, 49, 27, 58, 37, 48, 53, 56 }
	};

	*p0 += *p1;
	*p1 = ROL64(*p1, cpu_ROT1024[ROT][0]);
	*p1 ^= *p0;
	*p2 += *p3;
	*p3 = ROL64(*p3, cpu_ROT1024[ROT][1]);
	*p3 ^= *p2;
	*p4 += *p5;
	*p5 = ROL64(*p5, cpu_ROT1024[ROT][2]);
	*p5 ^= *p4;
	*p6 += *p7;
	*p7 = ROL64(*p7, cpu_ROT1024[ROT][3]);
	*p7 ^= *p6;
	*p8 += *p9;
	*p9 = ROL64(*p9, cpu_ROT1024[ROT][4]);
	*p9 ^= *p8;
	*pA += *pB;
	*pB = ROL64(*pB, cpu_ROT1024[ROT][5]);
	*pB ^= *pA;
	*pC += *pD;
	*pD = ROL64(*pD, cpu_ROT1024[ROT][6]);
	*pD ^= *pC;
	*pE += *pF;
	*pF = ROL64(*pF, cpu_ROT1024[ROT][7]);
	*pF ^= *pE;
}

void SkeinFirstRound(unsigned int *pData, unsigned long long* skeinC)
{
	/// first round of skein performed on cpu ==> constant on gpu

	static const uint64_t cpu_SKEIN1024_IV_1024[16] =
	{
		//     lo           hi
		0x5A4352BE62092156,
		0x5F6E8B1A72F001CA,
		0xFFCBFE9CA1A2CE26,
		0x6C23C39667038BCA,
		0x583A8BFCCE34EB6C,
		0x3FDBFB11D4A46A3E,
		0x3304ACFCA8300998,
		0xB2F6675FA17F0FD2,
		0x9D2599730EF7AB6B,
		0x0914A20D3DFEA9E4,
		0xCC1A9CAFA494DBD3,
		0x9828030DA0A6388C,
		0x0D339D5DAADEE3DC,
		0xFC46DE35C4E2A086,
		0x53D6E4F52E19A6D1,
		0x5663952F715D1DDD,
	};

	uint64_t t[3];
	uint64_t h[17];
	uint64_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;

	uint64_t cpu_skein_ks_parity = 0x5555555555555555;
	h[16] = cpu_skein_ks_parity;
	for (int i = 0; i<16; i++) {
		h[i] = cpu_SKEIN1024_IV_1024[i];
		h[16] ^= h[i];
	}
	uint64_t* alt_data = (uint64_t*)pData;
	/////////////////////// round 1 //////////////////////////// should be on cpu => constant on gpu
	p0 = alt_data[0];
	p1 = alt_data[1];
	p2 = alt_data[2];
	p3 = alt_data[3];
	p4 = alt_data[4];
	p5 = alt_data[5];
	p6 = alt_data[6];
	p7 = alt_data[7];
	p8 = alt_data[8];
	p9 = alt_data[9];
	p10 = alt_data[10];
	p11 = alt_data[11];
	p12 = alt_data[12];
	p13 = alt_data[13];
	p14 = alt_data[14];
	p15 = alt_data[15];
	t[0] = 0x80; // ptr  
	t[1] = 0x7000000000000000; // etype
	t[2] = 0x7000000000000080;

	p0 += h[0];
	p1 += h[1];
	p2 += h[2];
	p3 += h[3];
	p4 += h[4];
	p5 += h[5];
	p6 += h[6];
	p7 += h[7];
	p8 += h[8];
	p9 += h[9];
	p10 += h[10];
	p11 += h[11];
	p12 += h[12];
	p13 += h[13] + t[0];
	p14 += h[14] + t[1];
	p15 += h[15];

	for (int i = 1; i < 21; i += 2)
	{

		Round1024_host(&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, 0);
		Round1024_host(&p0, &p9, &p2, &p13, &p6, &p11, &p4, &p15, &p10, &p7, &p12, &p3, &p14, &p5, &p8, &p1, 1);
		Round1024_host(&p0, &p7, &p2, &p5, &p4, &p3, &p6, &p1, &p12, &p15, &p14, &p13, &p8, &p11, &p10, &p9, 2);
		Round1024_host(&p0, &p15, &p2, &p11, &p6, &p13, &p4, &p9, &p14, &p1, &p8, &p5, &p10, &p3, &p12, &p7, 3);

		p0 += h[(i + 0) % 17];
		p1 += h[(i + 1) % 17];
		p2 += h[(i + 2) % 17];
		p3 += h[(i + 3) % 17];
		p4 += h[(i + 4) % 17];
		p5 += h[(i + 5) % 17];
		p6 += h[(i + 6) % 17];
		p7 += h[(i + 7) % 17];
		p8 += h[(i + 8) % 17];
		p9 += h[(i + 9) % 17];
		p10 += h[(i + 10) % 17];
		p11 += h[(i + 11) % 17];
		p12 += h[(i + 12) % 17];
		p13 += h[(i + 13) % 17] + t[(i + 0) % 3];
		p14 += h[(i + 14) % 17] + t[(i + 1) % 3];
		p15 += h[(i + 15) % 17] + (uint64_t)i;

		Round1024_host(&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, 4);
		Round1024_host(&p0, &p9, &p2, &p13, &p6, &p11, &p4, &p15, &p10, &p7, &p12, &p3, &p14, &p5, &p8, &p1, 5);
		Round1024_host(&p0, &p7, &p2, &p5, &p4, &p3, &p6, &p1, &p12, &p15, &p14, &p13, &p8, &p11, &p10, &p9, 6);
		Round1024_host(&p0, &p15, &p2, &p11, &p6, &p13, &p4, &p9, &p14, &p1, &p8, &p5, &p10, &p3, &p12, &p7, 7);

		p0 += h[(i + 1) % 17];
		p1 += h[(i + 2) % 17];
		p2 += h[(i + 3) % 17];
		p3 += h[(i + 4) % 17];
		p4 += h[(i + 5) % 17];
		p5 += h[(i + 6) % 17];
		p6 += h[(i + 7) % 17];
		p7 += h[(i + 8) % 17];
		p8 += h[(i + 9) % 17];
		p9 += h[(i + 10) % 17];
		p10 += h[(i + 11) % 17];
		p11 += h[(i + 12) % 17];
		p12 += h[(i + 13) % 17];
		p13 += h[(i + 14) % 17] + t[(i + 1) % 3];
		p14 += h[(i + 15) % 17] + t[(i + 2) % 3];
		p15 += h[(i + 16) % 17] + (uint64_t)(i + 1);


	}

	h[0] = p0 ^ alt_data[0];
	h[1] = p1 ^ alt_data[1];
	h[2] = p2 ^ alt_data[2];
	h[3] = p3 ^ alt_data[3];
	h[4] = p4 ^ alt_data[4];
	h[5] = p5 ^ alt_data[5];
	h[6] = p6 ^ alt_data[6];
	h[7] = p7 ^ alt_data[7];
	h[8] = p8 ^ alt_data[8];
	h[9] = p9 ^ alt_data[9];
	h[10] = p10 ^ alt_data[10];
	h[11] = p11 ^ alt_data[11];
	h[12] = p12 ^ alt_data[12];
	h[13] = p13 ^ alt_data[13];
	h[14] = p14 ^ alt_data[14];
	h[15] = p15 ^ alt_data[15];
	h[16] = cpu_skein_ks_parity;
	for (int i = 0; i<16; i++) { h[16] ^= h[i]; }


	memcpy(skeinC, h, sizeof(unsigned long long) * 17);
}

void SK1024(char* input, int size, char* hash)
{
	static unsigned char pblank[1];

	char skein[128];
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 1024);
	Skein1024_Update(&ctx, input, size);
	Skein1024_Final(&ctx, (unsigned char *)skein);
	printf("skein:");
	print_bin(skein, 128);
	printf("\n");
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize(&ctx_keccak, 576, 1024, 1024, 0x05);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)skein, 1024);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)hash);
}

extern void skunkhash(void *state, const void *input, char* midstate);

void haraka_hash(char* output,char* input,int len) {
	char buf[64] = { 0 };
	char hash[32];
	int i = 0;
	for (i = 0; len - i >= 32;i+=32) {
		memcpy(buf + 32, input+i, 32);
		haraka512_port_zero(hash,buf);
		memcpy(buf, hash, 32);
	}
	if (len - i>0) {
		memset(buf+32,0,32);
		memcpy(buf + 32, input + i, len - i);
		haraka512_port_zero(hash, buf);
	}
	memcpy(output,hash,32);
}

char key[8832];
uint8_t before_buf[64] = { 0 };
uint64_t golden_nonce = 0x8239e675;//0x023387;//0x03e693;//0x016ac4;
uint64_t intermediate = 0;

void verus_key_generate(char* input,int len) {
	char buf[64] = { 0 };
	char hash[32];
	int i = 0;
	for (i = 0; len - i >= 32; i += 32) {
		memcpy(buf + 32, input + i, 32);
		haraka512_port(hash, buf);
		memcpy(buf, hash, 32);
	}

	if (len - i > 0) {
		memset(buf + 32, 0, 32);
		memcpy(buf + 32, input + i, len - i);
		//haraka512_port(hash, buf);
	}
	printf("temp buf:");
	print_bin(buf,64);
	memcpy(before_buf,buf, 64);

	int n256blks = 8832 >> 5;
	int nbytesExtra = 8832 & 0x1f;
	unsigned char* pkey = key;
	unsigned char* psrc = buf;

	for (i = 0; i < n256blks; i++) {
		haraka256_port(pkey,psrc);
		psrc = pkey;
		pkey += 32;
	}

	if (nbytesExtra) {
		unsigned char buf2[32];
		haraka256_port(buf2,psrc);
		memcpy(pkey, buf2, nbytesExtra);
	}

	//key[8832] now have the key
}

typedef unsigned char u_char;

extern uint64_t verusclhash_port(void * random, const unsigned char buf[64], uint64_t keyMask);
extern void _mm_store_si128_emu(void* p, __m128i val);
extern __m128i _mm_load_si128_emu(const void* p);
extern u128 _mm_setr_epi8_emu(u_char c0, u_char c1, u_char c2, u_char c3, u_char c4, u_char c5, u_char c6, u_char c7, u_char c8, u_char c9, u_char c10, u_char c11, u_char c12, u_char c13, u_char c14, u_char c15);
void verushash_v2b2(std::string const bytes, void * ptrResult) {
	uint8_t hash[128] = { 0 };
	//uint8_t target[32] = { 0 };
	uint8_t buf[1500] = { 0 };
	uint8_t rev_buf[320] = { 0 };
	uint8_t data[112] = { 0 };
	uint8_t midstate[48] = {0};
	uint8_t temp[48] = { 0 };
	uint8_t tt[112] = { 0 };
	uint8_t endiandata[144] = { 0 };
	//uint32_t nonce = 0x00023387;//0x03e693;//0x010c76;

	load_constants_port();

	// char* hex_data="0400010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011dd9b64000000000000000000000000000000000000000000000000000000000000000000000000fd40050700000000010400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001af5b8015c64d39ab44c60ead8317f9f5a9b6c4c6d29128df5f7758ce5394e6423d0fecab5fae48cbe53e0e216d968e8188a1a1b0100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	char* hex_data= bytes.data();
    hex2bin(buf, hex_data, 1487);

	verus_key_generate(buf,1487);
	// print_bin(key,8832);

	//input noncespace
	hex2bin(before_buf + 32, "75c5254b0000000000000014839d02", 15);
	__m128i shuf1 = _mm_setr_epi8(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0);
	__m128i fill1 = _mm_shuffle_epi8(_mm_load_si128_emu(before_buf), shuf1);
	_mm_store_si128_emu(before_buf + 32 + 16, fill1);
	*(before_buf + 32 + 15) = before_buf[0];
	//*(uint64_t*)(before_buf + 32) = golden_nonce;
	//memcpy(before_buf+32+15, before_buf,16);
	//memcpy(before_buf + 32 + 15+16, before_buf, 1);
	// printf("before buf:");
	// print_bin(before_buf,64);

	intermediate = verusclhash_port(key,before_buf,8191);
	// print_bin((char*)&intermediate, 8);

	__m128i shuf2 = _mm_setr_epi8_emu(1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0);
	__m128i fill2 = _mm_shuffle_epi8(_mm_loadl_epi64((u128*)&intermediate), shuf2);
	_mm_store_si128_emu(before_buf+32+16,fill2);
	*(before_buf + 32 + 15) = *((unsigned char* )&intermediate);
	//memcpy(before_buf + 32 + 15, &intermediate, 8);
	//memcpy(before_buf + 32 + 15 + 8, &intermediate, 8);
	//memcpy(before_buf + 32 + 15 + 8 + 8, &intermediate, 1);
	// printf("after buf:");
	// print_bin(before_buf, 64);

	uint64_t mask = intermediate & (8191 >> 4);
	haraka512_port_keyed(hash,before_buf,((u128 *)key)+mask);

	// printf("hash:");
	//print_bin(hash, 32);

    memcpy(ptrResult, &hash, 32);

}