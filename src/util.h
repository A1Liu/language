#pragma once
#include <cstdint>
#include <cstring>
#include <ostream>
#include <stdint.h>
#include <vector>

struct String {
  const char *begin, *end;

  String() = default;
  String(const char *s);
  String(const char *_begin, const char *_end) : begin(_begin), end(_end) {}
  String(const char *_begin, uint64_t _len)
      : begin(_begin), end(_begin + _len) {}

  uint64_t size();
  const char &at(uint64_t idx);
  String substr(uint64_t start, uint64_t char_count);
};

std::ostream &operator<<(std::ostream &os, const String &);
bool operator==(const String &, const String &);
bool operator==(const char *, const String &);
bool operator==(const String &, const char *);

// Got idea from Jonathan Blow's video on macros and iteration.
// This is a pass-by-reference/pointer datastructure.
struct BucketArray {
  struct Bucket {
    char *begin;
    char *progress;
  };

  std::vector<Bucket> buckets;

  BucketArray() noexcept = default;
  BucketArray &operator=(const BucketArray &) = delete;
  BucketArray(BucketArray &other) = delete;
  BucketArray(BucketArray &&other) = delete;
  ~BucketArray() = default;

  void free();

  template <class Type> Type *add() noexcept {
    return (Type *)add(sizeof(Type));
  }

  char *add(uint64_t size) noexcept;
};

// This is a pass-by-reference/pointer datastructure.
struct Pool {
  char *begin;
  char *progress;
  char *end;

  Pool(BucketArray *buckets, uint64_t size) noexcept;
  Pool &operator=(const Pool &) = delete;
  Pool(Pool &other) = delete;
  Pool(Pool &&other) = delete;
  ~Pool() = default;

  char *add(uint64_t size) noexcept;
  char *add_extend(BucketArray *buckets, uint64_t size) noexcept;
  BucketArray::Bucket push_to_buckets(BucketArray *buckets) noexcept;
  void clear() noexcept;
  void free() noexcept;

  template <class Type> Type *add() noexcept {
    return (Type *)add(sizeof(Type));
  }
  template <class Type> Type *add_extend(BucketArray *backing_pool) noexcept {
    return (Type *)add_extend(backing_pool, sizeof(Type));
  }
};

struct Hash {
  uint32_t capa;
  uint32_t size;
  uint32_t *data;
  BucketArray *buckets;

  Hash(BucketArray *buckets, uint32_t size, uint32_t data_type);

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
