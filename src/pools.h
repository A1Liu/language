#pragma once
#include <vector>

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

  template <class Type> Type *add() noexcept {
    return (Type *)add(sizeof(Type));
  }

  char *add(uint64_t size) noexcept;
  char *add_extend(uint64_t size) noexcept;
  void clear() noexcept;
  void free() noexcept;
};

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
