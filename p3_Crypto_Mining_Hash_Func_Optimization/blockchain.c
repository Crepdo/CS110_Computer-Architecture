/*********************************************************************
Homework 5
CS 110: Computer Architecture, Spring 2021
ShanghaiTech University

* Last Modified: 03/28/2021
*********************************************************************/

#include "blockchain.h"
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include <stdio.h>

#define MAX_UINT_64 0xffffffffffffffff

void blockchain_node_init(blk_t *node, uint32_t index, uint32_t timestamp,
                          unsigned char prev_hash[32], unsigned char *data,
                          size_t data_size)
{
  if (!node || !data || !prev_hash)
    return;

  node->header.index = index;
  node->header.timestamp = timestamp;
  node->header.nonce = -1;

  memset(node->header.data, 0, sizeof(unsigned char) * 256);
  memcpy(node->header.prev_hash, prev_hash, HASH_BLOCK_SIZE);
  memcpy(node->header.data, data,
         sizeof(unsigned char) * ((data_size < 256) ? data_size : 256));
}

void blockchain_node_hash(blk_t *node, unsigned char hash_buf[HASH_BLOCK_SIZE],
                          hash_func func)
{
  if (node)
    func((unsigned char *)node, sizeof(blkh_t), (unsigned char *)hash_buf);
}

BOOL blockchain_node_verify(blk_t *node, blk_t *prev_node, hash_func func)
{
  unsigned char hash_buf[HASH_BLOCK_SIZE];

  if (!node || !prev_node)
    return False;

  blockchain_node_hash(node, hash_buf, func);
  if (memcmp(node->hash, hash_buf, sizeof(unsigned char) * HASH_BLOCK_SIZE))
    return False;

  blockchain_node_hash(prev_node, hash_buf, func);
  if (memcmp(node->header.prev_hash, hash_buf,
             sizeof(unsigned char) * HASH_BLOCK_SIZE))
    return False;

  return True;
}

/* The sequiental implementation of mining implemented for you. */
void blockchain_node_mine(blk_t *node, unsigned char hash_buf[HASH_BLOCK_SIZE],
                          size_t diff, hash_func func)
{
  unsigned char one_diff[HASH_BLOCK_SIZE];
  size_t diff_q, diff_m;
  diff_q = diff / 8;
  diff_m = diff % 8;
  memset(one_diff, 0xFF, sizeof(unsigned char) * HASH_BLOCK_SIZE);
  memset(one_diff, 0, sizeof(unsigned char) * diff_q);
  one_diff[diff_q] = ((uint8_t)0xFF) >> diff_m;

  BOOL nonce_flag = False;

  unsigned long base = 0;

  /* Set threads according to cpu */
#pragma omp
  {
    if (omp_get_num_threads() == 20)
    {
      omp_set_num_threads(20);
    }
    else
    {
      omp_set_num_threads(4);
    }
  }

  for (base = 0; base < MAX_UINT_64 && !nonce_flag && base != MAX_UINT_64; base += 0x2000)
  {
#pragma omp parallel shared(nonce_flag)
    {

      /* Buffer for hash and node */
      blk_t private_node;
      unsigned char private_hash_buf[HASH_BLOCK_SIZE];

      /* Init private node */
      memcpy(&private_node, node, sizeof(blk_t));
#pragma omp for schedule(dynamic)
      for (unsigned long i = base; i < base + 0x2000; i++)
      {
        if (nonce_flag)
          continue;

        blockchain_node_hash(&private_node, private_hash_buf, func);
        if ((!memcmp(private_hash_buf, one_diff, sizeof(unsigned char) * diff_q)) &&
            memcmp(&private_hash_buf[diff_q], &one_diff[diff_q],
                   sizeof(unsigned char) * (HASH_BLOCK_SIZE - diff_q)) <= 0)
        {
          {
            if (nonce_flag)
              continue;
            /* Set nonce flag */
            nonce_flag = True;
            /* Save back result */
            node->header.nonce = private_node.header.nonce;
            memcpy(node->hash, private_hash_buf, sizeof(unsigned char) * HASH_BLOCK_SIZE);
          }
        }
        private_node.header.nonce = base + i;
      }
    }
  }

  /* Store calculated hash into hash_buf */
  memcpy(hash_buf, node->hash, sizeof(unsigned char) * HASH_BLOCK_SIZE);
}
