#include "DBCToken.h"

CSTORE_NS_BEGIN

// Token构造器
Token::Token() : DBObject("Token", this) {
  aLine = 0;
  aTag = 0;
  indexOfCode = 0;
  aColumn = 0;
  length = 0;
  errorCode = 0;
  errorInfo = "";
  detail = "";
}

// Token拷贝构造器
Token::Token(const Token &_other) : DBObject("Token", this) {
  this->indexOfCode = _other.indexOfCode;
  this->errorCode = _other.errorCode;
  this->errorInfo = _other.errorInfo;
  this->aColumn = _other.aColumn;
  this->length = _other.length;
  this->detail = _other.detail;
  this->aLine = _other.aLine;
  this->aType = _other.aType;
  this->aTag = _other.aTag;
}

CSTORE_NS_END
