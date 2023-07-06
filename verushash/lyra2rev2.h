#ifndef LYRA2REV2_H
#define LYRA2REV2_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "miner.h"
#define LYRA_SCRATCHBUF_SIZE (1536) // matrix size [12][4][4] uint64_t or equivalent
#define LYRA_SECBUF_SIZE (4) // (not used)

void lyra2rev2_regenhash(char* tdata, uint32_t nonce, char *ohash);
void lyra2rev2hash(void *state, const void *input);
void lyra2rev2_regenhash_by_midstate(char* tdata, uint32_t nonce, char *ohash);

#endif /* LYRA2REV2_H */
