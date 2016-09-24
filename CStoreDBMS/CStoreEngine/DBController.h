#ifndef __CSTORE_DBCONTROLLER
#define __CSTORE_DBCONTROLLER
#include "DBBase.h"

class DBController {
public:
  // 函数作用： 工厂方法，获得类的唯一实例
  // 参数列表： N/A
  // 返 回 值： 类在内存中唯一实例的引用
  static DBController* Invoke();
 
  // 函数作用： 初始化当前的数据库引擎
  // 参数列表： 
  //       argc 参数的个数
  //       argv 参数向量
  // 返 回 值： N/A
  void DBInit(int argc, char* argv[]);

  // 函数作用： 将前端动作转化为后台动作
  // 参数列表： N/A
  // 返 回 值： 操作成功与否
  bool DBStartDash();

private:
  // 函数作用： 私有的构造器
  // 参数列表： N/A
  DBController();

  // 唯一实例
  static DBController* Instance;

  // 阻止复制和赋值
  DISALLOW_COPY_AND_ASSIGN(DBController);
};


#endif // __CSTORE_DBCONTROLLER