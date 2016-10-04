#ifndef ___CSTORE_DBCPROXY
#define ___CSTORE_DBCPROXY
#include "DBCBase.h"

CSTORE_NS_BEGIN

class DBCProxy : public DBObject {
public:
  // 构造器
  CONSTRUCTOR_SIMPLE_DBOBJECT(DBCProxy, "DBCProxy");
  // 句子号
  int id;
  // 列通配符
  bool isStar;
  // 邂逅主键声明词次数
  int encounter;
  // 错误位
  bool errorBit;
  // 附加值
  double aTag;
  // 操作码
  DashType opCode;
  // 操作表
  istr opTable;
  // 操作表二
  istr opTablePaired;
  // 要载入的文件名
  istr loadFile;
  // 条件子句的指针
  SyntaxTreeNode* condPtr;
  // 操作列向量
  StrVec Pi;
  // 列类型
  StrVec PiType;
  // 列赋值向量
  IntVec ProxyPi;
  // 初始值向量
  TablePileDictionary DefaPi;
  // 条件列向量
  StrVec CondPi;
  // 主键列向量
  StrVec PrimaryPi;
}; /* DBCProxy */

CSTORE_NS_END

#endif /* ___CSTORE_DBCPROXY */