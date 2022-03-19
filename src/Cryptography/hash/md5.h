
#ifndef _MD5_H
#define _MD5_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define 	MD5_DIGEST_LENGTH   16

struct MD5Context {
    uint32_t buf[4];
    uint32_t bits[2];
	union {
		unsigned char in[64];
        uint32_t in32[MD5_DIGEST_LENGTH];
	};
};

/*
 * Calculate and store in 'output' the MD5 digest of 'len' bytes at
 * 'input'. 'output' must have enough space to hold 16 bytes.
 */
#ifdef __cplusplus
extern "C" {
void MD5 (const unsigned char *input, int len, unsigned char output[MD5_DIGEST_LENGTH]);
}
#endif

#endif /* _MD5_H */
