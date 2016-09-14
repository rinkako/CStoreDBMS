#include "ILexicalAnalyzer.h"

// LexicalAnalyzer构造器
LexicalAnalyzer::LexicalAnalyzer() {
  this->Reset();
}

// LexicalAnalyzer带源码的构造器
LexicalAnalyzer::LexicalAnalyzer(const istr& _src) {
  this->SetSourceCode(_src);
}

// LexicalAnalyzer复位词法分析器
void LexicalAnalyzer::Reset() {
  sourceCode = "";
  TempStr = "";
  sentenceContainer.clear();
  currentLine = ROWSTARTNUM;
  currentColumn = COLSTARTNUM;
  PTRnextLetter = 0;
  if (currentList != NULL) {
    currentList->_tokenContainer.clear();
    delete currentList;
    currentList = NULL;
  }
}

// LexicalAnalyzer获得句子容器
StrVec* LexicalAnalyzer::GetStrVec() {
  return &(this->sentenceContainer);
}

// LexicalAnalyzer启动SSQL词法分析自动机
TokenStream* LexicalAnalyzer::Analyze() {
  currentList = new TokenStream();
  if (sourceCode == "") {
    return currentList;
  }
  else {
    int bound = this->GetSourceCode().length();
    while (PTRnextLetter < bound) {
      Token *kotori = this->NextToken();
      if (kotori != NULL) {
        currentList->Add(kotori);
      }
    }
  }
  // 返回前清空缓冲区
  if (TempStr != "") {
    sentenceContainer.push_back(TempStr);
  }
  return currentList;
}

// LexicalAnalyzer获取源代码
istr LexicalAnalyzer::GetSourceCode() {
  return sourceCode;
}

// LexicalAnalyzer设置待处理源代码
void LexicalAnalyzer::SetSourceCode(const istr& _src) {
  this->Reset();
  sourceCode = _src;
}

// LexicalAnalyzer-DFA
Token* LexicalAnalyzer::NextToken() {
  // 定义结果变量并初始化
  Token* result = new Token();
  result->aLine = this->currentLine;
  result->aColumn = this->currentColumn;
  result->indexOfCode = this->PTRnextLetter;
  int alen = this->GetSourceCode().length();
  // 如果越界就返回
  if (PTRnextLetter < 0 || PTRnextLetter >= alen) {
    return result;
  }
  // 获取下一字符来判断token
  bool successFlag = false;
  CharaType cara = GetCharType(this->GetSourceCode()[PTRnextLetter]);
  switch (cara)
  {
    // 单字符token
  case CharaType::Plus:
  case CharaType::Minus:
  case CharaType::Multiply:
  case CharaType::Divide:
  case CharaType::Not:
  case CharaType::LeftParentheses:
  case CharaType::RightParentheses:
  case CharaType::Comma:
  case CharaType::Semicolon:
    successFlag = this->GetOneCharaCalculator(result);
    break;
    // 可能双字符token
  case CharaType::Equality:
  case CharaType::LessThan:
  case CharaType::GreaterThan:
  case CharaType::And:
  case CharaType::Or:
    successFlag = this->GetTwoCharaCalculator(result);
    break;
    // 关键字和标识符
  case CharaType::Letter:
  case CharaType::UnderLine:
    successFlag = this->GetReservedCalculator(result);
    break;
    // 数值
  case CharaType::Number:
    successFlag = this->GetConstantNumber(result);
    break;
    // 空白
  case CharaType::Space:
    successFlag = this->GetSpace(result);
    break;
    // 谜
  default:
    successFlag = this->GetUnknown(result);
    break;
  }
  // 如果成功获得了token，就返回她
  if (successFlag) {
    result->length = PTRnextLetter - result->indexOfCode;
    return result;
  }
  // 否则返回空
  return NULL;
}

// LexicalAnalyzer获取器，得到空白对应的Token
bool LexicalAnalyzer::GetSpace(Token *result) {
  // 获取字符，看是否要换行
  char c = this->GetSourceCode()[PTRnextLetter];
  if (c == '\n') {
    this->currentLine++;
    this->currentColumn = COLSTARTNUM;
  }
  // 为空格跳游程
  if (c == ' ') {
    Jump(1);
  }
  else {
    PTRnextLetter++;
  }
  // 空token，永久为false
  return false;
}

// LexicalAnalyzer获取器，得到未知字符对应的Token
bool LexicalAnalyzer::GetUnknown(Token *result) {
  // 生成错误token
  result->aType = TokenType::unknown;
  result->detail = this->GetSourceCode()[PTRnextLetter];
  result->errorCode = 1;
  result->errorInfo = "错误：未知的字符" + result->detail + "\n";
  // 跳游程
  Jump(1);
  return true;
}

// LexicalAnalyzer获取器，得到标识符对应的Token
bool LexicalAnalyzer::GetIdentifier(Token *result) {

  // 把builder构造成完整的一个identifier
  istr builder = "";
  while (PTRnextLetter < (int)this->GetSourceCode().length()) {
    CharaType cara = this->GetCharType(this->GetSourceCode()[PTRnextLetter]);
    if (cara == CharaType::Letter
      || cara == CharaType::Number
      || cara == CharaType::UnderLine) {
      builder += this->GetSourceCode()[PTRnextLetter];
      Jump(1);
    }
    else {
      break;
    }
  }
  // 修改token标签
  result->aType = TokenType::token_iden;
  result->detail = builder;
  return true;
}

// LexicalAnalyzer获取器，得到常数对应的Token
bool LexicalAnalyzer::GetConstantNumber(Token *result) {
  istr numberBuilder = "";
  bool successFlag = false;
  char c;
  // 扫描数字序列
  while (PTRnextLetter < (int)this->GetSourceCode().length()) {
    c = this->GetSourceCode()[PTRnextLetter];
    if ('0' <= c && c <= '9') {
      // 压数字
      numberBuilder += c;
      // 跳动游程
      Jump(1);
      successFlag = true;
    }
    else {
      break;
    }
  }
  // 成功得到了数字token
  if (successFlag) {
    result->aType = TokenType::number;
    result->detail = numberBuilder;
    result->aTag = atoi(numberBuilder.c_str());
    return true;
  }
  return false;
}

// LexicalAnalyzer获取器，得到单字符符号对应的Token
bool LexicalAnalyzer::GetOneCharaCalculator(Token *result) {
  int glen = this->GetSourceCode().length();
  if (PTRnextLetter + 1 <= glen) {
    istr str = this->GetSourceCode().substr(PTRnextLetter, 1);
    result->detail = str;
    if (str == "+") {
      result->aType = TokenType::token_Plus_;
    }
    else if (str == "-") {
      result->aType = TokenType::token_Minus_;
    }
    else if (str == "*") {
      result->aType = TokenType::token_Multiply_;
    }
    else if (str == "/") {
      result->aType = TokenType::token_Divide_;
    }
    else if (str == "!") {
      result->aType = TokenType::token_Not_;
    }
    else if (str == "(") {
      result->aType = TokenType::token_LeftParentheses_;
    }
    else if (str == ")") {
      result->aType = TokenType::token_RightParentheses_;
    }
    else if (str == ",") {
      result->aType = TokenType::token_Comma_;
    }
    else if (str == ";") {
      result->aType = TokenType::token_Semicolon_;
    }
    // 递增字符指针
    Jump(1);
    return true;
  }
  return false;
}

// LexicalAnalyzer获取器，得到可能为双字符对应的Token
bool LexicalAnalyzer::GetTwoCharaCalculator(Token *result) {
  int glen = this->GetSourceCode().length();
  // 2字符的情况
  if (PTRnextLetter + 2 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 2);
    if (str == "==") {
      result->aType = TokenType::token_Equality_Equality_;
      okFlag = true;
    }
    else if (str == ">=") {
      result->aType = TokenType::token_GreaterThan_Equality_;
      okFlag = true;
    }
    else if (str == "<=") {
      result->aType = TokenType::token_LessThan_Equality_;
      okFlag = true;
    }
    else if (str == "&&") {
      result->aType = TokenType::token_And_And_;
      okFlag = true;
    }
    else if (str == "||") {
      result->aType = TokenType::token_Or_Or_;
      okFlag = true;
    }
    else if (str == "<>") {
      result->aType = TokenType::token_LessThan_GreaterThan_;
      okFlag = true;
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(2);
      return true;
    }
  }
  // 不符合状态转移，返回1字符情况
  if (PTRnextLetter + 1 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 1);
    if (str == ">") {
      result->aType = TokenType::token_GreaterThan_;
      okFlag = true;
    }
    else if (str == "<") {
      result->aType = TokenType::token_LessThan_;
      okFlag = true;
    }
    else if (str == "=") {
      result->aType = TokenType::token_Equality_;
      okFlag = true;
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(1);
      return true;
    }
  }
  return false;
}

// LexicalAnalyzer获取器，得到保留字对应的Token
bool LexicalAnalyzer::GetReservedCalculator(Token *result) {
  int glen = this->GetSourceCode().length();
  // 7字符的情况
  if (PTRnextLetter + 7 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 7);
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    if (str == "default") {
      result->aType = TokenType::token_default;
      if (PTRnextLetter + 7 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 7]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "primary") {
      result->aType = TokenType::token_primary;
      if (PTRnextLetter + 7 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 7]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(7);
      return true;
    }
  }
  // 状态转移，6字符
  if (PTRnextLetter + 6 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 6);
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    if (str == "create") {
      result->aType = TokenType::token_create;
      if (PTRnextLetter + 6 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 6]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "insert") {
      result->aType = TokenType::token_insert;
      if (PTRnextLetter + 6 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 6]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "values") {
      result->aType = TokenType::token_values;
      if (PTRnextLetter + 6 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 6]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "delete") {
      result->aType = TokenType::token_delete;
      if (PTRnextLetter + 6 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 6]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "select") {
      result->aType = TokenType::token_select;
      if (PTRnextLetter + 6 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 6]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(6);
      return true;
    }
  }
  // 状态转移，5字符
  if (PTRnextLetter + 5 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 5);
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    if (str == "table") {
      result->aType = TokenType::token_table;
      if (PTRnextLetter + 5 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 5]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "where") {
      result->aType = TokenType::token_where;
      if (PTRnextLetter + 5 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 5]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(5);
      return true;
    }
  }
  // 状态转移，4字符
  if (PTRnextLetter + 4 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 4);
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    if (str == "into") {
      result->aType = TokenType::token_into;
      if (PTRnextLetter + 4 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 4]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "from") {
      result->aType = TokenType::token_from;
      if (PTRnextLetter + 4 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 4]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(4);
      return true;
    }
  }
  // 状态转移，3字符
  if (PTRnextLetter + 3 <= glen) {
    bool okFlag = false;
    istr str = this->GetSourceCode().substr(PTRnextLetter, 3);
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    if (str == "int") {
      result->aType = TokenType::token_int;
      if (PTRnextLetter + 3 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 3]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    else if (str == "key") {
      result->aType = TokenType::token_key;
      if (PTRnextLetter + 3 <= glen) {
        if (this->GetCharType(this->GetSourceCode()[PTRnextLetter + 3]) != CharaType::Letter) {
          okFlag = true;
        }
      }
      else {
        okFlag = true;
      }
    }
    if (okFlag) {
      // 修改token的标识符
      result->detail = str;
      // 跳游程
      Jump(3);
      return true;
    }
  }
  // 在断定非保留字后，作为标识符处理
  if (this->GetIdentifier(result)) {
    return true;
  }
  return false;
}

// LexicalAnalyzer判断一个字符属于哪种类型
CharaType LexicalAnalyzer::GetCharType(const char& c)
{
  if ('a' <= c && c <= 'z') { return CharaType::Letter; }
  if ('A' <= c && c <= 'Z') { return CharaType::Letter; }
  if ('0' <= c && c <= '9') { return CharaType::Number; }
  if (c == '_')  { return CharaType::UnderLine; }
  if (c == '.')  { return CharaType::Dot; }
  if (c == ',')  { return CharaType::Comma; }
  if (c == '+')  { return CharaType::Plus; }
  if (c == '-')  { return CharaType::Minus; }
  if (c == '*')  { return CharaType::Multiply; }
  if (c == '/')  { return CharaType::Divide; }
  if (c == '%')  { return CharaType::Percent; }
  if (c == '^')  { return CharaType::Xor; }
  if (c == '&')  { return CharaType::And; }
  if (c == '|')  { return CharaType::Or; }
  if (c == '~')  { return CharaType::Reverse; }
  if (c == '$')  { return CharaType::Dollar; }
  if (c == '<')  { return CharaType::LessThan; }
  if (c == '>')  { return CharaType::GreaterThan; }
  if (c == '(')  { return CharaType::LeftParentheses; }
  if (c == ')')  { return CharaType::RightParentheses; }
  if (c == '[')  { return CharaType::LeftBracket; }
  if (c == ']')  { return CharaType::RightBracket; }
  if (c == '{')  { return CharaType::LeftBrace; }
  if (c == '}')  { return CharaType::RightBrace; }
  if (c == '!')  { return CharaType::Not; }
  if (c == '#')  { return CharaType::Pound; }
  if (c == '?')  { return CharaType::Question; }
  if (c == '"')  { return CharaType::DoubleQuotation; }
  if (c == ':')  { return CharaType::Colon; }
  if (c == ';')  { return CharaType::Semicolon; }
  if (c == '=')  { return CharaType::Equality; }
  if (c == ' ')  { return CharaType::Space; }
  if (c == '\\') { return CharaType::Slash; }
  if (c == '\'') { return CharaType::Quotation; }
  if (c == '\t') { return CharaType::Space; }
  if (c == '\r') { return CharaType::Space; }
  if (c == '\n') { return CharaType::Space; }
  return CharaType::cUnknown;
}

// LexicalAnalyzer计算指针跳跃、列数修正和句子保存
void LexicalAnalyzer::Jump(int _go) {
  istr builder = "";
  for (int i = PTRnextLetter; i < PTRnextLetter + _go; i++) {
    if (sourceCode[i] != '\n' || sourceCode[i] != '\r') {
      builder += sourceCode[i];
    }
  }
  currentColumn += _go;
  PTRnextLetter += _go;
  if (PTRnextLetter - 1 >= 0 && sourceCode[PTRnextLetter - 1] == SENTENCETERMINATOR) {
    sentenceContainer.push_back(TempStr + ";");
    TempStr = "";
  }
  else {
    TempStr += builder;
  }
}


/********************************************************
Author:      编译原理计应1班12小组
Date:        2014-11-22
Description: SSQL词法分析器
*********************************************************/