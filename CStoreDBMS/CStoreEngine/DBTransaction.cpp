#include "DBTransaction.hpp"

CSTORE_NS_BEGIN

// 构造器
DBTransaction::DBTransaction(std::string queryCode)
  :DBObject("DBTransaction", this) {
  id = DBTransaction::encounter++;
  time_t ts = time(NULL);
  tm* __CSTRACETIMESTAMPPTR__ = localtime(&ts);
  char timeBuffer[10];
  sprintf(timeBuffer, "%02d:%02d:%02d", __CSTRACETIMESTAMPPTR__->tm_hour, __CSTRACETIMESTAMPPTR__->tm_min, __CSTRACETIMESTAMPPTR__->tm_sec);
  this->timestamp = std::string(timeBuffer);
  this->successFlag = false;
  this->finishstamp = "";
  this->code = queryCode;
  beginStampObj = std::clock();
}

// 获取事务持续时长
inline std::string DBTransaction::GetDuration() {
  char dBuffer[10];
  sprintf(dBuffer, "%.8lf", (double)(std::clock() - this->beginStampObj) / (double)CLOCKS_PER_SEC);
  return std::string(dBuffer);
}

// 重写字符串化方法
std::string DBTransaction::ToString() {
  std::string sb = "";
  char buf[10];
  _itoa(this->id, buf, 10);
  sb += "ID:\t" + std::string(buf) + NEWLINE;
  sb += "Begin:\t" + this->timestamp + NEWLINE;
  sb += "Over:\t" + this->finishstamp + NEWLINE;
  sb += "Cost:\t" + this->GetDuration() + NEWLINE;
  sb += "Code:\t" + (this->code.length() > 15 ? this->code.substr(0, 14) + "..." : this->code) + NEWLINE;
  return sb;
}

// 静态计数器
int DBTransaction::encounter = 0;

CSTORE_NS_END
