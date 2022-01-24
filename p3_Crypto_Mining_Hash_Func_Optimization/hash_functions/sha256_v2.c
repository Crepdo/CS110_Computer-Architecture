/*********************************************************************
Homework 5
CS 110: Computer Architecture, Spring 2021
ShanghaiTech University

* Last Modified: 03/28/2021
* Copyright Notice: This file is dervived from work of Brad Conte at
  https://github.com/B-Con/crypto-algorithms/
*********************************************************************/

/*********************************************************************
* Filename:   sha256.c
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Implementation of the SHA-256 hashing algorithm.
              SHA-256 is one of the three algorithms in the SHA2
              specification. The others, SHA-384 and SHA-512, are not
              offered in this implementation.
              Algorithm specification can be found here:
               * http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
              This implementation uses little endian byte order.
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include "sha256.h"
#include <memory.h>
#include <stdlib.h>
#include <emmintrin.h>

/****************************** MACROS ******************************/
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Integer */
#define ROTR(a, b) (((a) >> (b)) | ((a) << (32 - (b))))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

/* SIMD */
static inline __m128i ROTR_SIMD(__m128i x, int n)
{
  return _mm_srli_epi32(x, n) | _mm_slli_epi32(x, 32 - n);
}

static inline __m128i SHR_SIMD(__m128i x, int n)
{
  return _mm_srli_epi32(x, n);
}

#define EP0_SIMD(x) (ROTR_SIMD(x, 2) ^ ROTR_SIMD(x, 13) ^ ROTR_SIMD(x, 22))
#define EP1_SIMD(x) (ROTR_SIMD(x, 6) ^ ROTR_SIMD(x, 11) ^ ROTR_SIMD(x, 25))
#define SIG0_SIMD(x) (ROTR_SIMD(x, 7) ^ ROTR_SIMD(x, 18) ^ SHR_SIMD(x, 3))
#define SIG1_SIMD(x) (ROTR_SIMD(x, 17) ^ ROTR_SIMD(x, 19) ^ SHR_SIMD(x, 10))
#define ADD4_SIMD(x0, x1, x2, x3) _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(x0, x1), x2), x3)

/**************************** VARIABLES *****************************/
static const WORD k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/*********************** FUNCTION DEFINITIONS ***********************/
void store_epi32(__m128i x, WORD *x0, WORD *x1, WORD *x2, WORD *x3)
{
  union
  {
    WORD data[4];
    __m128i x;
  } container = {.x = x};
  *x0 = container.data[3];
  *x1 = container.data[2];
  *x2 = container.data[1];
  *x3 = container.data[0];
}

void sha256_compress(SHA256_CTX *ctx, WORD *message)
{
  WORD a, b, c, d, e, f, g, h;
  WORD t1, t2, i;

  /* Compress (Integer excution) */
  a = ctx->state[0];
  b = ctx->state[1];
  c = ctx->state[2];
  d = ctx->state[3];
  e = ctx->state[4];
  f = ctx->state[5];
  g = ctx->state[6];
  h = ctx->state[7];

  for (i = 0; i < 64; ++i)
  {
    t1 = h + EP1(e) + CH(e, f, g) + k[i] + message[i];
    t2 = EP0(a) + MAJ(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
}

void sha256_transform_integer(SHA256_CTX *ctx)
{
  WORD a, b, c, d, e, f, g, h;
  WORD i, j, m[64];

  /* Message scheduler */
  for (i = 0, j = 0; i < 16; ++i, j += 4)
    m[i] = (ctx->data[j] << 24) | (ctx->data[j + 1] << 16) | (ctx->data[j + 2] << 8) |
           (ctx->data[j + 3]);
  for (; i < 64; ++i)
    m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

  /* Compress */
  sha256_compress(ctx, m);
}

void sha256_transform_optimized_simd(SHA256_CTX *ctx)
{
  /* Temp values */
  WORD i, j;

  /* Buffer 4 simd lines */
  WORD m[4][64] = {0};

  /* SIMD variables */
  __m128i M[64];

  /* Message scheduler (SIMD) */
  /* Load first 16 words */
  for (i = 0, j = 0; i < 16; ++i, j += 4)
  {
    M[i] = _mm_set_epi32(
        (ctx->data[j] << 24) | (ctx->data[j + 1] << 16) | (ctx->data[j + 2] << 8) | (ctx->data[j + 3]),
        (ctx->data[j + 64] << 24) | (ctx->data[j + 64 + 1] << 16) | (ctx->data[j + 64 + 2] << 8) | (ctx->data[j + 64 + 3]),
        (ctx->data[j + 128] << 24) | (ctx->data[j + 128 + 1] << 16) | (ctx->data[j + 128 + 2] << 8) | (ctx->data[j + 128 + 3]),
        (ctx->data[j + 192] << 24) | (ctx->data[j + 192 + 1] << 16) | (ctx->data[j + 192 + 2] << 8) | (ctx->data[j + 192 + 3]));
  }

  /* Calculate rest 48 words */
  for (; i < 64; ++i)
  {
    M[i] = ADD4_SIMD(SIG1_SIMD(M[i - 2]), M[i - 7], SIG0_SIMD(M[i - 15]), M[i - 16]);
  }

  /* Store scheduled messages into m */
  for (i = 0; i < 64; ++i)
  {
    store_epi32(M[i], &m[0][i], &m[1][i], &m[2][i], &m[3][i]);
  }

  /* Compress */
  for (i = 0; i < 4; i++)
  {
    sha256_compress(ctx, m[i]);
  }
}

void sha256_init(SHA256_CTX *ctx)
{
  ctx->datalen = 0;
  ctx->bitlen = 0;
  ctx->state[0] = 0x6a09e667;
  ctx->state[1] = 0xbb67ae85;
  ctx->state[2] = 0x3c6ef372;
  ctx->state[3] = 0xa54ff53a;
  ctx->state[4] = 0x510e527f;
  ctx->state[5] = 0x9b05688c;
  ctx->state[6] = 0x1f83d9ab;
  ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
  WORD i;

  /* Calculate(4 blocks per unit) with SIMD optimization */
  for (i = 0; i < len - len % 256; ++i)
  {
    ctx->data[ctx->datalen] = data[i];
    ctx->datalen++;
    if (ctx->datalen == 256)
    {
      sha256_transform_optimized_simd(ctx);
      ctx->bitlen += 2048;
      ctx->datalen = 0;
    }
  }

  /* Calculate rest part (1 block per unit) with integer excution */
  for (; i < len; ++i)
  {
    ctx->data[ctx->datalen] = data[i];
    ctx->datalen++;
    if (ctx->datalen == 64)
    {
      sha256_transform_integer(ctx);
      ctx->bitlen += 512;
      ctx->datalen = 0;
    }
  }
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
  WORD i;

  i = ctx->datalen;

  /* Pad whatever data is left in the buffer. */
  if (ctx->datalen < 56)
  {
    ctx->data[i++] = 0x80;
    while (i < 56)
      ctx->data[i++] = 0x00;
  }
  else
  {
    ctx->data[i++] = 0x80;
    while (i < 64)
      ctx->data[i++] = 0x00;
    sha256_transform_integer(ctx);
    memset(ctx->data, 0, 56);
  }

  /* Append to the padding the total message's length in bits and transform. */
  ctx->bitlen += ctx->datalen * 8;
  ctx->data[63] = ctx->bitlen;
  ctx->data[62] = ctx->bitlen >> 8;
  ctx->data[61] = ctx->bitlen >> 16;
  ctx->data[60] = ctx->bitlen >> 24;
  ctx->data[59] = ctx->bitlen >> 32;
  ctx->data[58] = ctx->bitlen >> 40;
  ctx->data[57] = ctx->bitlen >> 48;
  ctx->data[56] = ctx->bitlen >> 56;
  sha256_transform_integer(ctx);

  /* Since this implementation uses little endian byte ordering and SHA uses big
     endian, reverse all the bytes when copying the final state to the output
     hash. */
  for (i = 0; i < 4; ++i)
  {
    hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
    hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
  }
}
