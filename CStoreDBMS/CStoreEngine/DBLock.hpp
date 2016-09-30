#ifndef ___CSTORE_DBLOCK
#define ___CSTORE_DBLOCK
#include "DBBase.h"

class DBLock : public DBObject {
public:
  // 构造器
  CONSTRUCTOR_SIMPLE_DBOBJECT(DBLock, "DBLock");

  // 重载字符串化方法
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
}; /* DBLock */

#endif /* ___CSTORE_DBLOCK */