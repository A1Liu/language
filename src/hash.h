#pragma once

#include <stdint.h>

struct Hash {
  uint32_t capa;
  uint32_t size;
  uint32_t *data;
  uint32_t data_type;
};

Hash hashCreate(uint32_t size, uint32_t data_type);
void hashFree(Hash *h);
void *hashFind(Hash *h, uint32_t key);
void *hashInsert(Hash *h, uint32_t key);
void hashDelete(Hash *h, uint32_t key);
