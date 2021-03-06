#include "ISyntaxParser.h"

CSTORE_NS_BEGIN

// 前置声明
SyntaxTreeNode* Homura(SyntaxTreeNode*, void*, CFunctionType, SyntaxType, istr);

// ISyntaxParser获取token流方法
TokenStream* ISyntaxParser::GetTokenStream() {
  return istream;
}

// ISyntaxParser设置待处理token流方法
void ISyntaxParser::SetTokenStream(TokenStream* _myProxy, StrVec* _mySVect) {
  istream = _myProxy;
  this->SetSentencePtr(_mySVect);
}

// ISyntaxParser获取sentence指针
StrVec* ISyntaxParser::GetSectencePtr() {
  return this->isentence;
}

// ISyntaxParser设置sentence向量指针
void ISyntaxParser::SetSentencePtr(StrVec* _myProxy) {
  this->isentence = _myProxy;
}

// LL1SyntaxParser默认构造器
LL1SyntaxParser::LL1SyntaxParser()
  :DBObject("LL1SyntaxParser", this) {
  this->iPTRnextToken = this->iPTRnextSectence = 0;
  this->iMap = new LL1SyntaxParserMap(LL1PARSERMAPROW, LL1PARSERMAPCOL);
  this->InitMap();
}

// LL1SyntaxParser带token流构造函数
LL1SyntaxParser::LL1SyntaxParser(TokenStream *_myProxy, StrVec* _mySVect)
  :DBObject("LL1SyntaxParser", this) {
  this->SetTokenStream(_myProxy, _mySVect);
  this->iPTRnextToken = this->iPTRnextSectence = 0;
  this->iMap = new LL1SyntaxParserMap(LL1PARSERMAPROW, LL1PARSERMAPCOL);
  this->InitMap();
}

// LL1SyntaxParser复位函数
void LL1SyntaxParser::Reset() {
  // 变量初期化
  this->iPTRnextToken = 0;
  //this->iPTRnextSectence = 0;
  while (!this->iParseStack.empty()) {
    this->iParseStack.pop();
  }
  // 放置起始节点
  iParseStack.push(SyntaxType::tail_startEndLeave);
  iParseStack.push(SyntaxType::case_ssql_stmt);
  // 如若token流是空白的直接放一个处理结束标记
  if (this->GetTokenStream()->Length() == 0) {
    Token* ccToken = new Token();
    ccToken->indexOfCode = 0;
    ccToken->aColumn = 0;
    ccToken->aLine = 0;
    ccToken->length = 1;
    ccToken->detail = '#';
    ccToken->errorCode = 0;
    ccToken->aType = DBTokenType::token_startEnd;
    this->GetTokenStream()->Add(ccToken);
  }
  // 否则在追后追加一个处理结束标记的token
  else {
    Token* lastToken = this->GetTokenStream()->_tokenContainer[this->GetTokenStream()->Length() - 1];
    Token* ccToken = new Token();
    ccToken->indexOfCode = lastToken->indexOfCode + lastToken->length + 1;
    ccToken->aColumn = lastToken->aColumn + lastToken->length + 1;
    ccToken->aLine = lastToken->aLine;
    ccToken->length = 1;
    ccToken->detail = '#';
    ccToken->errorCode = 0;
    ccToken->aType = DBTokenType::token_startEnd;
    this->GetTokenStream()->Add(ccToken);
  }
}

// LL1SyntaxParser递增指针
void LL1SyntaxParser::Dash() {
  this->iPTRnextSectence++;
}

// LL1SyntaxParser处理CSTORE特殊保留字
bool LL1SyntaxParser::CSTOREQL(Token* xtoken, SyntaxTreeNode*& curRoot) {
  curRoot->nodeType = CFunctionType::umi_cstore;
  switch (xtoken->aType) {
  case DBTokenType::token_load:
    curRoot->nodeSyntaxType = SyntaxType::cstore_load;
    // require identifier
    if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
      Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
      if (tableNameToken->aType == DBTokenType::token_iden) {
        curRoot->nodeValue = tableNameToken->detail;
      }
      else {
        PILEPRINTLN("Load command should follow with a file *.tbl");
        curRoot = nullptr;
        return true;
      }
      this->iPTRnextToken++;
      curRoot->nodeName = "cstore_load";
      return true;
    }
    else {
      PILEPRINTLN("Load command should follow with a file *.tbl");
      curRoot = nullptr;
      return true;
    }
    break;
  case DBTokenType::token_retrieve:
    curRoot->nodeSyntaxType = SyntaxType::cstore_retrieve;
    // require identifier
    if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
      Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
      if (tableNameToken->aType == DBTokenType::token_iden) {
        curRoot->nodeValue = tableNameToken->detail;
      }
      else {
        TRACE("Retrieve command should follow with a table name");
        curRoot = nullptr;
        return true;
      }
      if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
        Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
        if (tableNameToken->aType == DBTokenType::number) {
          curRoot->nodeValue += "@" + tableNameToken->detail;
        }
        else {
          TRACE("Retrieve command should end with a primary key value");
          curRoot = nullptr;
          return true;
        }
        this->iPTRnextToken++;
        curRoot->nodeName = "cstore_retrieve";
        return true;
      }
      else {
        TRACE("Retrieve command should end with a primary key value");
        curRoot = nullptr;
        return true;
      }
    }
    else {
      TRACE("Retrieve command should follow with a table name");
      curRoot = nullptr;
      return true;
    }
    break;
  case DBTokenType::token_compress:
    curRoot->nodeSyntaxType = SyntaxType::cstore_compress;
    // require identifier
    if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
      Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
      if (tableNameToken->aType == DBTokenType::token_iden) {
        curRoot->nodeValue = tableNameToken->detail;
      }
      else {
        TRACE("Compress command should follow with tablename");
        curRoot = nullptr;
        return true;
      }
      if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
        Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
        if (tableNameToken->aType == DBTokenType::token_iden) {
          curRoot->nodeValue += "@" + tableNameToken->detail;
        }
        else {
          TRACE("Compress command should end with a column name");
          curRoot = nullptr;
          return true;
        }
        this->iPTRnextToken++;
        curRoot->nodeName = "cstore_compress";
        return true;
      }
      else {
        TRACE("Compress command should end with a column name");
        curRoot = nullptr;
        return true;
      }
    }
    else {
      TRACE("Compress command should follow with tablename");
      curRoot = nullptr;
      return true;
    }
    return true;
  case DBTokenType::token_join:
    curRoot->nodeSyntaxType = SyntaxType::cstore_join;
    // require identifier
    if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
      Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
      if (tableNameToken->aType == DBTokenType::token_iden) {
        curRoot->nodeValue = tableNameToken->detail;
      }
      else {
        TRACE("Join command should follow with a table name");
        curRoot = nullptr;
        return true;
      }
      if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
        Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
        if (tableNameToken->aType == DBTokenType::token_iden) {
          curRoot->nodeValue += "@" + tableNameToken->detail;
        }
        else {
          TRACE("Join command should end with a table name");
          curRoot = nullptr;
          return true;
        }
        this->iPTRnextToken++;
        curRoot->nodeName = "cstore_join";
        return true;
      }
      else {
        TRACE("Join command should end with a table name");
        curRoot = nullptr;
        return true;
      }
    }
    else {
      TRACE("Join command should follow with a table name");
      curRoot = nullptr;
      return true;
    }
    break;
  case DBTokenType::token_count:
    curRoot->nodeSyntaxType = SyntaxType::cstore_count;
    // require identifier
    if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
      Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
      if (tableNameToken->aType == DBTokenType::token_iden) {
        curRoot->nodeValue = tableNameToken->detail;
      }
      else {
        TRACE("Count command should follow with a table name");
        curRoot = nullptr;
        return true;
      }
      if (this->GetTokenStream()->_tokenContainer.size() > this->iPTRnextToken + 1) {
        Token* tableNameToken = this->GetTokenStream()->_tokenContainer[++this->iPTRnextToken];
        if (tableNameToken->aType == DBTokenType::token_iden) {
          curRoot->nodeValue += "@" + tableNameToken->detail;
        }
        else {
          TRACE("Count command should end with a column name");
          curRoot = nullptr;
          return true;
        }
        this->iPTRnextToken++;
        curRoot->nodeName = "cstore_count";
        return true;
      }
      else {
        TRACE("Count command should end with a column name");
        curRoot = nullptr;
        return true;
      }
    }
    else {
      TRACE("Count command should follow with a table name");
      curRoot = nullptr;
      return true;
    }
    break;
  default:
    return false;
  }
  return true;
}

// LL1SyntaxParser启动器
SyntaxTreeNode* LL1SyntaxParser::Parse() {
  // 初期化
  this->Reset();
  // 原始节点
  SyntaxTreeNode* parsePointer = new SyntaxTreeNode();
  SyntaxTreeNode* currentPtr = parsePointer;
  // 处理CStore特殊保留字
  bool spottedCQL = this->CSTOREQL(this->GetTokenStream()->_tokenContainer[iPTRnextToken], parsePointer);
  // 命中CStore关键字
  if (spottedCQL == true) {
    // 语法错误时
    if (parsePointer == nullptr) {
      return nullptr;
    }
    return parsePointer;
  }
  // 利用栈去递归下降
  while (iPTRnextToken < this->GetTokenStream()->Length()) {
    // 查预测表，获得产生式处理函数
    SyntaxType nodeType = this->iParseStack.top();
    Token* iToken = this->GetTokenStream()->_tokenContainer[iPTRnextToken];
    DBTokenType tokenType = iToken->aType;
    CandidateFunction* func = this->iMap->GetCFunction(nodeType, tokenType, Homura);
    // 语法错误时
    if (func->GetType() == CFunctionType::umi_errorEnd) {
      // 调用错误处理
      this->iException();
      // 跳跃掉这个语句
      this->Dash();
      return nullptr;
    }
    // 如果处于非终结符，就设置她的候选式
    if (currentPtr != nullptr) {
      currentPtr->candidateFunction = func;
    }
    // 调用产生式，下降
    if (func != nullptr) {
      if (currentPtr != nullptr) {
        currentPtr = currentPtr->candidateFunction->Call(currentPtr, this, nodeType, iToken->detail);
      }
      else {
        currentPtr = Homura(currentPtr, this, func->GetType(), nodeType, iToken->detail);
      }
    }
    // 没有对应的候选式时
    else {
      if (currentPtr != nullptr) {
        currentPtr->errorCode = 1;
      }
      break;
    }
  }
  // 更新信息
  parsePointer->mappedLength = this->GetTokenStream()->Length();
  // 跳跃指针
  this->Dash();
  return parsePointer;
}

// LL1SyntaxParser下一节点
SyntaxTreeNode* LL1SyntaxParser::NextNode(SyntaxTreeNode* _res, LL1SyntaxParser* _parser) const {
  // 已经没有需要扩展的节点了就返回空
  if (_res == nullptr) {
    return nullptr;
  }
  // 否则取其父节点来取得其兄弟节点
  SyntaxTreeNode* parent = _res->parent;
  // 若无父母节点就已经到递归退回到最上层了
  if (parent == nullptr) {
    return nullptr;
  }
  int i = 0;
  // 遍历寻找自己在姐妹中的排位
  for (; i < static_cast<int>(parent->children.size()); i++) {
    if (parent->children[i] == _res) {
      break;
    }
  }
  // 跳过自己，选最大的妹节点作为下一个展开节点
  if (i + 1 >= static_cast<int>(parent->children.size())) {
    return parent->children[i + 1];
  }
  // 已经没有比自己同辈且小的节点，就递归去找父母辈的兄弟姐妹
  if (i + 1 == parent->children.size()) {
    parent->mappedLength = _parser->iPTRnextToken - parent->mappedBegin;
  }
  return NextNode(parent, _parser);
}

// LL1SyntaxParser错误处理
void LL1SyntaxParser::iException() {
  TRACE("# Parser Exception Spotted.");
  PILEPRINTLN("# Syntax Error: At ("
    << this->GetTokenStream()->_tokenContainer[iPTRnextToken]->aLine << ", "
    << this->GetTokenStream()->_tokenContainer[iPTRnextToken]->aColumn << ")"
    << ", which Token detail: " << this->GetTokenStream()->_tokenContainer[iPTRnextToken]->detail << NEWLINE
    << "# In sentence:  " << this->GetSectencePtr()->at(iPTRnextSectence) << NEWLINE << "#");
  for (int i = 0; i < this->GetTokenStream()->_tokenContainer[iPTRnextToken]->aColumn + 14; i++) {
    PILEPRINT(" ");
  }
  PILEPRINTLN("^" << NEWLINE
    << "# Cannot map any syntax type, this sentence will be ignored."
    << NEWLINE);
}

// LL1SyntaxParser初始化预测分析表
void LL1SyntaxParser::InitMap() {
  this->InitLinkerVector();
  this->InitMapProperties();
  this->InitCellular();
}

// LL1SyntaxParser取子向量
SyntaxVector LL1SyntaxParser::GetVector(CFunctionType _type) const {
  return this->iDict[_type];
}

// LL1SyntaxParser取栈指针
SyntaxStack* LL1SyntaxParser::GetStack() {
  return &(this->iParseStack);
}

// LL1SyntaxParser初始化链接向量
void LL1SyntaxParser::InitLinkerVector() {
  this->iDict.resize(192);
  // <ssql_stmt> -> <create_stmt>
  this->iDict[CFunctionType::deri___ssql_stmt__create_stmt_1].push_back(SyntaxType::case_create_stmt);
  // <ssql_stmt> -> <insert_stmt>
  this->iDict[CFunctionType::deri___ssql_stmt__insert_stmt_2].push_back(SyntaxType::case_insert_stmt);
  // <ssql_stmt> -> <delete_stmt>
  this->iDict[CFunctionType::deri___ssql_stmt__delete_stmt_3].push_back(SyntaxType::case_delete_stmt);
  // <ssql_stmt> -> <query_stmt>
  this->iDict[CFunctionType::deri___ssql_stmt__query_stmt_4].push_back(SyntaxType::case_query_stmt);
  // <create_stmt> -> "create" "table" id "(" <decl_list> ")" ";"
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::tail_createLeave);
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::tail_tableLeave);
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::tail_leftParentheses_Leave);
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::case_decl_list);
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::tail_rightParentheses_Leave);
  this->iDict[CFunctionType::deri___create_stmt__decl_list_5].push_back(SyntaxType::tail_semicolon_Leave);
  // <decl_list> -> <decl> <decl_listpi>
  this->iDict[CFunctionType::deri___decl_list__decl__decl_listpi_6].push_back(SyntaxType::case_decl);
  this->iDict[CFunctionType::deri___decl_list__decl__decl_listpi_6].push_back(SyntaxType::case_decl_listpi);
  // <decl_listpi> -> "," <decl> <decl_listpi>
  this->iDict[CFunctionType::deri___decl_listpi__decl__decl_listpi_67].push_back(SyntaxType::tail_comma_Leave);
  this->iDict[CFunctionType::deri___decl_listpi__decl__decl_listpi_67].push_back(SyntaxType::case_decl);
  this->iDict[CFunctionType::deri___decl_listpi__decl__decl_listpi_67].push_back(SyntaxType::case_decl_listpi);
  // <decl_listpi> -> epsilon
  this->iDict[CFunctionType::deri___decl_listpi__epsilon_7].push_back(SyntaxType::epsilonLeave);
  // <decl> -> id "int" <default_spec>
  // <decl> -> id decltype <default_spec>
  this->iDict[CFunctionType::deri___decl__decltype__default_spec_8].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___decl__decltype__default_spec_8].push_back(SyntaxType::case_decltype);
  this->iDict[CFunctionType::deri___decl__decltype__default_spec_8].push_back(SyntaxType::case_default_spec);
  // <decltype> -> "int";
  this->iDict[CFunctionType::deri___decltype__intleave].push_back(SyntaxType::tail_intLeave);
  // <decltype> -> "double";
  this->iDict[CFunctionType::deri___decltype__doubleleave].push_back(SyntaxType::tail_doubleLeave);
  // <decl> -> "primary" "key" "(" <column_list> ")"
  this->iDict[CFunctionType::deri___decl__column_list_9].push_back(SyntaxType::tail_primaryLeave);
  this->iDict[CFunctionType::deri___decl__column_list_9].push_back(SyntaxType::tail_keyLeave);
  this->iDict[CFunctionType::deri___decl__column_list_9].push_back(SyntaxType::tail_leftParentheses_Leave);
  this->iDict[CFunctionType::deri___decl__column_list_9].push_back(SyntaxType::case_column_list);
  this->iDict[CFunctionType::deri___decl__column_list_9].push_back(SyntaxType::tail_rightParentheses_Leave);
  // <default_spec> -> "default" "=" sexpr
  this->iDict[CFunctionType::deri___default_spec__sexpr_10].push_back(SyntaxType::tail_defaultLeave);
  this->iDict[CFunctionType::deri___default_spec__sexpr_10].push_back(SyntaxType::tail_equality_Leave);
  this->iDict[CFunctionType::deri___default_spec__sexpr_10].push_back(SyntaxType::case_sexpr);
  // <default_spec> -> epsilon
  this->iDict[CFunctionType::deri___default_spec__epsilon_11].push_back(SyntaxType::epsilonLeave);
  // <sexpr> -> <smulti> <sexpr_pi>
  this->iDict[CFunctionType::deri___sexpr__smulti__sexpr_pi_12].push_back(SyntaxType::case_smulti);
  this->iDict[CFunctionType::deri___sexpr__smulti__sexpr_pi_12].push_back(SyntaxType::case_sexpr_pi);
  // <splus> -> "+" <smulti>
  this->iDict[CFunctionType::deri___splus__plus_smulti_14].push_back(SyntaxType::tail_plus_Leave);
  this->iDict[CFunctionType::deri___splus__plus_smulti_14].push_back(SyntaxType::case_smulti);
  // <splus> -> "-" <smulti>
  this->iDict[CFunctionType::deri___splus__minus_smulti_15].push_back(SyntaxType::tail_minus_Leave);
  this->iDict[CFunctionType::deri___splus__minus_smulti_15].push_back(SyntaxType::case_smulti);
  // <sexpr_pi> -> <splus> <sexpr_pi>
  this->iDict[CFunctionType::deri___sexpr_pi__splus__sexpr_pi_70].push_back(SyntaxType::case_splus);
  this->iDict[CFunctionType::deri___sexpr_pi__splus__sexpr_pi_70].push_back(SyntaxType::case_sexpr_pi);
  // <sexpr_pi> -> epsilon
  this->iDict[CFunctionType::deri___sexpr_pi__epsilon_71].push_back(SyntaxType::epsilonLeave);
  // <smulti> -> <sunit> <smultiOpt>
  this->iDict[CFunctionType::deri___smulti__sunit__smultiOpt_17].push_back(SyntaxType::case_sunit);
  this->iDict[CFunctionType::deri___smulti__sunit__smultiOpt_17].push_back(SyntaxType::case_smultiOpt);
  // <smultiOpt> -> "*" <sunit>
  this->iDict[CFunctionType::deri___smultiOpt__multi_sunit__smultiOpt_18].push_back(SyntaxType::tail_multiply_Leave);
  this->iDict[CFunctionType::deri___smultiOpt__multi_sunit__smultiOpt_18].push_back(SyntaxType::case_sunit);
  this->iDict[CFunctionType::deri___smultiOpt__multi_sunit__smultiOpt_18].push_back(SyntaxType::case_smultiOpt);
  // <smultiOpt> -> "/" <sunit>
  this->iDict[CFunctionType::deri___smultiOpt__div_sunit__smultiOpt_19].push_back(SyntaxType::tail_divide_Leave);
  this->iDict[CFunctionType::deri___smultiOpt__div_sunit__smultiOpt_19].push_back(SyntaxType::case_sunit);
  this->iDict[CFunctionType::deri___smultiOpt__div_sunit__smultiOpt_19].push_back(SyntaxType::case_smultiOpt);
  // <smultiOpt> -> epsilon
  this->iDict[CFunctionType::deri___smultiOpt__epsilon_20].push_back(SyntaxType::epsilonLeave);
  // <sunit> -> number
  this->iDict[CFunctionType::deri___sunit__number_21].push_back(SyntaxType::numberLeave);
  // <sunit> -> "-" <sunit>
  this->iDict[CFunctionType::deri___sunit__minus_sunit_22].push_back(SyntaxType::tail_minus_Leave);
  this->iDict[CFunctionType::deri___sunit__minus_sunit_22].push_back(SyntaxType::case_sunit);
  // <sunit> -> "+" <sunit>
  this->iDict[CFunctionType::deri___sunit__plus_sunit_23].push_back(SyntaxType::tail_plus_Leave);
  this->iDict[CFunctionType::deri___sunit__plus_sunit_23].push_back(SyntaxType::case_sunit);
  // <sunit> -> "(" <sexpr> ")"
  this->iDict[CFunctionType::deri___sunit__brucket_sexpr_24].push_back(SyntaxType::tail_leftParentheses_Leave);
  this->iDict[CFunctionType::deri___sunit__brucket_sexpr_24].push_back(SyntaxType::case_sexpr);
  this->iDict[CFunctionType::deri___sunit__brucket_sexpr_24].push_back(SyntaxType::tail_rightParentheses_Leave);
  // <insert_stmt> -> "insert" "into" id "(" <column_list> ")" "values" "(" <value_list> ")" ";"
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_insertLeave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_intoLeave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_leftParentheses_Leave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::case_column_list);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_rightParentheses_Leave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_valuesLeave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_leftParentheses_Leave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::case_value_list);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_rightParentheses_Leave);
  this->iDict[CFunctionType::deri___insert_stmt__column_list__value_list_28].push_back(SyntaxType::tail_semicolon_Leave);
  // <value_list> -> <sexpr> <value_listpi>
  this->iDict[CFunctionType::deri___value_list__sexpr__value_listpi_29].push_back(SyntaxType::case_sexpr);
  this->iDict[CFunctionType::deri___value_list__sexpr__value_listpi_29].push_back(SyntaxType::case_value_listpi);
  // <value_listpi> -> "," <sexpr> <value_listpi>
  this->iDict[CFunctionType::deri___value_listpi__sexpr__value_listpi_30].push_back(SyntaxType::tail_comma_Leave);
  this->iDict[CFunctionType::deri___value_listpi__sexpr__value_listpi_30].push_back(SyntaxType::case_sexpr);
  this->iDict[CFunctionType::deri___value_listpi__sexpr__value_listpi_30].push_back(SyntaxType::case_value_listpi);
  // <value_listpi> -> epsilon
  this->iDict[CFunctionType::deri___value_listpi__epsilon_31].push_back(SyntaxType::epsilonLeave);
  // <delete_stmt> -> "delete" "from" id <where_clause> ";"
  this->iDict[CFunctionType::deri___delete_stmt__where_clause_32].push_back(SyntaxType::tail_deleteLeave);
  this->iDict[CFunctionType::deri___delete_stmt__where_clause_32].push_back(SyntaxType::tail_fromLeave);
  this->iDict[CFunctionType::deri___delete_stmt__where_clause_32].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___delete_stmt__where_clause_32].push_back(SyntaxType::case_where_clause);
  this->iDict[CFunctionType::deri___delete_stmt__where_clause_32].push_back(SyntaxType::tail_semicolon_Leave);
  // <where_clause> -> "where" <disjunct>
  this->iDict[CFunctionType::deri___where_clause__disjunct_33].push_back(SyntaxType::tail_whereLeave);
  this->iDict[CFunctionType::deri___where_clause__disjunct_33].push_back(SyntaxType::case_disjunct);
  // <where_clause> -> epsilon
  this->iDict[CFunctionType::deri___where_clause__epsilon_34].push_back(SyntaxType::epsilonLeave);
  // <disjunct> -> <conjunct> <disjunct_pi>
  this->iDict[CFunctionType::deri___disjunct__conjunct__disjunct_pi_35].push_back(SyntaxType::case_conjunct);
  this->iDict[CFunctionType::deri___disjunct__conjunct__disjunct_pi_35].push_back(SyntaxType::case_disjunct_pi);
  // <disjunct_pi> -> "||" <conjunct> <disjunct_pi>
  this->iDict[CFunctionType::deri___disjunct_pi__conjunct__disjunct_pi_36].push_back(SyntaxType::tail_or_Or_Leave);
  this->iDict[CFunctionType::deri___disjunct_pi__conjunct__disjunct_pi_36].push_back(SyntaxType::case_conjunct);
  this->iDict[CFunctionType::deri___disjunct_pi__conjunct__disjunct_pi_36].push_back(SyntaxType::case_disjunct_pi);
  // <disjunct_pi> -> epsilon
  this->iDict[CFunctionType::deri___disjunct_pi__epsilon_37].push_back(SyntaxType::epsilonLeave);
  // <conjunct> -> <bool> <conjunct_pi>
  this->iDict[CFunctionType::deri___conjunct__bool__conjunct_pi_38].push_back(SyntaxType::case_bool);
  this->iDict[CFunctionType::deri___conjunct__bool__conjunct_pi_38].push_back(SyntaxType::case_conjunct_pi);
  // <conjunct_pi> -> "&&" <bool> <conjunct_pi>
  this->iDict[CFunctionType::deri___conjunct_pi__bool__conjunct_pi_39].push_back(SyntaxType::tail_and_And_Leave);
  this->iDict[CFunctionType::deri___conjunct_pi__bool__conjunct_pi_39].push_back(SyntaxType::case_bool);
  this->iDict[CFunctionType::deri___conjunct_pi__bool__conjunct_pi_39].push_back(SyntaxType::case_conjunct_pi);
  // <conjunct_pi> -> epsilon
  this->iDict[CFunctionType::deri___conjunct_pi__epsilon_40].push_back(SyntaxType::epsilonLeave);
  // <bool> -> "!" <bool>
  this->iDict[CFunctionType::deri___bool__not_bool_42].push_back(SyntaxType::tail_not_Leave);
  this->iDict[CFunctionType::deri___bool__not_bool_42].push_back(SyntaxType::case_bool);
  // <bool> -> <comp>
  this->iDict[CFunctionType::deri___bool__comp_43].push_back(SyntaxType::case_comp);
  // <comp> -> <wexpr> <rop> <wexpr>
  this->iDict[CFunctionType::deri___comp__wexpr__rop__wexpr_44].push_back(SyntaxType::case_wexpr);
  this->iDict[CFunctionType::deri___comp__wexpr__rop__wexpr_44].push_back(SyntaxType::case_rop);
  this->iDict[CFunctionType::deri___comp__wexpr__rop__wexpr_44].push_back(SyntaxType::case_wexpr);
  // <rop> -> "<>"
  this->iDict[CFunctionType::deri___rop__lessgreater_58].push_back(SyntaxType::tail_lessThan_GreaterThan_Leave);
  // <rop> -> "=="
  this->iDict[CFunctionType::deri___rop__equalequal_59].push_back(SyntaxType::tail_equality_Equality_Leave);
  // <rop> -> ">"
  this->iDict[CFunctionType::deri___rop__greater_60].push_back(SyntaxType::tail_greaterThan_Leave);
  // <rop> -> "<"
  this->iDict[CFunctionType::deri___rop__less_61].push_back(SyntaxType::tail_lessThan_Leave);
  // <rop> -> ">="
  this->iDict[CFunctionType::deri___rop__greaterequal_62].push_back(SyntaxType::tail_greaterThan_Equality_Leave);
  // <rop> -> "<="
  this->iDict[CFunctionType::deri___rop__lessequal_63].push_back(SyntaxType::tail_lessThan_Equality_Leave);
  // <rop> -> epsilon
  this->iDict[CFunctionType::deri___rop__epsilon_80].push_back(SyntaxType::epsilonLeave);
  // <wexpr> -> <wmulti> <wexpr_pi>
  this->iDict[CFunctionType::deri___wexpr__wmulti__wexpr_pi_45].push_back(SyntaxType::case_wmulti);
  this->iDict[CFunctionType::deri___wexpr__wmulti__wexpr_pi_45].push_back(SyntaxType::case_wexpr_pi);
  // <wexpr> -> epsilon
  this->iDict[CFunctionType::deri___wexpr__epsilon_81].push_back(SyntaxType::epsilonLeave);
  // <wplus> -> "+" <wmulti>
  this->iDict[CFunctionType::deri___wplus__plus_wmulti_46].push_back(SyntaxType::tail_plus_Leave);
  this->iDict[CFunctionType::deri___wplus__plus_wmulti_46].push_back(SyntaxType::case_wmulti);
  // <wplus> -> "-" <wmulti>
  this->iDict[CFunctionType::deri___wplus__minus_wmulti_47].push_back(SyntaxType::tail_minus_Leave);
  this->iDict[CFunctionType::deri___wplus__minus_wmulti_47].push_back(SyntaxType::case_wmulti);
  // <wexpr_pi> -> <wplus> <wexpr_pi>
  this->iDict[CFunctionType::deri___wexpr_pi__wplus__wexpr_pi_72].push_back(SyntaxType::case_wplus);
  this->iDict[CFunctionType::deri___wexpr_pi__wplus__wexpr_pi_72].push_back(SyntaxType::case_wexpr_pi);
  // <wexpr_pi> -> epsilon
  this->iDict[CFunctionType::deri___wexpr_pi__epsilon_73].push_back(SyntaxType::epsilonLeave);
  // <wmulti> -> <wunit> <wmultiOpt>
  this->iDict[CFunctionType::deri___wmulti__wunit__wmultiOpt_49].push_back(SyntaxType::case_wunit);
  this->iDict[CFunctionType::deri___wmulti__wunit__wmultiOpt_49].push_back(SyntaxType::case_wmultiOpt);
  // <wmultiOpt> -> "*" <wunit>
  this->iDict[CFunctionType::deri___wmultiOpt__multi_wunit__wmultiOpt_50].push_back(SyntaxType::tail_multiply_Leave);
  this->iDict[CFunctionType::deri___wmultiOpt__multi_wunit__wmultiOpt_50].push_back(SyntaxType::case_wunit);
  this->iDict[CFunctionType::deri___wmultiOpt__multi_wunit__wmultiOpt_50].push_back(SyntaxType::case_wmultiOpt);
  // <wmultiOpt> -> "/" <wunit>
  this->iDict[CFunctionType::deri___wmultiOpt__div_wunit__wmultiOpt_51].push_back(SyntaxType::tail_divide_Leave);
  this->iDict[CFunctionType::deri___wmultiOpt__div_wunit__wmultiOpt_51].push_back(SyntaxType::case_wunit);
  this->iDict[CFunctionType::deri___wmultiOpt__div_wunit__wmultiOpt_51].push_back(SyntaxType::case_wmultiOpt);
  // <wmultiOpt> -> epsilon
  this->iDict[CFunctionType::deri___wmultiOpt__epsilon_52].push_back(SyntaxType::epsilonLeave);
  // <wunit> -> number
  this->iDict[CFunctionType::deri___wunit__number_53].push_back(SyntaxType::numberLeave);
  // <wunit> -> iden
  this->iDict[CFunctionType::deri___wunit__iden_54].push_back(SyntaxType::tail_idenLeave);
  // <wunit> -> "-" <wunit>
  this->iDict[CFunctionType::deri___wunit__minus_wunit_55].push_back(SyntaxType::tail_minus_Leave);
  this->iDict[CFunctionType::deri___wunit__minus_wunit_55].push_back(SyntaxType::case_wunit);
  // <wunit> -> "+" <wunit>
  this->iDict[CFunctionType::deri___wunit__plus_wunit_56].push_back(SyntaxType::tail_plus_Leave);
  this->iDict[CFunctionType::deri___wunit__plus_wunit_56].push_back(SyntaxType::case_wunit);
  // <wunit> -> "(" <disjunct> ")"
  this->iDict[CFunctionType::deri___wunit__brucket_disjunct_57].push_back(SyntaxType::tail_leftParentheses_Leave);
  this->iDict[CFunctionType::deri___wunit__brucket_disjunct_57].push_back(SyntaxType::case_disjunct);
  this->iDict[CFunctionType::deri___wunit__brucket_disjunct_57].push_back(SyntaxType::tail_rightParentheses_Leave);
  // <query_stmt> -> "select" <select_list> "from" id <where_clause> ";"
  this->iDict[CFunctionType::deri___query_stmt__select_list__where_clause_64].push_back(SyntaxType::tail_selectLeave);
  this->iDict[CFunctionType::deri___query_stmt__select_list__where_clause_64].push_back(SyntaxType::case_select_list);
  this->iDict[CFunctionType::deri___query_stmt__select_list__where_clause_64].push_back(SyntaxType::tail_fromLeave);
  this->iDict[CFunctionType::deri___query_stmt__select_list__where_clause_64].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___query_stmt__select_list__where_clause_64].push_back(SyntaxType::case_where_clause);
  this->iDict[CFunctionType::deri___query_stmt__select_list__where_clause_64].push_back(SyntaxType::tail_semicolon_Leave);
  // <select_list> -> <column_list>
  this->iDict[CFunctionType::deri___select_list__column_list_65].push_back(SyntaxType::case_column_list);
  // <select_list> -> "*"
  this->iDict[CFunctionType::deri___select_list__star_66].push_back(SyntaxType::tail_multiply_Leave);
  // <column_list> -> id <column_pi>
  this->iDict[CFunctionType::deri___column_list__column_pi_25].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___column_list__column_pi_25].push_back(SyntaxType::case_column_pi);
  // <column_pi> -> "," id <column_pi>
  this->iDict[CFunctionType::deri___column_pi__comma_column_pi_26].push_back(SyntaxType::tail_comma_Leave);
  this->iDict[CFunctionType::deri___column_pi__comma_column_pi_26].push_back(SyntaxType::tail_idenLeave);
  this->iDict[CFunctionType::deri___column_pi__comma_column_pi_26].push_back(SyntaxType::case_column_pi);
  // <column_pi> -> epsilon
  this->iDict[CFunctionType::deri___column_pi__epsilon_27].push_back(SyntaxType::epsilonLeave);
}

// LL1SyntaxParser初始化分析表行列
void LL1SyntaxParser::InitMapProperties() const {
  // 设置行属性：非终结符
  iMap->SetRow(0, SyntaxType::case_ssql_stmt);
  iMap->SetRow(1, SyntaxType::case_create_stmt);
  iMap->SetRow(2, SyntaxType::case_decl_list);
  iMap->SetRow(3, SyntaxType::case_decl_listpi);
  iMap->SetRow(4, SyntaxType::case_decl);
  iMap->SetRow(5, SyntaxType::case_default_spec);
  iMap->SetRow(6, SyntaxType::case_sexpr);
  iMap->SetRow(7, SyntaxType::case_sexpr_pi);
  iMap->SetRow(8, SyntaxType::case_splus);
  iMap->SetRow(9, SyntaxType::case_smulti);
  iMap->SetRow(10, SyntaxType::case_smultiOpt);
  iMap->SetRow(11, SyntaxType::case_sunit);
  iMap->SetRow(12, SyntaxType::case_insert_stmt);
  iMap->SetRow(13, SyntaxType::case_value_list);
  iMap->SetRow(14, SyntaxType::case_value_listpi);
  iMap->SetRow(15, SyntaxType::case_delete_stmt);
  iMap->SetRow(16, SyntaxType::case_where_clause);
  iMap->SetRow(17, SyntaxType::case_disjunct);
  iMap->SetRow(18, SyntaxType::case_disjunct_pi);
  iMap->SetRow(19, SyntaxType::case_conjunct);
  iMap->SetRow(20, SyntaxType::case_conjunct_pi);
  iMap->SetRow(21, SyntaxType::case_bool);
  iMap->SetRow(22, SyntaxType::case_comp);
  iMap->SetRow(23, SyntaxType::case_rop);
  iMap->SetRow(24, SyntaxType::case_wexpr);
  iMap->SetRow(25, SyntaxType::case_wexpr_pi);
  iMap->SetRow(26, SyntaxType::case_wplus);
  iMap->SetRow(27, SyntaxType::case_wmulti);
  iMap->SetRow(28, SyntaxType::case_wmultiOpt);
  iMap->SetRow(29, SyntaxType::case_wunit);
  iMap->SetRow(30, SyntaxType::case_query_stmt);
  iMap->SetRow(31, SyntaxType::case_select_list);
  iMap->SetRow(32, SyntaxType::case_column_list);
  iMap->SetRow(33, SyntaxType::case_column_pi);
  iMap->SetRow(70, SyntaxType::case_decltype);
  // 设置行属性：终结符
  iMap->SetRow(34, SyntaxType::tail_createLeave);
  iMap->SetRow(35, SyntaxType::tail_tableLeave);
  iMap->SetRow(36, SyntaxType::tail_idenLeave);
  iMap->SetRow(37, SyntaxType::tail_leftParentheses_Leave);
  iMap->SetRow(38, SyntaxType::tail_rightParentheses_Leave);
  iMap->SetRow(39, SyntaxType::tail_semicolon_Leave);
  iMap->SetRow(40, SyntaxType::tail_comma_Leave);
  iMap->SetRow(41, SyntaxType::epsilonLeave);
  iMap->SetRow(42, SyntaxType::tail_intLeave);
  iMap->SetRow(43, SyntaxType::tail_primaryLeave);
  iMap->SetRow(44, SyntaxType::tail_keyLeave);
  iMap->SetRow(45, SyntaxType::tail_defaultLeave);
  iMap->SetRow(46, SyntaxType::tail_equality_Leave);
  iMap->SetRow(47, SyntaxType::tail_plus_Leave);
  iMap->SetRow(48, SyntaxType::tail_minus_Leave);
  iMap->SetRow(49, SyntaxType::tail_multiply_Leave);
  iMap->SetRow(50, SyntaxType::tail_divide_Leave);
  iMap->SetRow(51, SyntaxType::numberLeave);
  iMap->SetRow(52, SyntaxType::tail_insertLeave);
  iMap->SetRow(53, SyntaxType::tail_intoLeave);
  iMap->SetRow(54, SyntaxType::tail_valuesLeave);
  iMap->SetRow(55, SyntaxType::tail_deleteLeave);
  iMap->SetRow(56, SyntaxType::tail_fromLeave);
  iMap->SetRow(57, SyntaxType::tail_whereLeave);
  iMap->SetRow(58, SyntaxType::tail_or_Or_Leave);
  iMap->SetRow(59, SyntaxType::tail_and_And_Leave);
  iMap->SetRow(60, SyntaxType::tail_not_Leave);
  iMap->SetRow(61, SyntaxType::tail_lessThan_GreaterThan_Leave);
  iMap->SetRow(62, SyntaxType::tail_equality_Equality_Leave);
  iMap->SetRow(63, SyntaxType::tail_greaterThan_Leave);
  iMap->SetRow(64, SyntaxType::tail_lessThan_Leave);
  iMap->SetRow(65, SyntaxType::tail_greaterThan_Equality_Leave);
  iMap->SetRow(66, SyntaxType::tail_lessThan_Equality_Leave);
  iMap->SetRow(67, SyntaxType::tail_selectLeave);
  iMap->SetRow(68, SyntaxType::tail_startEndLeave);
  iMap->SetRow(69, SyntaxType::tail_doubleLeave);
  // 设置列属性：向前看的一个token
  iMap->SetCol(0, DBTokenType::token_create);
  iMap->SetCol(1, DBTokenType::token_table);
  iMap->SetCol(2, DBTokenType::token_iden);
  iMap->SetCol(3, DBTokenType::token_LeftParentheses_);
  iMap->SetCol(4, DBTokenType::token_RightParentheses_);
  iMap->SetCol(5, DBTokenType::token_Semicolon_);
  iMap->SetCol(6, DBTokenType::token_Comma_);
  iMap->SetCol(7, DBTokenType::epsilon);
  iMap->SetCol(8, DBTokenType::token_int);
  iMap->SetCol(9, DBTokenType::token_primary);
  iMap->SetCol(10, DBTokenType::token_key);
  iMap->SetCol(11, DBTokenType::token_default);
  iMap->SetCol(12, DBTokenType::token_Equality_);
  iMap->SetCol(13, DBTokenType::token_Plus_);
  iMap->SetCol(14, DBTokenType::token_Minus_);
  iMap->SetCol(15, DBTokenType::token_Multiply_);
  iMap->SetCol(16, DBTokenType::token_Divide_);
  iMap->SetCol(17, DBTokenType::number);
  iMap->SetCol(18, DBTokenType::token_insert);
  iMap->SetCol(19, DBTokenType::token_into);
  iMap->SetCol(20, DBTokenType::token_values);
  iMap->SetCol(21, DBTokenType::token_delete);
  iMap->SetCol(22, DBTokenType::token_from);
  iMap->SetCol(23, DBTokenType::token_where);
  iMap->SetCol(24, DBTokenType::token_Or_Or_);
  iMap->SetCol(25, DBTokenType::token_And_And_);
  iMap->SetCol(26, DBTokenType::token_Not_);
  iMap->SetCol(27, DBTokenType::token_LessThan_GreaterThan_);
  iMap->SetCol(28, DBTokenType::token_Equality_Equality_);
  iMap->SetCol(29, DBTokenType::token_GreaterThan_);
  iMap->SetCol(30, DBTokenType::token_LessThan_);
  iMap->SetCol(31, DBTokenType::token_GreaterThan_Equality_);
  iMap->SetCol(32, DBTokenType::token_LessThan_Equality_);
  iMap->SetCol(33, DBTokenType::token_select);
  iMap->SetCol(34, DBTokenType::token_startEnd);
  iMap->SetCol(35, DBTokenType::token_double);
}

// LL1SyntaxParser初始化文法
void LL1SyntaxParser::InitCellular() const {
  // iProco都指向通用展开式函数Homura
  iHandle iProco = Homura;
  // 错误的情况下，没有考虑短语层次的错误恢复，因此错误处理器都指向NULL
  for (int i = 0; i < LL1PARSERMAPROW; i++) {
    for (int j = 0; j < LL1PARSERMAPCOL; j++) {
      this->iMap->SetCellular(i, j, new CandidateFunction(nullptr, CFunctionType::umi_errorEnd));
    }
  }
  // 流命中： <ssql_stmt> ::= "create"的Token
  // 语句命中： <ssql_stmt> ::= <create_stmt>
  this->iMap->SetCellular(SyntaxType::case_ssql_stmt, DBTokenType::token_create,
    new CandidateFunction(iProco, CFunctionType::deri___ssql_stmt__create_stmt_1));
  // 流命中： <ssql_stmt> ::= "insert"的Token
  // 语句命中： <ssql_stmt> ::= <insert_stmt>
  this->iMap->SetCellular(SyntaxType::case_ssql_stmt, DBTokenType::token_insert,
    new CandidateFunction(iProco, CFunctionType::deri___ssql_stmt__insert_stmt_2));
  // 流命中： <ssql_stmt> ::= "delete"的Token
  // 语句命中： <ssql_stmt> ::= <delete_stmt>
  this->iMap->SetCellular(SyntaxType::case_ssql_stmt, DBTokenType::token_delete,
    new CandidateFunction(iProco, CFunctionType::deri___ssql_stmt__delete_stmt_3));
  // 流命中： <ssql_stmt> ::= "select"的Token
  // 语句命中： <ssql_stmt> ::= <query_stmt>
  this->iMap->SetCellular(SyntaxType::case_ssql_stmt, DBTokenType::token_select,
    new CandidateFunction(iProco, CFunctionType::deri___ssql_stmt__query_stmt_4));
  // 流命中： <create_stmt> ::= "create"的Token
  // 语句命中： <create_stmt> ::= "create" "table" iden "(" <decl_list> ")" ";"
  this->iMap->SetCellular(SyntaxType::case_create_stmt, DBTokenType::token_create,
    new CandidateFunction(iProco, CFunctionType::deri___create_stmt__decl_list_5));
  // 流命中： <decl_list> ::= iden的Token
  // 语句命中： <decl_list> ::= <decl> <decl_listpi>
  this->iMap->SetCellular(SyntaxType::case_decl_list, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___decl_list__decl__decl_listpi_6));
  // 流命中： <decl_list> ::= "primary"的Token
  // 语句命中： <decl_list> ::= <decl> <decl_listpi>
  this->iMap->SetCellular(SyntaxType::case_decl_list, DBTokenType::token_primary,
    new CandidateFunction(iProco, CFunctionType::deri___decl_list__decl__decl_listpi_6));
  // 流命中： <decl_listpi> ::= ")"的Token
  // 语句命中： <decl_listpi> ::= null
  this->iMap->SetCellular(SyntaxType::case_decl_listpi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___decl_listpi__epsilon_7));
  // 流命中： <decl_listpi> ::= ","的Token
  // 语句命中： <decl_listpi> ::= "," <decl> <decl_listpi>
  this->iMap->SetCellular(SyntaxType::case_decl_listpi, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___decl_listpi__decl__decl_listpi_67));
  // 流命中： <decl> ::= iden的Token
  // 语句命中： <decl> ::= iden <decltype> <default_spec>
  this->iMap->SetCellular(SyntaxType::case_decl, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___decl__decltype__default_spec_8));
  // 流命中： <decltype> ::= int的Token
  // 语句命中： <decltype> ::= int
  this->iMap->SetCellular(SyntaxType::case_decltype, DBTokenType::token_int,
    new CandidateFunction(iProco, CFunctionType::deri___decltype__intleave));
  // 流命中： <decltype> ::= double的Token
  // 语句命中： <decltype> ::= double
  this->iMap->SetCellular(SyntaxType::case_decltype, DBTokenType::token_double,
    new CandidateFunction(iProco, CFunctionType::deri___decltype__doubleleave));
  // 流命中： <decl> ::= "primary"的Token
  // 语句命中： <decl> ::= "primary" "key" "(" <column_list> ")"
  this->iMap->SetCellular(SyntaxType::case_decl, DBTokenType::token_primary,
    new CandidateFunction(iProco, CFunctionType::deri___decl__column_list_9));
  // 流命中： <default_spec> ::= ")"的Token
  // 语句命中： <default_spec> ::= null
  this->iMap->SetCellular(SyntaxType::case_default_spec, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___default_spec__epsilon_11));
  // 流命中： <default_spec> ::= ","的Token
  // 语句命中： <default_spec> ::= null
  this->iMap->SetCellular(SyntaxType::case_default_spec, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___default_spec__epsilon_11));
  // 流命中： <default_spec> ::= "default"的Token
  // 语句命中： <default_spec> ::= "default" "=" <sexpr>
  this->iMap->SetCellular(SyntaxType::case_default_spec, DBTokenType::token_default,
    new CandidateFunction(iProco, CFunctionType::deri___default_spec__sexpr_10));
  // 流命中： <sexpr> ::= "("的Token
  // 语句命中： <sexpr> ::= <smulti> <splus>
  this->iMap->SetCellular(SyntaxType::case_sexpr, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr__smulti__sexpr_pi_12));
  // 流命中： <sexpr> ::= "+"的Token
  // 语句命中： <sexpr> ::= <smulti> <splus>
  this->iMap->SetCellular(SyntaxType::case_sexpr, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr__smulti__sexpr_pi_12));
  // 流命中： <sexpr> ::= "-"的Token
  // 语句命中： <sexpr> ::= <smulti> <splus>
  this->iMap->SetCellular(SyntaxType::case_sexpr, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr__smulti__sexpr_pi_12));
  // 流命中： <sexpr> ::= number的Token
  // 语句命中： <sexpr> ::= <smulti> <splus>
  this->iMap->SetCellular(SyntaxType::case_sexpr, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr__smulti__sexpr_pi_12));
  // 流命中： <sexpr_pi> ::= ")"的Token
  // 语句命中： <sexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_sexpr_pi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr_pi__epsilon_71));
  // 流命中： <sexpr_pi> ::= ","的Token
  // 语句命中： <sexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_sexpr_pi, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr_pi__epsilon_71));
  // 流命中： <sexpr_pi> ::= "+"的Token
  // 语句命中： <sexpr_pi> ::= <splus> <sexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_sexpr_pi, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr_pi__splus__sexpr_pi_70));
  // 流命中： <sexpr_pi> ::= "-"的Token
  // 语句命中： <sexpr_pi> ::= <splus> <sexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_sexpr_pi, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___sexpr_pi__splus__sexpr_pi_70));
  // 流命中： <splus> ::= "+"的Token
  // 语句命中： <splus> ::= "+" <smulti>
  this->iMap->SetCellular(SyntaxType::case_splus, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___splus__plus_smulti_14));
  // 流命中： <splus> ::= "-"的Token
  // 语句命中： <splus> ::= "-" <smulti>
  this->iMap->SetCellular(SyntaxType::case_splus, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___splus__minus_smulti_15));
  // 流命中： <smulti> ::= "("的Token
  // 语句命中： <smulti> ::= <sunit> <smultiOpt>
  this->iMap->SetCellular(SyntaxType::case_smulti, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___smulti__sunit__smultiOpt_17));
  // 流命中： <smulti> ::= "+"的Token
  // 语句命中： <smulti> ::= <sunit> <smultiOpt>
  this->iMap->SetCellular(SyntaxType::case_smulti, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___smulti__sunit__smultiOpt_17));
  // 流命中： <smulti> ::= "-"的Token
  // 语句命中： <smulti> ::= <sunit> <smultiOpt>
  this->iMap->SetCellular(SyntaxType::case_smulti, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___smulti__sunit__smultiOpt_17));
  // 流命中： <smulti> ::= number的Token
  // 语句命中： <smulti> ::= <sunit> <smultiOpt>
  this->iMap->SetCellular(SyntaxType::case_smulti, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___smulti__sunit__smultiOpt_17));
  // 流命中： <smultiOpt> ::= ")"的Token
  // 语句命中： <smultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_smultiOpt, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___smultiOpt__epsilon_20));
  // 流命中： <smultiOpt> ::= ","的Token
  // 语句命中： <smultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_smultiOpt, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___smultiOpt__epsilon_20));
  // 流命中： <smultiOpt> ::= "+"的Token
  // 语句命中： <smultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_smultiOpt, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___smultiOpt__epsilon_20));
  // 流命中： <smultiOpt> ::= "-"的Token
  // 语句命中： <smultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_smultiOpt, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___smultiOpt__epsilon_20));
  // 流命中： <smultiOpt> ::= "*"的Token
  // 语句命中： <smultiOpt> ::= "*" <sunit>
  this->iMap->SetCellular(SyntaxType::case_smultiOpt, DBTokenType::token_Multiply_,
    new CandidateFunction(iProco, CFunctionType::deri___smultiOpt__multi_sunit__smultiOpt_18));
  // 流命中： <smultiOpt> ::= "/"的Token
  // 语句命中： <smultiOpt> ::= "/" <sunit>
  this->iMap->SetCellular(SyntaxType::case_smultiOpt, DBTokenType::token_Divide_,
    new CandidateFunction(iProco, CFunctionType::deri___smultiOpt__div_sunit__smultiOpt_19));
  // 流命中： <sunit> ::= "("的Token
  // 语句命中： <sunit> ::= "(" <sexpr> ")"
  this->iMap->SetCellular(SyntaxType::case_sunit, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___sunit__brucket_sexpr_24));
  // 流命中： <sunit> ::= "+"的Token
  // 语句命中： <sunit> ::= "+" <sunit>
  this->iMap->SetCellular(SyntaxType::case_sunit, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___sunit__plus_sunit_23));
  // 流命中： <sunit> ::= "-"的Token
  // 语句命中： <sunit> ::= "-" <sunit>
  this->iMap->SetCellular(SyntaxType::case_sunit, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___sunit__minus_sunit_22));
  // 流命中： <sunit> ::= number的Token
  // 语句命中： <sunit> ::= number
  this->iMap->SetCellular(SyntaxType::case_sunit, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___sunit__number_21));
  // 流命中： <insert_stmt> ::= "insert"的Token
  // 语句命中： <insert_stmt> ::= "insert" "into" iden "(" <column_list> ")" "values" "(" <value_list> ")" ";"
  this->iMap->SetCellular(SyntaxType::case_insert_stmt, DBTokenType::token_insert,
    new CandidateFunction(iProco, CFunctionType::deri___insert_stmt__column_list__value_list_28));
  // 流命中： <value_list> ::= iden的Token
  // 语句命中： <value_list> ::= <wexpr> <value_listpi>
  this->iMap->SetCellular(SyntaxType::case_value_list, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___value_list__sexpr__value_listpi_29));
  // 流命中： <value_list> ::= "("的Token
  // 语句命中： <value_list> ::= <wexpr> <value_listpi>
  this->iMap->SetCellular(SyntaxType::case_value_list, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___value_list__sexpr__value_listpi_29));
  // 流命中： <value_list> ::= "+"的Token
  // 语句命中： <value_list> ::= <wexpr> <value_listpi>
  this->iMap->SetCellular(SyntaxType::case_value_list, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___value_list__sexpr__value_listpi_29));
  // 流命中： <value_list> ::= "-"的Token
  // 语句命中： <value_list> ::= <wexpr> <value_listpi>
  this->iMap->SetCellular(SyntaxType::case_value_list, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___value_list__sexpr__value_listpi_29));
  // 流命中： <value_list> ::= number的Token
  // 语句命中： <value_list> ::= <wexpr> <value_listpi>
  this->iMap->SetCellular(SyntaxType::case_value_list, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___value_list__sexpr__value_listpi_29));
  // 流命中： <value_listpi> ::= ")"的Token
  // 语句命中： <value_listpi> ::= null
  this->iMap->SetCellular(SyntaxType::case_value_listpi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___value_listpi__epsilon_31));
  // 流命中： <value_listpi> ::= ","的Token
  // 语句命中： <value_listpi> ::= "," <wexpr> <value_listpi>
  this->iMap->SetCellular(SyntaxType::case_value_listpi, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___value_listpi__sexpr__value_listpi_30));
  // 流命中： <delete_stmt> ::= "delete"的Token
  // 语句命中： <delete_stmt> ::= "delete" "from" iden <where_clause> ";"
  this->iMap->SetCellular(SyntaxType::case_delete_stmt, DBTokenType::token_delete,
    new CandidateFunction(iProco, CFunctionType::deri___delete_stmt__where_clause_32));
  // 流命中： <where_clause> ::= ";"的Token
  // 语句命中： <where_clause> ::= null
  this->iMap->SetCellular(SyntaxType::case_where_clause, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___where_clause__epsilon_34));
  // 流命中： <where_clause> ::= "where"的Token
  // 语句命中： <where_clause> ::= "where" <disjunct>
  this->iMap->SetCellular(SyntaxType::case_where_clause, DBTokenType::token_where,
    new CandidateFunction(iProco, CFunctionType::deri___where_clause__disjunct_33));
  // 流命中： <disjunct> ::= iden的Token
  // 语句命中： <disjunct> ::= <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct__conjunct__disjunct_pi_35));
  // 流命中： <disjunct> ::= "("的Token
  // 语句命中： <disjunct> ::= <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct__conjunct__disjunct_pi_35));
  // 流命中： <disjunct> ::= "+"的Token
  // 语句命中： <disjunct> ::= <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct__conjunct__disjunct_pi_35));
  // 流命中： <disjunct> ::= "-"的Token
  // 语句命中： <disjunct> ::= <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct__conjunct__disjunct_pi_35));
  // 流命中： <disjunct> ::= number的Token
  // 语句命中： <disjunct> ::= <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct__conjunct__disjunct_pi_35));
  // 流命中： <disjunct> ::= "!"的Token
  // 语句命中： <disjunct> ::= <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct, DBTokenType::token_Not_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct__conjunct__disjunct_pi_35));
  // 流命中： <disjunct_pi> ::= ")"的Token
  // 语句命中： <disjunct_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_disjunct_pi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct_pi__epsilon_37));
  // 流命中： <disjunct_pi> ::= ";"的Token
  // 语句命中： <disjunct_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_disjunct_pi, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct_pi__epsilon_37));
  // 流命中： <disjunct_pi> ::= "||"的Token
  // 语句命中： <disjunct_pi> ::= "||" <conjunct> <disjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_disjunct_pi, DBTokenType::token_Or_Or_,
    new CandidateFunction(iProco, CFunctionType::deri___disjunct_pi__conjunct__disjunct_pi_36));
  // 流命中： <conjunct> ::= iden的Token
  // 语句命中： <conjunct> ::= <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct__bool__conjunct_pi_38));
  // 流命中： <conjunct> ::= "("的Token
  // 语句命中： <conjunct> ::= <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct__bool__conjunct_pi_38));
  // 流命中： <conjunct> ::= "+"的Token
  // 语句命中： <conjunct> ::= <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct__bool__conjunct_pi_38));
  // 流命中： <conjunct> ::= "-"的Token
  // 语句命中： <conjunct> ::= <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct__bool__conjunct_pi_38));
  // 流命中： <conjunct> ::= number的Token
  // 语句命中： <conjunct> ::= <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct__bool__conjunct_pi_38));
  // 流命中： <conjunct> ::= "!"的Token
  // 语句命中： <conjunct> ::= <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct, DBTokenType::token_Not_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct__bool__conjunct_pi_38));
  // 流命中： <conjunct_pi> ::= ")"的Token
  // 语句命中： <conjunct_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_conjunct_pi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct_pi__epsilon_40));
  // 流命中： <conjunct_pi> ::= ";"的Token
  // 语句命中： <conjunct_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_conjunct_pi, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct_pi__epsilon_40));
  // 流命中： <conjunct_pi> ::= "||"的Token
  // 语句命中： <conjunct_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_conjunct_pi, DBTokenType::token_Or_Or_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct_pi__epsilon_40));
  // 流命中： <conjunct_pi> ::= "&&"的Token
  // 语句命中： <conjunct_pi> ::= "&&" <bool> <conjunct_pi>
  this->iMap->SetCellular(SyntaxType::case_conjunct_pi, DBTokenType::token_And_And_,
    new CandidateFunction(iProco, CFunctionType::deri___conjunct_pi__bool__conjunct_pi_39));
  // 流命中： <bool> ::= iden的Token
  // 语句命中： <bool> ::= <comp>
  this->iMap->SetCellular(SyntaxType::case_bool, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___bool__comp_43));
  // 流命中： <bool> ::= "("的Token
  // 语句命中： <bool> ::= <comp>
  this->iMap->SetCellular(SyntaxType::case_bool, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___bool__comp_43));
  // 流命中： <bool> ::= "+"的Token
  // 语句命中： <bool> ::= <comp>
  this->iMap->SetCellular(SyntaxType::case_bool, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___bool__comp_43));
  // 流命中： <bool> ::= "-"的Token
  // 语句命中： <bool> ::= <comp>
  this->iMap->SetCellular(SyntaxType::case_bool, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___bool__comp_43));
  // 流命中： <bool> ::= number的Token
  // 语句命中： <bool> ::= <comp>
  this->iMap->SetCellular(SyntaxType::case_bool, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___bool__comp_43));
  // 流命中： <bool> ::= "!"的Token
  // 语句命中： <bool> ::= "!" <bool>
  this->iMap->SetCellular(SyntaxType::case_bool, DBTokenType::token_Not_,
    new CandidateFunction(iProco, CFunctionType::deri___bool__not_bool_42));
  // 流命中： <comp> ::= iden的Token
  // 语句命中： <comp> ::= <wexpr> <rop> <wexpr>
  this->iMap->SetCellular(SyntaxType::case_comp, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___comp__wexpr__rop__wexpr_44));
  // 流命中： <comp> ::= "("的Token
  // 语句命中： <comp> ::= <wexpr> <rop> <wexpr>
  this->iMap->SetCellular(SyntaxType::case_comp, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___comp__wexpr__rop__wexpr_44));
  // 流命中： <comp> ::= "+"的Token
  // 语句命中： <comp> ::= <wexpr> <rop> <wexpr>
  this->iMap->SetCellular(SyntaxType::case_comp, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___comp__wexpr__rop__wexpr_44));
  // 流命中： <comp> ::= "-"的Token
  // 语句命中： <comp> ::= <wexpr> <rop> <wexpr>
  this->iMap->SetCellular(SyntaxType::case_comp, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___comp__wexpr__rop__wexpr_44));
  // 流命中： <comp> ::= number的Token
  // 语句命中： <comp> ::= <wexpr> <rop> <wexpr>
  this->iMap->SetCellular(SyntaxType::case_comp, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___comp__wexpr__rop__wexpr_44));
  // 流命中： <rop> ::= "<>"的Token
  // 语句命中： <rop> ::= "<>"
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_LessThan_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__lessgreater_58));
  // 流命中： <rop> ::= "=="的Token
  // 语句命中： <rop> ::= "=="
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_Equality_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__equalequal_59));
  // 流命中： <rop> ::= ">"的Token
  // 语句命中： <rop> ::= ">"
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__greater_60));
  // 流命中： <rop> ::= "<"的Token
  // 语句命中： <rop> ::= "<"
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_LessThan_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__less_61));
  // 流命中： <rop> ::= ">="的Token
  // 语句命中： <rop> ::= ">="
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_GreaterThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__greaterequal_62));
  // 流命中： <rop> ::= "<="的Token
  // 语句命中： <rop> ::= "<="
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_LessThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__lessequal_63));
  // 流命中： <rop> ::= ";"的Token
  // 语句命中： <rop> ::= epsilon
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__epsilon_80));
  // 流命中： <rop> ::= "("的Token
  // 语句命中： <rop> ::= epsilon
  this->iMap->SetCellular(SyntaxType::case_rop, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___rop__epsilon_80));
  // 流命中： <wexpr> ::= iden的Token
  // 语句命中： <wexpr> ::= <wmulti> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__wmulti__wexpr_pi_45));
  // 流命中： <wexpr> ::= ";"的Token
  // 语句命中： <wexpr> ::= epsilon
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__epsilon_81));
  // 流命中： <wexpr> ::= ")"的Token
  // 语句命中： <wexpr> ::= epsilon
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__epsilon_81));
  // 流命中： <wexpr> ::= "("的Token
  // 语句命中： <wexpr> ::= <wmulti> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__wmulti__wexpr_pi_45));
  // 流命中： <wexpr> ::= "+"的Token
  // 语句命中： <wexpr> ::= <wmulti> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__wmulti__wexpr_pi_45));
  // 流命中： <wexpr> ::= "-"的Token
  // 语句命中： <wexpr> ::= <wmulti> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__wmulti__wexpr_pi_45));
  // 流命中： <wexpr> ::= number的Token
  // 语句命中： <wexpr> ::= <wmulti> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr__wmulti__wexpr_pi_45));
  // 流命中： <wexpr_pi> ::= ")"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= ";"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= ","的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= "+"的Token
  // 语句命中： <wexpr_pi> ::= <wplus> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__wplus__wexpr_pi_72));
  // 流命中： <wexpr_pi> ::= "-"的Token
  // 语句命中： <wexpr_pi> ::= <wplus> <wexpr_pi>
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__wplus__wexpr_pi_72));
  // 流命中： <wexpr_pi> ::= "||"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Or_Or_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= "&&"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_And_And_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= "<>"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_LessThan_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= "=="的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Equality_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= ">"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= "<"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_LessThan_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= ">="的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_GreaterThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= "<="的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_LessThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wexpr_pi> ::= ")"的Token
  // 语句命中： <wexpr_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wplus> ::= ";"的Token
  // 语句命中： <wplus> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wplus> ::= ","的Token
  // 语句命中： <wplus> ::= null
  this->iMap->SetCellular(SyntaxType::case_wexpr_pi, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___wexpr_pi__epsilon_73));
  // 流命中： <wplus> ::= "+"的Token
  // 语句命中： <wplus> ::= "+" <wmulti>
  this->iMap->SetCellular(SyntaxType::case_wplus, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___wplus__plus_wmulti_46));
  // 流命中： <wplus> ::= "-"的Token
  // 语句命中： <wplus> ::= "-" <wmulti>
  this->iMap->SetCellular(SyntaxType::case_wplus, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___wplus__minus_wmulti_47));
  // 流命中： <wmulti> ::= iden的Token
  // 语句命中： <wmulti> ::= <wunit> <wmultiOpt>
  this->iMap->SetCellular(SyntaxType::case_wmulti, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___wmulti__wunit__wmultiOpt_49));
  // 流命中： <wmulti> ::= "("的Token
  // 语句命中： <wmulti> ::= <wunit> <wmultiOpt>
  this->iMap->SetCellular(SyntaxType::case_wmulti, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wmulti__wunit__wmultiOpt_49));
  // 流命中： <wmulti> ::= "+"的Token
  // 语句命中： <wmulti> ::= <wunit> <wmultiOpt>
  this->iMap->SetCellular(SyntaxType::case_wmulti, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___wmulti__wunit__wmultiOpt_49));
  // 流命中： <wmulti> ::= "-"的Token
  // 语句命中： <wmulti> ::= <wunit> <wmultiOpt>
  this->iMap->SetCellular(SyntaxType::case_wmulti, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___wmulti__wunit__wmultiOpt_49));
  // 流命中： <wmulti> ::= number的Token
  // 语句命中： <wmulti> ::= <wunit> <wmultiOpt>
  this->iMap->SetCellular(SyntaxType::case_wmulti, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___wmulti__wunit__wmultiOpt_49));
  // 流命中： <wmultiOpt> ::= ")"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= ";"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= ","的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "+"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "-"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "*"的Token
  // 语句命中： <wmultiOpt> ::= "*" <wunit>
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Multiply_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__multi_wunit__wmultiOpt_50));
  // 流命中： <wmultiOpt> ::= "/"的Token
  // 语句命中： <wmultiOpt> ::= "/" <wunit>
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Divide_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__div_wunit__wmultiOpt_51));
  // 流命中： <wmultiOpt> ::= "||"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Or_Or_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "&&"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_And_And_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "<>"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_LessThan_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "=="的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_Equality_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= ">"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "<"的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_LessThan_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= ">="的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_GreaterThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wmultiOpt> ::= "<="的Token
  // 语句命中： <wmultiOpt> ::= null
  this->iMap->SetCellular(SyntaxType::case_wmultiOpt, DBTokenType::token_LessThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::deri___wmultiOpt__epsilon_52));
  // 流命中： <wunit> ::= iden的Token
  // 语句命中： <wunit> ::= iden
  this->iMap->SetCellular(SyntaxType::case_wunit, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___wunit__iden_54));
  // 流命中： <wunit> ::= "("的Token
  // 语句命中： <wunit> ::= "(" <disjunct> ")"
  this->iMap->SetCellular(SyntaxType::case_wunit, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___wunit__brucket_disjunct_57));
  // 流命中： <wunit> ::= "+"的Token
  // 语句命中： <wunit> ::= "+" <wunit>
  this->iMap->SetCellular(SyntaxType::case_wunit, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::deri___wunit__plus_wunit_56));
  // 流命中： <wunit> ::= "-"的Token
  // 语句命中： <wunit> ::= "-" <wunit>
  this->iMap->SetCellular(SyntaxType::case_wunit, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::deri___wunit__minus_wunit_55));
  // 流命中： <wunit> ::= number的Token
  // 语句命中： <wunit> ::= number
  this->iMap->SetCellular(SyntaxType::case_wunit, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::deri___wunit__number_53));
  // 流命中： <query_stmt> ::= "select"的Token
  // 语句命中： <query_stmt> ::= "select" <select_list> "from" iden <where_clause> ";"
  this->iMap->SetCellular(SyntaxType::case_query_stmt, DBTokenType::token_select,
    new CandidateFunction(iProco, CFunctionType::deri___query_stmt__select_list__where_clause_64));
  // 流命中： <select_list> ::= iden的Token
  // 语句命中： <select_list> ::= <column_list>
  this->iMap->SetCellular(SyntaxType::case_select_list, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___select_list__column_list_65));
  // 流命中： <select_list> ::= "*"的Token
  // 语句命中： <select_list> ::= "*"
  this->iMap->SetCellular(SyntaxType::case_select_list, DBTokenType::token_Multiply_,
    new CandidateFunction(iProco, CFunctionType::deri___select_list__star_66));
  // 流命中： <column_list> ::= iden的Token
  // 语句命中： <column_list> ::= iden <column_pi>
  this->iMap->SetCellular(SyntaxType::case_column_list, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::deri___column_list__column_pi_25));
  // 流命中： <column_pi> ::= ")"的Token
  // 语句命中： <column_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_column_pi, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::deri___column_pi__epsilon_27));
  // 流命中： <column_pi> ::= ","的Token
  // 语句命中： <column_pi> ::= "," iden <column_pi>
  this->iMap->SetCellular(SyntaxType::case_column_pi, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::deri___column_pi__comma_column_pi_26));
  // 流命中： <column_pi> ::= "from"的Token
  // 语句命中： <column_pi> ::= null
  this->iMap->SetCellular(SyntaxType::case_column_pi, DBTokenType::token_from,
    new CandidateFunction(iProco, CFunctionType::deri___column_pi__epsilon_27));
  // 叶命中： "create" 的Token
  this->iMap->SetCellular(SyntaxType::tail_createLeave, DBTokenType::token_create,
    new CandidateFunction(iProco, CFunctionType::umi_create));
  // 叶命中： "table" 的Token
  this->iMap->SetCellular(SyntaxType::tail_tableLeave, DBTokenType::token_table,
    new CandidateFunction(iProco, CFunctionType::umi_table));
  // 叶命中： iden 的Token
  this->iMap->SetCellular(SyntaxType::tail_idenLeave, DBTokenType::token_iden,
    new CandidateFunction(iProco, CFunctionType::umi_iden));
  // 叶命中： "(" 的Token
  this->iMap->SetCellular(SyntaxType::tail_leftParentheses_Leave, DBTokenType::token_LeftParentheses_,
    new CandidateFunction(iProco, CFunctionType::umi_leftParentheses_));
  // 叶命中： ")" 的Token
  this->iMap->SetCellular(SyntaxType::tail_rightParentheses_Leave, DBTokenType::token_RightParentheses_,
    new CandidateFunction(iProco, CFunctionType::umi_rightParentheses_));
  // 叶命中： ";" 的Token
  this->iMap->SetCellular(SyntaxType::tail_semicolon_Leave, DBTokenType::token_Semicolon_,
    new CandidateFunction(iProco, CFunctionType::umi_semicolon_));
  // 叶命中： "," 的Token
  this->iMap->SetCellular(SyntaxType::tail_comma_Leave, DBTokenType::token_Comma_,
    new CandidateFunction(iProco, CFunctionType::umi_comma_));
  // 叶命中： null 的Token
  this->iMap->SetCellular(SyntaxType::epsilonLeave, DBTokenType::epsilon,
    new CandidateFunction(iProco, CFunctionType::umi_epsilon));
  // 叶命中： "int" 的Token
  this->iMap->SetCellular(SyntaxType::tail_intLeave, DBTokenType::token_int,
    new CandidateFunction(iProco, CFunctionType::umi_int));
  // 叶命中： "double" 的Token
  this->iMap->SetCellular(SyntaxType::tail_doubleLeave, DBTokenType::token_double,
    new CandidateFunction(iProco, CFunctionType::umi_double));
  // 叶命中： "primary" 的Token
  this->iMap->SetCellular(SyntaxType::tail_primaryLeave, DBTokenType::token_primary,
    new CandidateFunction(iProco, CFunctionType::umi_primary));
  // 叶命中： "key" 的Token
  this->iMap->SetCellular(SyntaxType::tail_keyLeave, DBTokenType::token_key,
    new CandidateFunction(iProco, CFunctionType::umi_key));
  // 叶命中： "default" 的Token
  this->iMap->SetCellular(SyntaxType::tail_defaultLeave, DBTokenType::token_default,
    new CandidateFunction(iProco, CFunctionType::umi_default));
  // 叶命中： "=" 的Token
  this->iMap->SetCellular(SyntaxType::tail_equality_Leave, DBTokenType::token_Equality_,
    new CandidateFunction(iProco, CFunctionType::umi_equality_));
  // 叶命中： "+" 的Token
  this->iMap->SetCellular(SyntaxType::tail_plus_Leave, DBTokenType::token_Plus_,
    new CandidateFunction(iProco, CFunctionType::umi_plus_));
  // 叶命中： "-" 的Token
  this->iMap->SetCellular(SyntaxType::tail_minus_Leave, DBTokenType::token_Minus_,
    new CandidateFunction(iProco, CFunctionType::umi_minus_));
  // 叶命中： "*" 的Token
  this->iMap->SetCellular(SyntaxType::tail_multiply_Leave, DBTokenType::token_Multiply_,
    new CandidateFunction(iProco, CFunctionType::umi_multiply_));
  // 叶命中： "/" 的Token
  this->iMap->SetCellular(SyntaxType::tail_divide_Leave, DBTokenType::token_Divide_,
    new CandidateFunction(iProco, CFunctionType::umi_divide_));
  // 叶命中： number 的Token
  this->iMap->SetCellular(SyntaxType::numberLeave, DBTokenType::number,
    new CandidateFunction(iProco, CFunctionType::umi_number));
  // 叶命中： "insert" 的Token
  this->iMap->SetCellular(SyntaxType::tail_insertLeave, DBTokenType::token_insert,
    new CandidateFunction(iProco, CFunctionType::umi_insert));
  // 叶命中： "into" 的Token
  this->iMap->SetCellular(SyntaxType::tail_intoLeave, DBTokenType::token_into,
    new CandidateFunction(iProco, CFunctionType::umi_into));
  // 叶命中： "values" 的Token
  this->iMap->SetCellular(SyntaxType::tail_valuesLeave, DBTokenType::token_values,
    new CandidateFunction(iProco, CFunctionType::umi_values));
  // 叶命中： "delete" 的Token
  this->iMap->SetCellular(SyntaxType::tail_deleteLeave, DBTokenType::token_delete,
    new CandidateFunction(iProco, CFunctionType::umi_delete));
  // 叶命中： "from" 的Token
  this->iMap->SetCellular(SyntaxType::tail_fromLeave, DBTokenType::token_from,
    new CandidateFunction(iProco, CFunctionType::umi_from));
  // 叶命中： "where" 的Token
  this->iMap->SetCellular(SyntaxType::tail_whereLeave, DBTokenType::token_where,
    new CandidateFunction(iProco, CFunctionType::umi_where));
  // 叶命中： "||" 的Token
  this->iMap->SetCellular(SyntaxType::tail_or_Or_Leave, DBTokenType::token_Or_Or_,
    new CandidateFunction(iProco, CFunctionType::umi_or_Or_));
  // 叶命中： "&&" 的Token
  this->iMap->SetCellular(SyntaxType::tail_and_And_Leave, DBTokenType::token_And_And_,
    new CandidateFunction(iProco, CFunctionType::umi_and_And_));
  // 叶命中： "!" 的Token
  this->iMap->SetCellular(SyntaxType::tail_not_Leave, DBTokenType::token_Not_,
    new CandidateFunction(iProco, CFunctionType::umi_not_));
  // 叶命中： "<>" 的Token
  this->iMap->SetCellular(SyntaxType::tail_lessThan_GreaterThan_Leave, DBTokenType::token_LessThan_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::umi_lessThan_GreaterThan_));
  // 叶命中： "==" 的Token
  this->iMap->SetCellular(SyntaxType::tail_equality_Equality_Leave, DBTokenType::token_Equality_Equality_,
    new CandidateFunction(iProco, CFunctionType::umi_equality_Equality_));
  // 叶命中： ">" 的Token
  this->iMap->SetCellular(SyntaxType::tail_greaterThan_Leave, DBTokenType::token_GreaterThan_,
    new CandidateFunction(iProco, CFunctionType::umi_greaterThan_));
  // 叶命中： "<" 的Token
  this->iMap->SetCellular(SyntaxType::tail_lessThan_Leave, DBTokenType::token_LessThan_,
    new CandidateFunction(iProco, CFunctionType::umi_lessThan_));
  // 叶命中： ">=" 的Token
  this->iMap->SetCellular(SyntaxType::tail_greaterThan_Equality_Leave, DBTokenType::token_GreaterThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::umi_greaterThan_Equality_));
  // 叶命中： "<=" 的Token
  this->iMap->SetCellular(SyntaxType::tail_lessThan_Equality_Leave, DBTokenType::token_LessThan_Equality_,
    new CandidateFunction(iProco, CFunctionType::umi_lessThan_Equality_));
  // 叶命中： "select" 的Token
  this->iMap->SetCellular(SyntaxType::tail_selectLeave, DBTokenType::token_select,
    new CandidateFunction(iProco, CFunctionType::umi_select));
  // 叶命中： # 的Token
  this->iMap->SetCellular(SyntaxType::tail_startEndLeave, DBTokenType::token_startEnd,
    new CandidateFunction(iProco, CFunctionType::umi_startEnd));
}

//函数作用： 通用产生式处理器
//参数列表：
//   _myNode 产生式节点
// _myParser 匹配器指针
//   _myType 候选式类型
// _mySyntax 节点语法类型
//  _myValue 命中单词文本
//返 回 值： SyntaxTreeNode* 下一个展开节点的指针
SyntaxTreeNode* Homura(SyntaxTreeNode* _myNode, void* _myParser, CFunctionType _myType, SyntaxType _mySyntax, istr _myValue) {
  // 取得匹配器，更新节点信息
  LL1SyntaxParser* iParser = static_cast<LL1SyntaxParser*>(_myParser);
  if (_myNode != nullptr) {
    _myNode->nodeType = _myType;
    _myNode->nodeValue = _myValue;
    _myNode->nodeSyntaxType = _mySyntax;
    _myNode->nodeName = SyntaxOutputTable[_mySyntax];
    _myNode->mappedList = iParser->GetTokenStream();
    _myNode->mappedBegin = iParser->iPTRnextToken;
  }
  // 取匹配栈和候选向量
  SyntaxVector iSvec = iParser->GetVector(_myType);
  SyntaxStack* iStack = iParser->GetStack();
  // 左边出栈
  iStack->pop();
  // 如果这是一个非终结符
  if (_myType < CFunctionType::DERI_UMI_BOUNDARY) {
    // 自右向左压匹配栈
    for (SyntaxVector::reverse_iterator iter = iSvec.rbegin();
      iter != iSvec.rend(); ++iter) {
      iStack->push(*iter);
    }
    // 自左向右建子节点
    bool flag = false;
    SyntaxTreeNode* successor = nullptr;
    for (SyntaxVector::const_iterator iter = iSvec.begin();
      iter != iSvec.end(); ++iter) {
      SyntaxTreeNode* newNode = new SyntaxTreeNode();
      newNode->mappedList = iParser->GetTokenStream();
      newNode->mappedBegin = iParser->iPTRnextToken;
      newNode->parent = _myNode;
      _myNode->children.push_back(newNode);
      if (flag == false) {
        successor = newNode;
        flag = true;
      }
    }
    // 返回第一个产生式
    return successor;
  }
  // 如果是一个终结符
  else {
    // 更新节点信息
    if (_myNode != nullptr && _myType != CFunctionType::umi_epsilon) {
      _myNode->mappedLength = iParser->GetTokenStream()->_tokenContainer[iParser->iPTRnextToken]->detail.size();
    }
    else if (_myType == CFunctionType::umi_epsilon) {
      _myNode->mappedLength = 0;
    }
    // 递增token指针
    if (_myType != CFunctionType::umi_epsilon) {
      iParser->iPTRnextToken++;
    }
    // 返回它的后继节点
    return iParser->NextNode(_myNode, iParser);
  }
}

CSTORE_NS_END
