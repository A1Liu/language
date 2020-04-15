#pragma once

#include <stdint.h>

struct Hash {
  uint32_t capa;
  uint32_t size;
  uint32_t *data;

  Hash(uint32_t size, uint32_t data_type);

  inline void free();
  inline void *inlineFind(uint32_t key, uint32_t data_type);
  inline void *inlineInsert(uint32_t key, uint32_t data_type);
  inline void *inlineRemove(uint32_t key, uint32_t data_type);
  void *find(uint32_t key, uint32_t data_type);
  void *insert(uint32_t key, uint32_t data_type);
  void *remove(uint32_t key, uint32_t data_type);

  template <class C> C *find(uint32_t key) {
    return (C *)inlineFind(key, sizeof(C));
  }
  template <class C> C *insert(uint32_t key) {
    return (C *)inlineInsert(key, sizeof(C));
  }
  template <class C> C *remove(uint32_t key) {
    return (C *)inlineRemove(key, sizeof(C));
  }
};
