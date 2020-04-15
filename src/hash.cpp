#include "hash.h"
#include <assert.h>
#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define NULL_KEY 0
#define TOMBSTONE_KEY INT_MAX
#define transform_key(key) (key + 1)
#define invert_transform_key(key) (key - 1)

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

static inline void hashGrow(Hash *h, uint32_t data_type) {
  uint32_t capa2 = h->capa * 2;
  uint32_t *data = h->data;
  Hash h2(capa2, data_type);
  for (int i = 0; i < capa2; i += 2) {
    if (data[i] != NULL_KEY && data[i] != TOMBSTONE_KEY) {
      if (data_type <= sizeof(uint32_t)) {
        *((uint32_t *)h2.insert(invert_transform_key(data[i]), data_type)) =
            data[i + 1];
      } else {
        uint32_t valueIdx = data[i + 1];
        memcpy(h2.insert(invert_transform_key(data[i]), data_type),
               ((char *)(&data[capa2])) + valueIdx * data_type, data_type);
      }
    }
  }
  *h = h2;
}

Hash::Hash(uint32_t size, uint32_t data_type) {
  if (size < 8) {
    size = 8;
  }

  if (data_type < sizeof(uint32_t)) {
    data_type = 0;
  }

  this->capa = pow2roundup(size);
  this->size = 0;
  this->data = (uint32_t *)malloc(this->capa * sizeof(Item) +
                                  data_type * (this->capa >> 1));
  uint32_t capa2 = this->capa * 2;
  for (uint32_t i = 0; i < capa2; i += 2) {
    this->data[i] = 0;
  }
}

void Hash::free() { ::free(this->data); }
void *Hash::find(uint32_t key, uint32_t data_type) {
  return inlineFind(key, data_type);
}
void *Hash::insert(uint32_t key, uint32_t data_type) {
  return inlineInsert(key, data_type);
}
void *Hash::remove(uint32_t key, uint32_t data_type) {
  return inlineRemove(key, data_type);
}

inline void *Hash::inlineFind(uint32_t key, uint32_t data_type) {
  if (data_type <= sizeof(uint32_t)) {
    data_type = 0;
  }

  key = transform_key(key);
  uint32_t *data = this->data;
  uint32_t idx = 2 * (hashword(key) & (this->capa - 1));
  uint32_t capa2 = this->capa * 2;

  for (; data[idx] != NULL_KEY && data[idx] != key;
       idx += 2, idx = idx >= capa2 ? 0 : idx)
    ;

  if (data[idx] == NULL_KEY) {
    return nullptr;
  }

  uint32_t *valueIdx = &data[idx + 1];
  if (data_type == 0) {
    return valueIdx;
  } else {
    return ((char *)(&data[capa2])) + *valueIdx * data_type;
  }
}

inline void *Hash::inlineInsert(uint32_t key, uint32_t data_type) {
  if (data_type <= sizeof(uint32_t)) {
    data_type = 0;
  }

  if (this->size * 2 >= this->capa) {
    hashGrow(this, data_type);
  }

  key = transform_key(key);
  uint32_t *data = this->data;
  uint32_t idx = 2 * (hashword(key) & (this->capa - 1));
  uint32_t capa2 = this->capa * 2;

  for (;
       data[idx] != NULL_KEY && data[idx] != key && data[idx] != TOMBSTONE_KEY;
       idx += 2, idx = idx >= capa2 ? 0 : idx)
    ;

  data[idx] = key;
  uint32_t *valueIdx = &data[idx + 1];
  if (data_type == 0) {
    this->size++;
    return valueIdx;
  } else {
    *valueIdx = this->size++;
    return ((char *)(&data[capa2])) + *valueIdx * data_type;
  }
}

inline void *Hash::inlineRemove(uint32_t key, uint32_t data_type) {
  uint32_t *value = ((uint32_t *)inlineFind(key, 0));
  if (value == nullptr) {
    return nullptr;
  }

  uint32_t *key_ptr = value - 1;
  assert(*key_ptr == transform_key(key));
  *key_ptr = TOMBSTONE_KEY;
  if (data_type <= sizeof(uint32_t)) {
    return value;
  } else {
    return ((char *)(&data[this->capa * 2])) + *value * data_type;
  }
}
