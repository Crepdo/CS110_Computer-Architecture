/*********************************************************************
Homework 5
CS 110: Computer Architecture, Spring 2021
ShanghaiTech University

* Last Modified: 03/28/2021
* Copyright Notice: This file is dervived from work of Brad Conte at
  https://github.com/B-Con/crypto-algorithms/
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include "sha256.h"
#include <memory.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <stdio.h>

/****************************** MACROS ******************************/
#define ROTR(a, b) (((a) >> (b)) | ((a) << (32 - (b))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* SIMD version of ROTR */
static inline __m128i ROTR_SIMD(__m128i x, int n)
{
  return _mm_srli_epi32(x, n) | _mm_slli_epi32(x, 32 - n);
}

/* SIMD version of SHR */
static inline __m128i SHR_SIMD(__m128i x, int n)
{
  return _mm_srli_epi32(x, n);
}

#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR_SIMD(x, 7) ^ ROTR_SIMD(x, 18) ^ (SHR_SIMD((x), 3)))
#define SIG1(x) (ROTR_SIMD(x, 17) ^ ROTR_SIMD(x, 19) ^ (SHR_SIMD((x), 10)))

/**************************** VARIABLES *****************************/

/* Constants K */
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

WORD get_one_epi32(__m128i x, WORD i)
{
  union
  {
    WORD data[4];
    __m128i x;
  } container = {.x = x};
  return container.data[i];
}

void compress_four_rounds(__m128i *SIMD_buf, WORD index, WORD *h_old, WORD *a, WORD *b, WORD *c, WORD *d, WORD *e, WORD *f, WORD *g, WORD *h)
{
  /* SIMD temp values for compression */
  __m128i T0;
  WORD t1, t2, i;
  WORD hwk[4] = {0};

  /* Calculate state register of 16 words */
  /* w[i] + k[i] + h */
  T0 = *SIMD_buf;
  T0 = _mm_add_epi32(_mm_add_epi32(T0, _mm_set_epi32(k[index + 3], k[index + 2], k[index + 1], k[index])), _mm_set_epi32(h_old[3], h_old[2], h_old[1], h_old[0]));
  store_epi32(T0, &hwk[3], &hwk[2], &hwk[1], &hwk[0]);

  for (i = 0; i < 4; i++)
  {
    t1 = EP1(*e) + CH(*e, *f, *g) + hwk[i];
    t2 = EP0(*a) + MAJ(*a, *b, *c);
    *h = *g;
    *g = *f;
    *f = *e;
    *e = *d + t1;
    *d = *c;
    *c = *b;
    *b = *a;
    *a = t1 + t2;
    h_old[i] = *e;
  }
}

/* Do message schaduling for word i */
void message_schedule_four_rounds(WORD *w, __m128i *X0, __m128i *X1, __m128i *X2, __m128i *X3, WORD i)
{
  /* Temp values */
  __m128i Wt;
  __m128i W;
  __m128i S0, S1;
  __m128i T0;
  WORD temp;

  /* Calculate s0 and mt 4-at-a-time */
  S0 = SIG0(_mm_set_epi32(get_one_epi32(*X1, 0), get_one_epi32(*X0, 3), get_one_epi32(*X0, 2), get_one_epi32(*X0, 1)));
  T0 = _mm_set_epi32(get_one_epi32(*X3, 0), get_one_epi32(*X2, 3), get_one_epi32(*X2, 2), get_one_epi32(*X2, 1));
  Wt = _mm_add_epi32(_mm_add_epi32(*X0, S0), T0);

  /* Calculate s1 2-at-a-time twice*/
  /* Calculate w[i] and w[i+1] */
  S1 = SIG1(_mm_set_epi32(0, 0, get_one_epi32(*X3, 3), get_one_epi32(*X3, 2)));
  W = _mm_add_epi32(S1, Wt);
  store_epi32(W, &temp, &temp, &w[i + 1], &w[i]);

  /* Calculate w[i+3] and w[i+2] */
  S1 = SIG1(_mm_set_epi32(w[i + 1], w[i], 0, 0));
  W = _mm_add_epi32(S1, Wt);
  store_epi32(W, &w[i + 3], &w[i + 2], &temp, &temp);
}

void four_rounds_and_sched(WORD *w, __m128i *X0, __m128i *X1, __m128i *X2, __m128i *X3, WORD index, WORD *h_old, WORD *a, WORD *b, WORD *c, WORD *d, WORD *e, WORD *f, WORD *g, WORD *h)
{
  /* Do compress and schedule in SIMD */
  compress_four_rounds(X0, index - 16, h_old, a, b, c, d, e, f, g, h);
  message_schedule_four_rounds(w, X0, X1, X2, X3, index);
}

void load_SIMD_buf(__m128i **X0, __m128i **X1, __m128i **X2, __m128i **X3, WORD *w, WORD i)
{
  if (i == 16)
  {
    **X3 = _mm_set_epi32(w[i - 1], w[i - 2], w[i - 3], w[i - 4]);
    **X2 = _mm_set_epi32(w[i - 5], w[i - 6], w[i - 7], w[i - 8]);
    **X1 = _mm_set_epi32(w[i - 9], w[i - 10], w[i - 11], w[i - 12]);
    **X0 = _mm_set_epi32(w[i - 13], w[i - 14], w[i - 15], w[i - 16]);
  }
  else
  {
    __m128i *Xt = NULL;
    Xt = *X0;
    *X0 = *X1;
    *X1 = *X2;
    *X2 = *X3;
    *X3 = Xt;
    **X3 = _mm_set_epi32(w[i - 1], w[i - 2], w[i - 3], w[i - 4]);
  }
}

void sha256_transform(SHA256_CTX *ctx, const BYTE data[])
{
  WORD a, b, c, d, e, f, g, h;
  WORD i, j;
  WORD w[64];
  __m128i *X0 = malloc(sizeof(__m128i));
  __m128i *X1 = malloc(sizeof(__m128i));
  __m128i *X2 = malloc(sizeof(__m128i));
  __m128i *X3 = malloc(sizeof(__m128i));
  WORD h_old[4];

  /* Initiate state register */
  a = ctx->state[0];
  b = ctx->state[1];
  c = ctx->state[2];
  d = ctx->state[3];
  e = ctx->state[4];
  f = ctx->state[5];
  g = ctx->state[6];
  h = ctx->state[7];

  /* Init h_old */
  h_old[0] = h;
  h_old[1] = g;
  h_old[2] = f;
  h_old[3] = e;

  /* Split a block into first 16 messages */
  for (i = 0, j = 0; i < 16; ++i, j += 4)
  {
    w[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
  }

  /* Do compressing and scheduling together */
  for (i = 16; i < 64; i += 16)
  {
    /* Basic loop body */
    for (j = i; j < i + 16; j += 4)
    {
      load_SIMD_buf(&X0, &X1, &X2, &X3, w, j);
      four_rounds_and_sched(w, X0, X1, X2, X3, j, h_old, &a, &b, &c, &d, &e, &f, &g, &h);
    }
  }

  /* Do four round for the last 16 rounds */
  load_SIMD_buf(&X0, &X1, &X2, &X3, w, 64);
  compress_four_rounds(X0, i - 16, h_old, &a, &b, &c, &d, &e, &f, &g, &h);
  compress_four_rounds(X1, i - 12, h_old, &a, &b, &c, &d, &e, &f, &g, &h);
  compress_four_rounds(X2, i - 8, h_old, &a, &b, &c, &d, &e, &f, &g, &h);
  compress_four_rounds(X3, i - 4, h_old, &a, &b, &c, &d, &e, &f, &g, &h);

  /* Add state register with calculated value */
  // S = _mm_set_epi32(ctx->state[3], ctx->state[2], ctx->state[1], ctx->state[0]);
  // R = _mm_set_epi32(d, c, b, a);
  // S = _mm_add_epi32(S, R);
  // store_epi32(S, &ctx->state[3], &ctx->state[2], &ctx->state[1], &ctx->state[0]);

  // S = _mm_set_epi32(ctx->state[7], ctx->state[6], ctx->state[5], ctx->state[4]);
  // R = _mm_set_epi32(h, g, f, e);
  // S = _mm_add_epi32(S, R);
  // store_epi32(S, &ctx->state[7], &ctx->state[6], &ctx->state[5], &ctx->state[4]);

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;

  free(X0);
  free(X1);
  free(X2);
  free(X3);
}

/* Init state registers with */
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
  for (i = 0; i < len; ++i)
  {
    /* Fill ctx data with input data */
    ctx->data[ctx->datalen] = data[i];
    ctx->datalen++;

    /* If ctx data is full (64 bytes), do transform */
    if (ctx->datalen == 64)
    {
      sha256_transform(ctx, ctx->data);

      /* Add bitlen by 512, reset datalen */
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
    /* Add an 1 after the data */
    ctx->data[i++] = 0x80;

    /* Padding with 0 */
    while (i < 56)
      ctx->data[i++] = 0x00;
  }
  else
  {
    /* Add an 1 after the data */
    ctx->data[i++] = 0x80;

    /* Fill the rest part of data with 0s */
    while (i < 64)
      ctx->data[i++] = 0x00;

    /* Do sha256 for the new block */
    sha256_transform(ctx, ctx->data);

    /* Set the new block with 0s and length of data */
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
  sha256_transform(ctx, ctx->data);

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
