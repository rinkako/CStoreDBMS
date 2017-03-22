#include "DBAllocator.h"
#include "TableManager.h"
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
  return DBAllocator::Instance == nullptr ?
    DBAllocator::Instance = new DBAllocator() : DBAllocator::Instance;
}

// 申请分配一个缓冲区
void* DBAllocator::Alloc(std::string _name, size_t _size) {
  // 不可重名
  if (this->allocPool.find(_name) != this->allocPool.end()) {
    TRACE("Expect to alloc buffer" + _name + ", but already exist.");
    return nullptr;
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
  return nullptr;
}

// 释放一个缓冲区
void DBAllocator::Free(std::string _name) {
  if (this->allocPool.find(_name) != this->allocPool.end()) {
    if (this->allocPool[_name] != nullptr) {
      std::free(this->allocPool[_name]);
      this->allocPool[_name] = nullptr;
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
    if ((*iter).second != nullptr) {
      std::free((*iter).second);
      (*iter).second = nullptr;
    }
  }
  this->allocPool.clear();
}

// 释放DBMS各模块内存准备退出程序
void DBAllocator::Collapse() {
  TRACE("DBMS Environment Begin to Collapse");
  // 释放全部缓冲区
  if (DBAllocator::Instance != nullptr) {
    DBAllocator::Instance->FreeAll();
  }
  // 保存表格
  CStore::TableManager::GetInstance()->SaveContext();
}

// 唯一实例
DBAllocator* DBAllocator::Instance = nullptr;

CSTORE_NS_END