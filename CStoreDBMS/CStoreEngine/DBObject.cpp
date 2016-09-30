#include "DBObject.h"

// 将实例转化为其字符串表示
std::string DBObject::ToString() {
  return this->typeName;
}

// 对比两个对象是否相等，如果子类不重载这个函数它相当于ReferenceEquals
bool DBObject::Equals(const DBObject& RefObj) {
  return this->ReferenceEquals(RefObj);
}

// 对比两个对象是否指向同一内存地址
bool DBObject::ReferenceEquals(const DBObject& RefObj) {
  return this->ReferenceEquals(&RefObj);
}

// 对比两个对象是否指向同一内存地址
bool DBObject::ReferenceEquals(const DBObject* RefObj) {
  return this->allocPointer == RefObj->allocPointer;
}

// 对比两个对象是否是同种类型
bool DBObject::EqualsType(const DBObject& RefObj) {
  return typeid(this) == typeid(RefObj);
}

// 请求这个对象类型名
std::string DBObject::GetTypename() {
  return this->typeName;
}

// 请求这个对象
void* DBObject::GetObject() {
  return this->allocPointer;
}

// 构造器
DBObject::DBObject(std::string TyName, void* ObjPtr)
  :typeName(TyName), allocPointer(ObjPtr) { };

// 析构器
DBObject::~DBObject() {
  this->allocPointer = NULL;
}