#ifndef ___CSTORE_DBOBJECT
#define ___CSTORE_DBOBJECT
#include "DBMarcos.h"
#include <string>
#include <typeinfo>

CSTORE_NS_BEGIN
class DBObject {
public:
  //函数作用： 将实例转化为其字符串表示
  //参数列表： N/A
  //返 回 值： 由子类定义的实例字符串表达方式
  virtual std::string ToString();

  //函数作用： 对比两个对象是否相等，如果子类不重写这个函数它相当于ReferenceEquals
  //参数列表：
  //    RefObj 要比较的对象引用
  //返 回 值： 是否相等
  virtual bool Equals(DBObject&);

  //函数作用： 对比两个对象是否指向同一内存地址
  //参数列表：
  //    RefObj 要比较的对象引用
  //返 回 值： 是否为同一对象
  bool ReferenceEquals(const DBObject&);

  //函数作用： 对比两个对象是否指向同一内存地址
  //参数列表：
  //    RefObj 要比较的对象指针
  //返 回 值： 是否为同一对象
  bool ReferenceEquals(const DBObject*);

  //函数作用： 对比两个对象是否是同种类型
  //参数列表：
  //    RefObj 要比较的对象引用
  //返 回 值： 是否同种类型
  bool EqualsType(const DBObject&);

  //函数作用： 请求这个对象的类型名字
  //参数列表： N/A
  //返 回 值： 类型名字符串
  std::string GetTypename();

  //函数作用： 请求这个对象
  //参数列表： N/A
  //返 回 值： 对象的指针
  void* GetObject();

  //函数作用： 构造器
  //参数列表：
  //    TyName 类型的名字
  //    ObjPtr 对象的指针
  //返 回 值： N/A
  DBObject(std::string, void*);

  //函数作用： 析构器
  //参数列表： N/A
  //返 回 值： N/A
  ~DBObject();

protected:
  //函数作用： 为当前实例注册一个Object信息
  //参数列表：
  //    TyName 类型的名字
  //    ObjPtr 对象的指针
  //返 回 值： N/A
  void ObjectRegister(std::string, void*);

private:
  // 对象类型名字
  std::string typeName;

  // 对象在内存的指针
  void* allocPointer;
}; /* DBObject */
CSTORE_NS_END

#endif /* ___CSTORE_DBOBJECT */