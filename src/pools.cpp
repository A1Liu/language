#include "pools.h"

#define BUCKET_SIZE 1024
#define DEFAULT_POOL_SIZE 1024
#define POOL_GROWTH_FACTOR 1.2

Pool::Pool() noexcept {
  progress = begin = new char[DEFAULT_POOL_SIZE];
  end = begin + DEFAULT_POOL_SIZE;
}
Pool::Pool(uint64_t size) noexcept {
  if (size < DEFAULT_POOL_SIZE) {
    size = DEFAULT_POOL_SIZE;
  }
  progress = begin = new char[size];
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

void Pool::clear() noexcept {
    progress = begin;
}

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
