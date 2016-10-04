#include "DBAllocator.h"
CSTORE_NS_BEGIN

// 私有的构造函数
DBAllocator::DBAllocator()
  :DBObject("DBAllocator", this) { }

// 析构函数
DBAllocator::~DBAllocator() {
  this->FreeAll();
}

// 工厂方法，获得类的唯一实例
DBAllocator* DBAllocator::GetInstance() {
  return DBAllocator::Instance == NULL ?
    DBAllocator::Instance = new DBAllocator() : DBAllocator::Instance;
}

// 申请分配一个缓冲区
void* DBAllocator::Alloc(std::string _name, size_t _size) {
  // 不可重名
  if (this->allocPool.find(_name) != this->allocPool.end()) {
    TRACE("Expect to alloc buffer" + _name + ", but already exist.");
    return NULL;
  }
  void* memblock = std::malloc(_size);
  std::memset(memblock, 0, sizeof(memblock));
  this->allocPool.insert(std::pair<std::string, void*>(_name, memblock));
  return memblock;
}

// 请求一个缓冲区
void* DBAllocator::Get(std::string _name) {
  if (this->allocPool.find(_name) != this->allocPool.end()) {
    return this->allocPool[_name];
  }
  TRACE("Expect to get buffer" + _name + ", but not exist.");
  return NULL;
}

// 释放一个缓冲区
void DBAllocator::Free(std::string _name) {
  if (this->allocPool.find(_name) != this->allocPool.end()) {
    if (this->allocPool[_name] != NULL) {
      std::free(this->allocPool[_name]);
      this->allocPool[_name] = NULL;
    }
    this->allocPool.erase(_name);
  } else {
    TRACE("Expect to release buffer" + _name + ", but not exist.");
  }
}

// 释放所有分配的缓冲区
void DBAllocator::FreeAll() {
  for (std::map<std::string, void*>::iterator iter = this->allocPool.begin();
    iter != this->allocPool.end(); iter++) {
    if ((*iter).second != NULL) {
      std::free((*iter).second);
      (*iter).second = NULL;
    }
  }
  this->allocPool.clear();
}

// 释放DBMS各模块内存准备退出程序
void DBAllocator::Collapse() {
  this->FreeAll();
}

// 唯一实例
DBAllocator* DBAllocator::Instance = NULL;

CSTORE_NS_END