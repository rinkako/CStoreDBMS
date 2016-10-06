#include "Pile.h"

CSTORE_NS_BEGIN

// IPile构造函数
IPile::IPile()
  :DBObject("IPile", this) {
  this->Reset();
}

// IPile复位函数
void IPile::Reset() {
  proxyID = 0;
  lastPi = "";
  this->parseTree = NULL;
}

// IPile设置分析树
void IPile::SetParseTree(SyntaxTreeNode* _myNode) {
  this->parseTree = _myNode;
}

// IPile启动语义分析器
DBCProxy IPile::Semanticer() {
  // 定义结果proxy
  DBCProxy myProxy;
  myProxy.id = this->proxyID++;
  myProxy.encounter = 0;
  myProxy.errorBit = false;
  myProxy.condPtr = NULL;
  myProxy.opCode = DashType::dash_nop;
  myProxy.isStar = false;
  // 空树直接返回
  if (this->parseTree == NULL) {
    return myProxy;
  }
  // 自顶向下递归遍历语法树
  this->Mise(myProxy, this->parseTree, 0);
  return myProxy;
}

// IPile语法树遍历（flag：0表，1列，2主列，3列初始值）
void IPile::Mise(DBCProxy &myproxy, SyntaxTreeNode* mynode, int flag) {
  switch (mynode->nodeSyntaxType)
  {
  case SyntaxType::SyntaxUnknown:
    break;
  case SyntaxType::cstore_load:
    myproxy.opCode = DashType::dash_load;
    myproxy.opTable = mynode->nodeValue;
    break;
  case SyntaxType::cstore_retrieve:
    myproxy.opCode = DashType::dash_retrieve;
    myproxy.opTable = CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[0];
    myproxy.aTag = atof(CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[1].c_str());
    break;
  case SyntaxType::cstore_compress:
    myproxy.opCode = DashType::dash_compress;
    myproxy.opTable = CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[0];
    myproxy.Pi.push_back(CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[1]);
    break;
  case SyntaxType::cstore_join:
    myproxy.opCode = DashType::dash_join;
    myproxy.opTable = CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[0];
    myproxy.opTablePaired = CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[1];
    break;
  case SyntaxType::cstore_count:
    myproxy.opCode = DashType::dash_count;
    myproxy.opTable = CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[0];
    myproxy.Pi.push_back(CSCommonUtil::CStrSplit(mynode->nodeValue, "@")[1]);
    break;
  case SyntaxType::case_ssql_stmt:
    Mise(myproxy, mynode->children[0], flag);
    break;
  case SyntaxType::case_create_stmt:
    myproxy.opCode = DashType::dash_create;
    Mise(myproxy, mynode->children[2], 0); // id
    Mise(myproxy, mynode->children[4], 1); // decl_list
    break;
  case SyntaxType::case_decl_list:
    Mise(myproxy, mynode->children[0], flag); // decl
    Mise(myproxy, mynode->children[1], flag); // decl_listpi
    break;
  case SyntaxType::case_decl:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___decl__decltype__default_spec_8) {
      Mise(myproxy, mynode->children[0], flag); // id
      Mise(myproxy, mynode->children[1], flag); // decltype
      Mise(myproxy, mynode->children[2], 3); // default_spec
    }
    else {
      myproxy.encounter++;
      Mise(myproxy, mynode->children[3], 2); // column_list
    }
    break;
  case SyntaxType::case_decltype:
    // INT
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___decltype__intleave) {
      myproxy.PiType.push_back("INT");
    }
    // DOUBLE
    else {
      myproxy.PiType.push_back("DOUBLE");
    }
    break;
  case SyntaxType::case_decl_listpi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___decl_listpi__decl__decl_listpi_67) {
      Mise(myproxy, mynode->children[1], flag); // decl
      Mise(myproxy, mynode->children[2], flag); // decl_listpi
    }
    break;
  case SyntaxType::case_default_spec:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___default_spec__sexpr_10) {
      Mise(myproxy, mynode->children[2], flag);
    }
    break;
  case SyntaxType::case_sexpr:
    CSDatabase::AST(mynode, NULL, &myproxy);
    if (flag == 3) {
      myproxy.DefaPi.insert(PilePair(lastPi, (int)mynode->aTag));
    }
    myproxy.ProxyPi.push_back((int)mynode->aTag);
    break;
  case SyntaxType::case_insert_stmt:
    myproxy.opCode = DashType::dash_insert;
    Mise(myproxy, mynode->children[2], 0); // id
    Mise(myproxy, mynode->children[4], 1); // column_list
    Mise(myproxy, mynode->children[8], 1); // value_list
    break;
  case SyntaxType::case_value_list:
    Mise(myproxy, mynode->children[0], flag); // sexpr
    Mise(myproxy, mynode->children[1], flag); // value_listpi
    break;
  case SyntaxType::case_value_listpi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___value_listpi__sexpr__value_listpi_30) {
      Mise(myproxy, mynode->children[1], flag); // sexpr
      Mise(myproxy, mynode->children[2], flag); // value_listpi
    }
    break;
  case SyntaxType::case_query_stmt:
    myproxy.opCode = DashType::dash_select;
    Mise(myproxy, mynode->children[1], 1); // select_list
    Mise(myproxy, mynode->children[3], 0); // id
    Mise(myproxy, mynode->children[4], 1); // where_clause
    break;
  case SyntaxType::case_select_list:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___select_list__column_list_65) {
      Mise(myproxy, mynode->children[0], flag); // column_list
    }
    else {
      myproxy.isStar = true;
    }
    break;
  case SyntaxType::case_column_list:
    Mise(myproxy, mynode->children[0], flag); // id
    Mise(myproxy, mynode->children[1], flag); // column_pi
    break;
  case SyntaxType::case_column_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___column_pi__comma_column_pi_26) {
      Mise(myproxy, mynode->children[1], flag); // id
      Mise(myproxy, mynode->children[2], flag); // column_pi
    }
    break;
  case SyntaxType::case_delete_stmt:
    myproxy.opCode = DashType::dash_delete;
    Mise(myproxy, mynode->children[2], 0); // id
    Mise(myproxy, mynode->children[3], 1); // where_clause
    break;
  case SyntaxType::tail_idenLeave:
    switch (flag)
    {
    case 0:
      myproxy.opTable = mynode->nodeValue; // Table name
      break;
    case 2:
      myproxy.PrimaryPi.push_back(mynode->nodeValue); // Primary key
      break;
    default:
      myproxy.Pi.push_back(lastPi = mynode->nodeValue); // Pi
      break;
    }
    break;
  case SyntaxType::case_where_clause:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___where_clause__disjunct_33) {
      myproxy.condPtr = mynode->children[1]; // disjunct pointer
      Mise(myproxy, mynode->children[1], flag); // disjunct
    }
    break;
  case SyntaxType::case_disjunct:
    Mise(myproxy, mynode->children[0], flag);
    Mise(myproxy, mynode->children[1], flag);
    break;
  case SyntaxType::case_disjunct_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___disjunct_pi__conjunct__disjunct_pi_36) {
      Mise(myproxy, mynode->children[1], flag);
      Mise(myproxy, mynode->children[2], flag);
    }
    break;
  case SyntaxType::case_conjunct:
    Mise(myproxy, mynode->children[0], flag);
    Mise(myproxy, mynode->children[1], flag);
    break;
  case SyntaxType::case_conjunct_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___conjunct_pi__bool__conjunct_pi_39) {
      Mise(myproxy, mynode->children[1], flag);
      Mise(myproxy, mynode->children[2], flag);
    }
    break;
  case SyntaxType::case_bool:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___bool__not_bool_42) {
      Mise(myproxy, mynode->children[1], flag);
    }
    else {
      Mise(myproxy, mynode->children[0], flag);
    }
    break;
  case SyntaxType::case_comp:
    Mise(myproxy, mynode->children[0], flag);
    Mise(myproxy, mynode->children[2], flag);
    break;
  case SyntaxType::case_wexpr:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___wexpr__wmulti__wexpr_pi_45) {
      Mise(myproxy, mynode->children[0], flag);
      Mise(myproxy, mynode->children[1], flag);
    }
    break;
  case SyntaxType::case_wexpr_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___wexpr_pi__wplus__wexpr_pi_72) {
      Mise(myproxy, mynode->children[0], flag);
      Mise(myproxy, mynode->children[1], flag);
    }
    break;
  case SyntaxType::case_wplus:
    Mise(myproxy, mynode->children[1], flag);
    break;
  case SyntaxType::case_wmulti:
    Mise(myproxy, mynode->children[0], flag);
    Mise(myproxy, mynode->children[1], flag);
    break;
  case SyntaxType::case_wmultiOpt:
    if (mynode->candidateFunction->GetType() != CFunctionType::deri___wmultiOpt__epsilon_52) {
      Mise(myproxy, mynode->children[1], flag);
      Mise(myproxy, mynode->children[2], flag);
    }
    break;
  case SyntaxType::case_wunit:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__iden_54) {
      myproxy.CondPi.push_back(mynode->nodeValue);
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__brucket_disjunct_57) {
      Mise(myproxy, mynode->children[1], flag);
    }
    break;
  default:
    break;
  }
}

CSTORE_NS_END
