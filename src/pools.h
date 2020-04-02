#include <iostream>
#include <vector>

#define BUCKET_SIZE 1024

// Got idea from Jonathan Blow's video on macros and iteration.
// This is a pass-by-reference datastructure.
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

  void free() {
    for (auto bucket_ : buckets) {
      delete bucket_.begin;
    }
    buckets.clear();
  }

  template <class Type> Type *add() noexcept {
    const auto typeSize = sizeof(Type);
    if (typeSize > BUCKET_SIZE) {
      throw 1;
    }

    if (buckets.size() == 0) {
      char *begin = new char[BUCKET_SIZE];
      buckets.push_back(Bucket{begin, begin});
    }

    Bucket &last_bucket = buckets.back();
    if (BUCKET_SIZE - (last_bucket.progress - last_bucket.begin) >= typeSize) {
      char *begin = new char[BUCKET_SIZE];
      buckets.push_back(Bucket{begin, begin});
      last_bucket = buckets.back();
    }

    Type *retLocation = (Type *)last_bucket.progress;
    last_bucket.progress += typeSize;
    return retLocation;
  }
};

// Pass by value
struct Pool {
  char *begin;
  char *progress;
  char *end;

  explicit Pool(uint64_t size) noexcept {
    progress = begin = new char[size];
    end = begin + size;
  }

  template <class Type> Type *add(Type a) noexcept {
    const auto typeSize = sizeof(Type);
    if (typeSize > BUCKET_SIZE) {
      throw 1;
    }

    if (end - progress >= typeSize) {
      Type *retLocation = (Type *)progress;
      progress += typeSize;
      return retLocation;
    }

    return nullptr;
  }
};
