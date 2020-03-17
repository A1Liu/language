#pragma once
#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>

struct Type;

struct NoneType {
  Type unifiedWith(Type);
};

struct StrType {
  Type unifiedWith(Type);
};

struct IntType {
  Type unifiedWith(Type);
};

struct FloatType {
  Type unifiedWith(Type);
};

struct ObjectType {
  Type unifiedWith(Type);
};

struct BoolType {
  Type unifiedWith(Type);
};

struct TypeType {
  Type unifiedWith(Type);
};

struct FunctionType {
  Type *return_type; // If null, it is object

  FunctionType(Type *return_type_) : return_type(return_type_) {}
  Type unifiedWith(Type);
};

struct Type {
  std::variant<NoneType, StrType, IntType, FloatType, ObjectType, BoolType,
               TypeType, FunctionType>
      data;

  Type(NoneType t) : data(t) {}
  Type(StrType t) : data(t) {}
  Type(IntType t) : data(t) {}
  Type(FloatType t) : data(t) {}
  Type(ObjectType t) : data(t) {}
  Type(BoolType t) : data(t) {}
  Type(TypeType t) : data(t) {}
  Type(FunctionType t) : data(t) {}

  Type unifiedWith(Type other);
};

bool operator==(const Type &a, const Type &b);

struct TypeContext {
  std::unordered_map<std::string_view, std::optional<Type>> context;
  TypeContext *parent; // Global context doesn't have a parent
};
