#ifndef __CSTORE_DBCONTROLLER
#define __CSTORE_DBCONTROLLER
#include "DBBase.h"
#include "DBBridge.h"
#include <string>

class DBController {
public:
  //函数作用： 工厂方法，获得类的唯一实例
  //参数列表： N/A
  //返 回 值： 类在内存中唯一实例的引用
  static DBController* Invoke();
 
  //函数作用： 初始化当前的数据库引擎
  //参数列表： 
  //      argc 参数的个数
  //      argv 参数向量
  //返 回 值： N/A
  void DBInit(int argc, char* argv[]);

  //函数作用： 将前端动作转化为后台动作
  //参数列表： N/A
  //返 回 值： N/A
  void DBStartDash();

  //函数作用： 获取当前数据库状态
  //参数列表： N/A
  //返 回 值： 当前数据库状态的枚举
  RunType GetRunType();

  //函数作用： 获取数据库程序在系统的运行路径
  //参数列表： N/A
  //返 回 值： 运行路径字符串
  std::string GetRunPath();

  //函数作用： 析构器
  //参数列表： N/A
  ~DBController();

private:
  //函数作用： 处理控制台终端输入
  //参数列表： N/A
  //返 回 值： N/A
  void Terminal();

  //函数作用： 处理查询语句输入
  //参数列表： N/A
  //返 回 值： N/A
  void Dash(const std::string& query);

  //函数作用： 私有的构造器
  //参数列表： N/A
  DBController();

  // 唯一实例
  static DBController* Instance;

  // 当前状态
  RunType runType = RunType::RUN_CONSOLE;

  // 运行的路径
  std::string runPath = "";

  // 命令行临时查询语句
  std::string tempQuery = "";

  // 桥容器
  DBBridge* IBridge = NULL;

  // 阻止复制和赋值
  DISALLOW_COPY_AND_ASSIGN(DBController);
};


#endif // __CSTORE_DBCONTROLLER