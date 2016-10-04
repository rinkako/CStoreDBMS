#ifndef ___CSTORE_TABLEMANAGER
#define ___CSTORE_TABLEMANAGER
#include "DBBase.h"
#include "DBLock.hpp"
#include "DBTable.hpp"

CSTORE_NS_BEGIN

class TableManager : public DBObject {
public:
  //函数作用： 工厂方法，获得类的唯一实例
  //参数列表： N/A
  //返 回 值： 表管理器的唯一实例
  static TableManager* GetInstance();

  //函数作用： 添加表
  //参数列表：
  //   tabName 表名
  //  loadFile 要载入的表文件
  //返 回 值： 操作成功与否
  bool AddTable(const std::string&, const std::string&);

  //函数作用： 删除表
  //参数列表：
  //   tabName 表名
  //返 回 值： 操作成功与否
  bool DropTable(const std::string&);

  //函数作用： 判断表的存在
  //参数列表：
  //   tabName 表名
  //返 回 值： 表是否存在
  bool ExistTable(const std::string&);

  //函数作用： 获取表对象
  //参数列表：
  //   tabName 表名
  //返 回 值： 表对象的指针
  DBTable* GetTable(const std::string&);

  //函数作用： 获取表的锁对象
  //参数列表：
  //   tabName 表名
  //返 回 值： 表锁的指针
  DBLock* GetTableLock(const std::string&);

  //函数作用： 获取所有表的描述信息
  //参数列表： N/A
  //返 回 值： 表的描述字符串
  std::string ShowTable();

  //函数作用： 统计数据库中表的数量
  //参数列表： N/A
  //返 回 值： 表的数量
  int CountTable();

  //函数作用： 清空数据库所有表格
  //参数列表： N/A
  //返 回 值： N/A
  void Clear();

  //函数作用： 将当前数据库表状态保存到磁盘
  //参数列表： N/A
  //返 回 值： N/A
  void SaveContext();

  //函数作用： 将当前数据库表状态从磁盘读入
  //参数列表： N/A
  //返 回 值： 操作成功与否
  bool LoadContext();

private:
  //函数作用： 私有的构造函数
  //参数列表： N/A
  //返 回 值： N/A
  TableManager();

  // 表对象容器
  std::vector<DBTable*> tableContainer;

  // 锁容器对象
  std::vector<DBLock*> lockContainer;

  // 表在容器中的下标索引
  std::map<std::string, int> tableIndexDict;

  // 唯一实例
  static TableManager* Instance;
}; /* TableManager */

CSTORE_NS_END

#endif /* ___CSTORE_TABLEMANAGER */