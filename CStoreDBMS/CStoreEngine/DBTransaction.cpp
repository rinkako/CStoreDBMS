#include "DBTransaction.hpp"

CSTORE_NS_BEGIN

// 构造器
DBTransaction::DBTransaction(std::string queryCode)
  :DBObject("DBTransaction", this) {
  id = DBTransaction::encounter++;
  time_t ts = std::time(NULL);
  tm* localt = std::localtime(&ts);
  char timeBuffer[10];
  std::sprintf(timeBuffer, "%02d:%02d:%02d", localt->tm_hour, localt->tm_min, localt->tm_sec);
  this->timestamp = std::string(timeBuffer);
  this->successFlag = false;
  this->finishstamp = "";
  this->duration = "";
  this->code = queryCode;
  beginStampObj = std::clock();
}

//函数作用： 标记事务成功
//参数列表： N/A
//返 回 值： N/A
void DBTransaction::Finish() {
  time_t ts = std::time(NULL);
  tm* localt = std::localtime(&ts);
  char timeBuffer[16];
  std::sprintf(timeBuffer, "%02d:%02d:%02d", localt->tm_hour, localt->tm_min, localt->tm_sec);
  this->finishstamp = std::string(timeBuffer);
  std::sprintf(timeBuffer, "%.8lf", (double)(std::clock() - this->beginStampObj) / (double)CLOCKS_PER_SEC);
  this->duration = std::string(timeBuffer);
  this->successFlag = true;
}

// 重写字符串化方法
std::string DBTransaction::ToString() {
  CSCommonUtil::StringBuilder sb;
  sb.Append("ID:\t").Append(this->id).Append(NEWLINE);
  sb.Append("Tid:\t").Append(this->handleId).Append(NEWLINE);
  sb.Append("Fin:\t").Append(this->successFlag ? "True" : "False").Append(NEWLINE);
  sb.Append("Begin:\t").Append(this->timestamp).Append(NEWLINE);
  sb.Append("Over:\t").Append(this->successFlag ? this->finishstamp : "---").Append(NEWLINE);
  sb.Append("Cost:\t").Append(this->successFlag ? this->GetDuration() : "---").Append(NEWLINE);
  sb.Append("Code:\t").Append((this->code.length() > 15 ? this->code.substr(0, 14) + "..." : this->code)).Append(NEWLINE);
  return sb.ToString();
}

// 静态计数器
int DBTransaction::encounter = 0;

CSTORE_NS_END
