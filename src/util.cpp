#include "util.h"
#include <assert.h>
#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define BUCKET_SIZE 1024
#define DEFAULT_POOL_SIZE 1024
#define POOL_GROWTH_FACTOR 1.2

#define NULL_KEY 0
#define TOMBSTONE_KEY INT_MAX
#define transform_key(key) (key + 1)
#define invert_transform_key(key) (key - 1)

String::String(const char *s) : begin(s) {
  uint64_t len = strlen(s);
  end = begin + len;
}

uint64_t String::size() { return end - begin; }

const char &String::at(uint64_t idx) { return *(begin + idx); }

String String::substr(uint64_t start, uint64_t char_count) {
  return String{begin + start, begin + start + char_count};
}

bool operator==(const char *other, const String &view) { return view == other; }
bool operator==(const String &view, const char *other) {
  uint64_t len = strlen(other);
  if (view.end - view.begin != len) {
    return false;
  }
  for (uint64_t i = 0; i != len; i++) {
    if (view.begin[i] != other[i]) {
      return false;
    }
  }
  return true;
}

bool operator==(const String &a, const String &b) {
  if (a.end - a.begin != b.end - b.begin) {
    return false;
  }
  for (uint64_t i = 0; i != a.end - a.begin; i++) {
    if (a.begin[i] != b.begin[i]) {
      return false;
    }
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const String &view) {
  for (const char *i = view.begin; i != view.end; i++) {
    os << *i;
  }
  return os;
}

Pool::Pool(BucketArray *buckets, uint64_t size) noexcept {
  progress = begin = buckets->add(size);
  end = begin + size;
}

char *Pool::add(uint64_t size) noexcept {
  if (size > end - progress) {
    return nullptr;
  }

  char *retLocation = progress;
  progress += size;
  return retLocation;
}

char *Pool::add_extend(BucketArray *buckets, uint64_t size) noexcept {
  if (size > end - progress) {
    uint64_t current_size = end - begin;
    uint64_t new_size = size + current_size * POOL_GROWTH_FACTOR;
    char *new_begin = buckets->add(new_size);
    memcpy(new_begin, begin, end - begin);
    progress = progress - begin + new_begin;
    end = new_begin + new_size;
    begin = new_begin;
  }

  char *retLocation = progress;
  progress += size;
  return retLocation;
}

BucketArray::Bucket Pool::push_to_buckets(BucketArray *buckets) noexcept {
  uint64_t size = progress - begin;
  char *bucket_begin = buckets->add(size);
  char *bucket_end = bucket_begin + size;
  memcpy(bucket_begin, begin, size);
  clear();
  return BucketArray::Bucket{bucket_begin, bucket_end};
}

void Pool::clear() noexcept { progress = begin; }

void Pool::free() noexcept {
  ::free(begin);
  begin = progress = end = nullptr;
}

void BucketArray::free() {
  for (auto bucket_ : buckets) {
    delete bucket_.begin;
  }
  buckets.clear();
}

char *BucketArray::add(uint64_t size) noexcept {
  size = (size - 1) / 16 * 16 + 16;
  if (buckets.size() == 0) {
    char *begin = new char[BUCKET_SIZE];
    buckets.push_back(Bucket{begin, begin});
  }

  if (size > BUCKET_SIZE) {
    Bucket old_back = buckets.back();
    char *begin = new char[size];
    buckets.back() = Bucket{begin, begin + size};
    buckets.push_back(old_back);
    return begin;
  }

  Bucket &last_bucket = buckets.back();
  if (BUCKET_SIZE - (last_bucket.progress - last_bucket.begin) < size) {
    char *begin = new char[BUCKET_SIZE];
    buckets.push_back(Bucket{begin, begin});
    last_bucket = buckets.back();
  }

  char *retLocation = last_bucket.progress;
  last_bucket.progress += size;
  return retLocation;
}

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
  Hash h2(h->buckets, capa2, data_type);
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

Hash::Hash(BucketArray *buckets, uint32_t size, uint32_t data_type) {
  if (size < 8) {
    size = 8;
  }

  if (data_type < sizeof(uint32_t)) {
    data_type = 0;
  }

  this->buckets = buckets;
  this->capa = pow2roundup(size);
  this->size = 0;
  this->data = (uint32_t *)buckets->add(this->capa * sizeof(Item) +
                                        data_type * (this->capa >> 1));
  uint32_t capa2 = this->capa * 2;
  for (uint32_t i = 0; i < capa2; i += 2) {
    this->data[i] = 0;
  }
}

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
