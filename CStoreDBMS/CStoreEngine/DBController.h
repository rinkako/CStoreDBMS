#ifndef __CSTORE_DBCONTROLLER
#define __CSTORE_DBCONTROLLER
#include "DBBase.h"
#include "DBConnectionPool.h"

CSTORE_NS_BEGIN

class DBController : public DBObject {
public:
  //函数作用： 工厂方法，获得类的唯一实例
  //参数列表： N/A
  //返 回 值： 类在内存中唯一实例的引用
  static DBController* Invoke();

  //函数作用： 初始化当前的数据库引擎并开始交互
  //参数列表：
  //      argc 参数的个数
  //      argv 参数向量
  //返 回 值： N/A
  void StartDash(int, char**);

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
  void Dash(const std::string&);

  //函数作用： 处理特殊命令
  //参数列表：
  //     query 查询语句
  //返 回 值： N/A
  bool ReservedRouter(const std::string&);

  //函数作用： 私有的构造器
  //参数列表： N/A
  DBController();

  // 命令行临时查询语句
  std::string tempQuery = "";
  // 是否开启Debug模式
  bool DebugMode = false;
  // 当前状态
  RunType runType = RunType::RUN_CONSOLE;
  // 运行的路径
  std::string runPath = "";
  // 连接器
  DBConnectionPool* connector = nullptr;
  // 唯一实例
  static DBController* Instance;
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBController);
};

CSTORE_NS_END

#endif // __CSTORE_DBCONTROLLER