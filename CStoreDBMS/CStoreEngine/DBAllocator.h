#ifndef ___CSTORE_DBALLOCATOR
#define ___CSTORE_DBALLOCATOR
#include "DBBase.h"
CSTORE_NS_BEGIN

class DBAllocator : public DBObject {
public:
  //函数作用： 工厂方法，获得类的唯一实例
  //参数列表： N/A
  //返 回 值： 内存管理器的唯一实例
  static DBAllocator* GetInstance();

  //函数作用： 析构函数
  //参数列表： N/A
  //返 回 值： N/A
  ~DBAllocator();

  //函数作用： 申请分配一个缓冲区
  //参数列表：
  //     _name 缓冲区名称
  //     _size 缓冲区尺寸
  //返 回 值： 缓冲区指针
  void* Alloc(std::string, size_t);

  //函数作用： 请求一个缓冲区
  //参数列表：
  //     _name 缓冲区名称
  //返 回 值： 缓冲区指针
  void* Get(std::string);

  //函数作用： 释放一个缓冲区
  //参数列表：
  //     _name 缓冲区名称
  //返 回 值： N/A
  void Free(std::string);

  //函数作用： 释放所有分配的缓冲区
  //参数列表： N/A
  //返 回 值： N/A
  void FreeAll();

  //函数作用： 释放DBMS各模块内存准备退出程序
  //参数列表： N/A
  //返 回 值： N/A
  static void Collapse();

private:
  //函数作用： 私有的构造函数
  //参数列表： N/A
  //返 回 值： N/A
  DBAllocator();

  // 已分配句柄字典
  std::map<std::string, void*> allocPool;
  // 唯一实例
  static DBAllocator* Instance;
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBAllocator);
}; /* DBAllocator */

CSTORE_NS_END
#endif /* ___CSTORE_DBALLOCATOR */