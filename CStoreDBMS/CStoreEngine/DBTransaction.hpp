#ifndef ___CSTORE_DBTRANSACTION
#define ___CSTORE_DBTRANSACTION
#include "DBBase.h"

CSTORE_NS_BEGIN

class DBTransaction : public DBObject {
public:
  //函数作用： 构造器
  //参数列表：
  // queryCode 查询代码
  //返 回 值： N/A
  DBTransaction(std::string);

  //函数作用： 获取事务的编号
  //参数列表： N/A
  //返 回 值： 代表事务编号的整数
  inline int GetId() { return this->id; }

  //函数作用： 获取事务的查询代码
  //参数列表： N/A
  //返 回 值： 查询语句字符串
  inline std::string GetCode() { return this->code; }

  //函数作用： 获取事务的完成情况
  //参数列表： N/A
  //返 回 值： 是否已经完成
  inline bool GetState() { return this->successFlag; }

  //函数作用： 获取事务的完成时刻
  //参数列表： N/A
  //返 回 值： 完成时刻字符串
  inline std::string GetFinishTimestamp() { return this->finishstamp; }

  //函数作用： 获取事务的开始时刻
  //参数列表： N/A
  //返 回 值： 完成时刻字符串
  inline std::string GetBeginTimestamp() { return this->timestamp; }

  //函数作用： 标记事务成功
  //参数列表： N/A
  //返 回 值： N/A
  inline void Finish() { this->successFlag = true; }

  //函数作用： 获取事务持续时长
  //参数列表： N/A
  //返 回 值： 持续时间字符串，以秒为单位
  std::string GetDuration();

  //函数作用： 重写字符串化方法
  //参数列表： N/A
  //返 回 值： 事务的说明
  virtual std::string ToString();

private:
  // 事务id
  int id;

  // 事物是否已成功完成
  bool successFlag;

  // 事务的查询代码
  std::string code;

  // 发生的时间戳
  std::string timestamp;

  // 完成的时间戳
  std::string finishstamp;

  // 发生时间戳对象
  std::clock_t beginStampObj;

  // 事务计数器
  static int encounter;

  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBTransaction);
}; /* DBTransaction */

CSTORE_NS_END

#endif /* ___CSTORE_DBTRANSACTION */