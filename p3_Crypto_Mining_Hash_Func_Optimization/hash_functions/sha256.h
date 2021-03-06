/*********************************************************************
Homework 5
CS 110: Computer Architecture, Spring 2021
ShanghaiTech University

* Last Modified: 03/28/2021
* Copyright Notice: This file is dervived from work of Brad Conte at
  https://github.com/B-Con/crypto-algorithms/
*********************************************************************/

/*********************************************************************
 * Filename:   sha256.h
 * Author:     Brad Conte (brad AT bradconte.com)
 * Copyright:
 * Disclaimer: This code is presented "as is" without any guarantees.
 * Details:    Defines the API for the corresponding SHA1 implementation.
 *********************************************************************/

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32 /* SHA256 outputs a 32 byte digest */

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE; /* 8-bit byte */
typedef unsigned int
    WORD; /* 32-bit word, change to "long" for 16-bit machines */

typedef struct {
  BYTE data[512];         /* Block data, 8 SIMD lines * 64 bytes*/
  WORD datalen;           /* Current data length in the buffer */
  unsigned long bitlen;   /* Bit length of all data of the data */
  WORD state[8];          /* State registers */
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);

#endif /* SHA256_H */
