#ifndef ___CSTORE_DBCONNECTIONPOOL
#define ___CSTORE_DBCONNECTIONPOOL
#include "DBBase.h"
#include "DBTransaction.hpp"

CSTORE_NS_BEGIN

class DBConnectionPool : public DBObject {
public:
  //函数作用： 工厂方法，获得类的唯一实例
  //参数列表： N/A
  //返 回 值： 连接器的唯一实例
  static DBConnectionPool* GetInstance();

  //函数作用： 将事务提交给数据库引擎
  //参数列表：
  //     query 要提交的事务查询语句集合
  //返 回 值： 连接器的唯一实例
  void Commit(const std::string);

  //函数作用： 强行中断所有事务并清空队列
  //参数列表： N/A
  //返 回 值： N/A
  void KillAll();

  //函数作用： 清空事务队列
  //参数列表： N/A
  //返 回 值： N/A
  void ClearQueue();

  //函数作用： 事务队列中排队的数量
  //参数列表： N/A
  //返 回 值： 排队事务数
  int CountQueue();

  //函数作用： 正在进行的事务数量
  //参数列表： N/A
  //返 回 值： 进行中事务数
  int CountProcessing();

  //函数作用： 获取已完成的事务说明
  //参数列表： N/A
  //返 回 值： 含有已完成事务详细说明的字符串
  std::string ShowFinishedTransaction();

  //函数作用： 设置线程数
  //参数列表：
  //      tnum 线程数
  //返 回 值： N/A
  void SetThreadNum(int);

  //函数作用： 设置DEBUG输出状态
  //参数列表：
  //     state 是否debug
  //返 回 值： N/A
  void SetDebug(bool);

  //函数作用： 析构器
  //参数列表： N/A
  //返 回 值： N/A
  ~DBConnectionPool();

private:
  //函数作用： 私有的构造器
  //参数列表： N/A
  //返 回 值： N/A
  DBConnectionPool();

  //函数作用： 事务处理器
  //参数列表： N/A
  //返 回 值： N/A
  static void TransactionHandler();

  // 结束标志
  bool quitFlag;
  // 事务队列
  std::queue<DBTransaction*> transactionQueue;
  // 处理中事务容器
  std::vector<DBTransaction*> processingTransactionVector;
  // 已完成事务容器
  std::vector<DBTransaction*> finishedTransactionVector;
  // 线程池
  std::vector<std::thread> threadPool;
  // 计数器锁
  std::mutex encounterMutex;
  // 队列锁
  std::mutex queueMutex;
  // 线程数
  int HandleNum;
  // 进行中线程计数器
  int ProcCounter;
  // 是否输出DEBUG信息
  bool isDebug;
  // 唯一实例
  static DBConnectionPool* instance;
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBConnectionPool);
}; /* DBConnectionPool */

CSTORE_NS_END
#endif /* ___CSTORE_DBCONNECTIONPOOL */