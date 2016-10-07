#ifndef ___CSTORE_DBLOCK
#define ___CSTORE_DBLOCK
#include "DBBase.h"
#include "DBTransaction.hpp"

CSTORE_NS_BEGIN

class DBLock : public DBObject {
public:
  // 构造器
  CONSTRUCTOR_SIMPLE_DBOBJECT(DBLock, "DBLock");

  //函数作用： 重写字符串化方法
  //参数列表： N/A
  //返 回 值： 锁的说明
  virtual std::string ToString() {
    CSCommonUtil::StringBuilder sb(this->GetTypename() + " [Type:");
    switch (this->Type)
    {
    case TableLockType::tblock_share:
      sb.Append("Share");
      break;
    case TableLockType::tblock_mutex:
      sb.Append("Mutex");
      break;
    case TableLockType::tblock_none:
      sb.Append("None");
      break;
    }
    sb.Append(", Binding:" + (this->LockBinding == NULL ? "NULL" : this->LockBinding->ToString()) + "]");
    return sb.ToString();
  }

  //函数作用： 互斥锁定
  //参数列表：
  //    locker 触发锁的事务
  //返 回 值： N/A
  inline void LockWrite(DBTransaction* locker) {
    this->synLockObjWrite.lock();
    this->synLockObjRead.lock();
    this->LockTransaction = locker;
    this->Type = TableLockType::tblock_mutex;
  }

  //函数作用： 共享锁定
  //参数列表：
  //    locker 触发锁的事务
  //返 回 值： N/A
  inline void LockRead(DBTransaction* locker) {
    this->lockMutex.lock();
    if (this->Type == TableLockType::tblock_mutex) {
      this->synLockObjRead.lock();
    }
    else {
      this->synLockObjRead.try_lock();
    }
    this->readHandleNum++;
    this->LockTransaction = locker;
    this->Type = TableLockType::tblock_share;
    this->lockMutex.unlock();
  }

  //函数作用： 互斥解锁
  //参数列表： N/A
  //返 回 值： N/A
  inline void UnlockWrite() {
    this->synLockObjWrite.unlock();
    this->synLockObjRead.unlock();
    this->Type = TableLockType::tblock_none;
  }

  //函数作用： 共享解锁
  //参数列表： N/A
  //返 回 值： N/A
  inline void UnlockRead() {
    this->lockMutex.lock();
    this->readHandleNum--;
    if (readHandleNum == 0) {
      this->synLockObjRead.unlock();
      this->Type = TableLockType::tblock_none;
    }
    this->lockMutex.unlock();
  }

  //函数作用： 获取锁的状态
  //参数列表： N/A
  //返 回 值： 锁状态枚举
  inline TableLockType GetState() {
    return this->Type;
  }

  //函数作用： 复位锁
  //参数列表： N/A
  //返 回 值： N/A
  inline void Reset() {
    this->LockBinding = NULL;
    this->LockTransaction = NULL;
  }

  // 锁施加对象的指针
  DBObject* LockBinding = NULL;
  // 锁分配给的事务
  DBTransaction* LockTransaction = NULL;

private:
  // 锁的状态
  TableLockType Type = TableLockType::tblock_none;
  // 写锁
  std::mutex synLockObjWrite;
  // 读锁
  std::mutex synLockObjRead;
  // 自锁
  std::mutex lockMutex;
  // 读句柄数量
  int readHandleNum = 0;

  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBLock);
}; /* DBLock */

CSTORE_NS_END

#endif /* ___CSTORE_DBLOCK */