#include "type_checking.h"

Type NoneType::unifiedWith(Type t) { return t; }
Type StrType::unifiedWith(Type t) {
  if (t.data.index() == 1) { // It's a string
    return t;
  } else {
    return ObjectType();
  }
}

Type IntType::unifiedWith(Type t) {
  if (t.data.index() == 2 || t.data.index() == 5) {
    return *this;
  } else if (t.data.index() == 3) {
    return t;
  } else {
    return ObjectType();
  }
}

Type FloatType::unifiedWith(Type t) {
  if (t.data.index() == 2 || t.data.index() == 3) {
    return *this;
  } else {
    return ObjectType();
  }
}

Type ObjectType::unifiedWith(Type t) { return *this; }

Type BoolType::unifiedWith(Type t) {
  if (t.data.index() == 2 || t.data.index() == 3) {
    return t;
  } else {
    return ObjectType();
  }
}

Type TypeType::unifiedWith(Type t) {
  if (t.data.index() == 6) { // It's a type
    return t;
  } else {
    return ObjectType();
  }
}

Type FunctionType::unifiedWith(Type t) {
  if (t.data.index() == 7) { // It's a function

    if (*std::get<FunctionType>(t.data).return_type == *return_type) {
      return t;
    }

    return FunctionType(nullptr);
  }

  return ObjectType();
}

bool operator==(const Type &a, const Type &b) {
  if (a.data.index() != b.data.index()) {
    return false;
  }

  if (a.data.index() == 7) { // We're looking at functions
    Type *a_return_type = std::get<FunctionType>(a.data).return_type;
    Type *b_return_type = std::get<FunctionType>(b.data).return_type;
    if (a_return_type == b_return_type) {
      return true;
    }
    if (a_return_type == nullptr && b_return_type != nullptr) {
      return ObjectType() == *b_return_type;
    }

    if (a_return_type != nullptr && b_return_type == nullptr) {
      return *a_return_type == ObjectType();
    }

    return *a_return_type == *b_return_type;
  }

  return true;
}
