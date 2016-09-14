#ifndef ___CSTORE_DBCTOKEN
#define ___CSTORE_DBCTOKEN
#include "DBCDeclaration.h"
class Token {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  Token();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  explicit Token(const Token&);

  // 词类型
  TokenType aType;
  // 命中行
  int aLine;
  // 命中列
  int aColumn;
  // 附加值
  int aTag;
  // 位置戳
  int indexOfCode;
  // 配对长
  int length;
  // 错误位
  int errorCode;
  // 错误码
  istr errorInfo;
  // 原信息
  istr detail;
}; /* Token */

#endif /* ___CSTORE_DBCTOKEN */