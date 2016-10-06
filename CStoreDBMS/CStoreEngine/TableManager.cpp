#include "TableManager.h"

CSTORE_NS_BEGIN

// 工厂方法
TableManager* TableManager::GetInstance() {
  return TableManager::Instance == NULL ?
    TableManager::Instance = new TableManager() : TableManager::Instance;
}

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
  // 追加下标索引
  this->tableIndexDict[tabName] = this->tableContainer.size() - 1;
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
  // 更新索引记录
  if (this->tableIndexDict.find(tabName) != this->tableIndexDict.end()) {
    this->tableIndexDict.erase(tabName);
  }
  this->tableIndexDict.clear();
  for (int i = 0; i < this->tableContainer.size(); i++) {
    this->tableIndexDict[this->tableContainer[i]->TableName] = i;
  }
  TRACE("Expect to delete: " + tabName + ", but table not exist.");
  return false;
}

// 获取表对象
DBTable* TableManager::GetTable(const std::string& tabName) {
  if (this->tableIndexDict.find(tabName) != this->tableIndexDict.end()) {
    return this->tableContainer[this->tableIndexDict[tabName]];
  }
  return NULL;
}

// 判断表的存在
bool TableManager::ExistTable(const std::string& tabName) {
  return this->GetTable(tabName) == NULL;
}

// 获取表的锁对象
DBLock* TableManager::GetTableLock(const std::string& tabName) {
  // 获取表对象
  DBTable* tabObj = this->GetTable(tabName);
  if (tabObj == NULL) {
    return NULL;
  }
  // 返回锁
  for (std::vector<DBLock*>::iterator iter = this->lockContainer.begin(); iter != this->lockContainer.end(); iter++) {
    if ((*iter) != NULL && (*iter)->LockBinding->ReferenceEquals(tabObj)) {
      return (*iter);
    }
  }
  return NULL;
}

// 获取所有表的描述信息
std::string TableManager::ShowTable() {
  CSCommonUtil::StringBuilder sb;
  for (int i = 0; i < this->tableContainer.size(); i++) {
    sb.Append(this->tableContainer[i]->ToString()).Append(NEWLINE);
  }
  return sb.ToString();
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
  this->tableIndexDict.clear();
  this->tableContainer.clear();
  this->SaveContext();
}

// 将当前数据库表状态保存到磁盘
void TableManager::SaveContext() {
  std::ofstream f("table.dbconf");
  for (int i = 0; i < this->tableContainer.size(); i++) {
    DBTable* t = this->tableContainer[i];
    f << t->TableName << "$";
    if (t->PiList.size() > 0) {
      f << t->PiList[0] << "@" << t->PiTypeList[0];
      for (int j = 1; j < t->PiList.size(); j++) {
        f << "," << t->PiList[j] << "@" << t->PiTypeList[j];
      }
    }
    f << NEWLINE;
  }
  f.close();
}

// 将当前数据库表状态从磁盘读入
bool TableManager::LoadContext() {
  // 没有配置文件时
  std::ifstream f("table.dbconf");
  if (!f) {
    TRACE("No table config file found, DBMS will be reset.");
    f.close();
    return false;
  }
  // 读取配置文件，还原表格们
  char buf[256];
  while (!f.eof()) {
    f.getline(buf, 256);
    std::string line(buf);
    std::vector<std::string> lineitem = CSCommonUtil::CStrSplit(line, "$");
    if (lineitem.size() == 2) {
      if (this->AddTable(lineitem[0], lineitem[0]) == false) {
        TRACE("Cannot reload table:" + lineitem[0]);
        continue;
      }
      DBTable* tobj = this->GetTable(lineitem[0]);
      std::vector<std::string> piitem = CSCommonUtil::CStrSplit(lineitem[1], ",");
      for (int p = 0; p < piitem.size(); p++) {
        std::vector<std::string> pisingle = CSCommonUtil::CStrSplit(piitem[p], "@");
        tobj->PiList.push_back(pisingle[0]);
        tobj->PiTypeList.push_back(pisingle[1]);
        tobj->PiFileNameList[pisingle[0]] = lineitem[0] + "_" + pisingle[0] + ".db";
      }
    }
  }
  f.close();
  return true;
}

// 析构函数
TableManager::~TableManager() {
  this->SaveContext();
}

// 私有的构造函数
TableManager::TableManager()
  :DBObject("TableManager", this) {
  this->LoadContext();
}

// 唯一实例
TableManager* TableManager::Instance = NULL;

CSTORE_NS_END
