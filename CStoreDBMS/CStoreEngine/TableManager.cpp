#include "TableManager.h"

CSTORE_NS_BEGIN

// 添加表
bool TableManager::AddTable(const std::string& tabName, const std::string& loadFile) {
  // 不能重名
  if (this->GetTable(tabName)) {
    TRACE("Expect to create table: " + tabName + ", but table already exist.");
    return false;
  }
  // 构造新的表对象
  DBTable* ntb = new DBTable(tabName, loadFile);
  this->tableContainer.push_back(ntb);
  // 分配一个锁
  DBLock* ntl = new DBLock();
  ntl->LockBinding = ntb;
  this->lockContainer.push_back(ntl);
}

// 删除表
bool TableManager::DropTable(const std::string& tabName) {
  // 获取表对象
  DBTable* tabObj = this->GetTable(tabName);
  if (tabObj == NULL) {
    return false;
  }
  // 移除锁对象，此处不管是否上锁都会触发表删除，请在执行器处判断锁的状态
  for (std::vector<DBLock*>::iterator iter = this->lockContainer.begin(); iter != this->lockContainer.end(); iter++) {
    if ((*iter) != NULL && (*iter)->LockBinding->ReferenceEquals(tabObj)) {
      delete (*iter);
      this->lockContainer.erase(iter);
      return true;
    }
  }
  // 移除表对象
  for (std::vector<DBTable*>::iterator iter = this->tableContainer.begin(); iter != this->tableContainer.end(); iter++) {
    if ((*iter) != NULL && (*iter)->TableName == tabName) {
      delete (*iter);
      this->tableContainer.erase(iter);
      return true;
    }
  }
  TRACE("Expect to delete: " + tabName + ", but table not exist.");
  return false;
}

// 获取表对象
DBTable* TableManager::GetTable(const std::string& tabName) {
  for (int i = 0; i < this->tableContainer.size(); i++) {
    if (this->tableContainer[i]->TableName == tabName) {
      return this->tableContainer[i];
    }
  }
  TRACE("Expect to get: " + tabName + ", but table not exist.");
  return NULL;
}

// 获取表的锁对象
DBLock* TableManager::GetTableLock(const std::string& tabName) {
  // 获取表对象
  DBTable* tabObj = this->GetTable(tabName);
  if (tabObj == NULL) {
    return NULL;
  }
  // 移除锁对象，此处不管是否上锁都会触发表删除，请在执行器处判断锁的状态
  for (std::vector<DBLock*>::iterator iter = this->lockContainer.begin(); iter != this->lockContainer.end(); iter++) {
    if ((*iter) != NULL && (*iter)->LockBinding->ReferenceEquals(tabObj)) {
      return (*iter);
    }
  }
  return NULL;
}

// 获取所有表的描述信息
std::string TableManager::ShowTable() {

  return "";
}

// 统计数据库中表的数量
int TableManager::CountTable() {
  return this->tableContainer.size();
}

// 清空数据库所有表格
void TableManager::Clear() {
  for (int i = 0; i < this->tableContainer.size(); i++) {
    if (this->tableContainer[i] != NULL) {
      delete this->tableContainer[i];
    }
  }
  this->tableContainer.clear();
}

// 将当前数据库表状态保存到磁盘
void TableManager::SaveContext() {

}

// 将当前数据库表状态从磁盘读入
bool TableManager::LoadContext() {

  return true;
}

// 私有的构造函数
TableManager::TableManager()
  :DBObject("TableManager", this) {

}

CSTORE_NS_END
