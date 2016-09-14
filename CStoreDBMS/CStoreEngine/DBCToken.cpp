#include "DBCToken.h"

// Token构造器
Token::Token() {
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
Token::Token(const Token &_other) {
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