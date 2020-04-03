#pragma once
#include <vector>

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

  Pool() noexcept;
  explicit Pool(uint64_t size) noexcept;
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
