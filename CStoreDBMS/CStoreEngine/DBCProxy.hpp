#ifndef ___CSTORE_DBCPROXY
#define ___CSTORE_DBCPROXY
#include "DBCBase.h"

class DBCProxy : public DBObject {
public:
  CONSTRUCTOR_SIMPLE_DBOBJECT(DBCProxy, "DBCProxy");
  // 句子号
  int id;
  // 列通配符
  bool isStar;
  // 邂逅主键声明词次数
  int encounter;
  // 错误位
  bool errorBit;
  // 操作码
  DashType opCode;
  // 操作表
  istr opTable;
  // 条件子句的指针
  SyntaxTreeNode* condPtr;
  // 操作列向量
  StrVec Pi;
  // 列赋值向量
  IntVec ProxyPi;
  // 初始值向量
  TablePileDictionary DefaPi;
  // 条件列向量
  StrVec CondPi;
  // 主键列向量
  StrVec PrimaryPi;
}; /* DBCProxy */

#endif /* ___CSTORE_DBCPROXY */