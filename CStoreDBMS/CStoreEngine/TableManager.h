#ifndef ___CSTORE_TABLEMANAGER
#define ___CSTORE_TABLEMANAGER
#include "DBBase.h"
#include "DBTable.hpp"
#include <string>
#include <vector>

class TableManager {
public:
  //函数作用： 添加表
  //参数列表：
  //   tabName 表名
  //  loadFile 要载入的表文件
  //返 回 值： 操作成功与否
  bool AddTable(std::string, std::string);

  //函数作用： 删除表
  //参数列表：
  //   tabName 表名
  //返 回 值： 操作成功与否
  bool DropTable(std::string);

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
  //返 回 值： 操作成功与否
  bool SaveContext();

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
  std::vector<DBTable> tableContainer;

  // 锁容器对象
  std::vector<TableLockType> lockContainer;

  // 唯一实例
  static TableManager* instance;
}; /* TableManager */

#endif /* ___CSTORE_TABLEMANAGER */