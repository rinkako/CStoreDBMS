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
    std::string sb = this->GetTypename() + " [Type:";
    switch (this->Type)
    {
    case TableLockType::tblock_share:
      sb += "Share";
      break;
    case TableLockType::tblock_mutex:
      sb += "Mutex";
      break;
    case TableLockType::tblock_none:
      sb += "None";
      break;
    }
    sb += ", Binding:" + (this->LockBinding == NULL ? "NULL" : this->LockBinding->ToString());
    return sb;
  }

  // 锁的状态
  TableLockType Type;

  // 锁施加对象的指针
  DBObject* LockBinding = NULL;

  // 锁分配给的事务
  DBTransaction* LockTransaction = NULL;

private:
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBLock);
}; /* DBLock */

CSTORE_NS_END

#endif /* ___CSTORE_DBLOCK */