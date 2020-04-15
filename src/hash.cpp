#include "hash.h"
#include <assert.h>
#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define NULL_KEY 0
#define TOMBSTONE_KEY INT_MAX

// We add 1 to each key before adding to the hash table
// growth factor is 1/2 always, capacity is always a power of 2

static uint32_t hashword(uint32_t c) {
  // Bob Jenkin's mix function, possibly overkill for only 32 bits?
  // but a simpler one was no faster, so what the heck
  uint32_t a, b;
  a = b = 0x9e3779b9; /* the golden ratio; an arbitrary value */
  a -= b;
  a -= c;
  a ^= (c >> 13);
  b -= c;
  b -= a;
  b ^= (a << 8);
  c -= a;
  c -= b;
  c ^= (b >> 13);
  a -= b;
  a -= c;
  a ^= (c >> 12);
  b -= c;
  b -= a;
  b ^= (a << 16);
  c -= a;
  c -= b;
  c ^= (b >> 5);
  a -= b;
  a -= c;
  a ^= (c >> 3);
  b -= c;
  b -= a;
  b ^= (a << 10);
  c -= a;
  c -= b;
  c ^= (b >> 15);
  return c;
}

typedef struct {
  uint32_t key;
  uint32_t value;
} Item;

// https://stackoverflow.com/a/365068
static inline uint32_t pow2roundup(uint32_t x) {
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x + 1;
}

static inline void rehash(Hash *h) {}

Hash hashCreate(uint32_t size, uint32_t data_type) {
  Hash h;
  if (size < 8) {
    size = 8;
  }
  h.capa = pow2roundup(size);
  h.size = 0;
  h.data_type = data_type > sizeof(uint32_t) ? data_type : 0;
  h.data =
      (uint32_t *)malloc(h.capa * sizeof(Item) + h.data_type * (h.capa >> 1));
  return h;
}

void hashFree(Hash *h) { free(h->data); }

void *hashFind(Hash *h, uint32_t key) {
  key += 1;
  uint32_t *data = h->data;
  uint32_t idx = 2 * (hashword(key) & (h->capa - 1));
  uint32_t capa2 = h->capa * 2;

  for (; data[idx] != NULL_KEY && data[idx] != key;
       idx += 2, idx = idx >= capa2 ? 0 : idx)
    ;

  if (data[idx] == NULL_KEY) {
    return NULL;
  }

  uint32_t *valueIdx = &data[idx + 1];
  if (h->data_type == 0) {
    return valueIdx;
  } else {
    return ((char *)(&data[capa2])) + *valueIdx * h->data_type;
  }
}

void *hashInsert(Hash *h, uint32_t key) {
  if (h->size * 2 >= h->capa) {
    std::cerr << "rehashing" << std::endl;
    rehash(h);
  }
  key += 1;
  uint32_t *data = h->data;
  uint32_t idx = 2 * (hashword(key) & (h->capa - 1));
  uint32_t capa2 = h->capa * 2;

  for (;
       data[idx] != NULL_KEY && data[idx] != key && data[idx] != TOMBSTONE_KEY;
       idx += 2, idx = idx >= capa2 ? 0 : idx)
    ;

  data[idx] = key;
  uint32_t *valueIdx = &data[idx + 1];
  if (h->data_type == 0) {
    h->size++;
    return valueIdx;
  } else {
    *valueIdx = h->size++;
    return ((char *)(&data[capa2])) + *valueIdx * h->data_type;
  }
}

void hashDelete(Hash *h, uint32_t key) {}
