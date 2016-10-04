#ifndef ___CSTORE_DBCDECLARATION
#define ___CSTORE_DBCDECLARATION

#include "DBBase.h"
CSTORE_NS_BEGIN
class Token;
class DBCProxy;
class TokenStream;
class SyntaxTreeNode;
class LL1SyntaxParser;
class CandidateFunction;

typedef int iPtr;
typedef std::string istr;
typedef std::ostream ios;
typedef std::ifstream ifile;
typedef std::vector<int> IntVec;
typedef std::vector<istr> StrVec;
typedef std::vector<IntVec> IntTable;
typedef std::vector<Token*> TokenList;
typedef std::vector<SyntaxType> SyntaxVector;
typedef std::vector<SyntaxVector> SyntaxDict;
typedef std::vector<SyntaxTreeNode*> SyntaxTreeList;
typedef std::stack<SyntaxType> SyntaxStack;
typedef std::pair<istr, int> PilePair;
typedef std::pair<TokenType, int> TokenPair;
typedef std::pair<SyntaxType, int> SyntaxPair;
typedef std::map<istr, int> TablePileDictionary;
typedef std::map<TokenType, int> TokenDictionary;
typedef std::map<SyntaxType, int> SyntaxDictionary;
typedef CandidateFunction*** ParserMap;
typedef SyntaxTreeNode* (*iHandle)(SyntaxTreeNode*, void*, CFunctionType, SyntaxType, istr);

// 用于输出Debug单词类型的字符串向量
const istr TokenOutputTable[] = {
  "unknown", "token_create", "token_table", "token_iden",
  "token_LeftParentheses", "token_RightParentheses",
  "token_Semicolon", "token_Comma", "epsilon",
  "token_int", "token_double", "token_primary", "token_key",
  "token_default", "token_Equality", "token_Plus",
  "token_Minus", "token_Multiply", "token_Divide",
  "number", "token_insert", "token_into",
  "token_values", "token_delete", "token_from",
  "token_where", "token_Or_Or", "token_And_And",
  "token_Not", "token_LessThan_GreaterThan",
  "token_Equality_Equality", "token_GreaterThan",
  "token_LessThan", "token_GreaterThan_Equality",
  "token_LessThan_Equality", "token_select",
  "token_StartEnd", "other"
};

// 用于输出Debug语法树的字符串向量
const istr SyntaxOutputTable[] = {
  "case_ssql_stmt", "case_create_stmt", "case_decl_list", "case_decl_listpi",
  "case_decl", "case_decltype", "case_default_spec", "case_sexpr", "case_sexpr_pi", "case_splus",
  "case_smulti", "case_smultiOpt", "case_sunit", "case_insert_stmt",
  "case_value_list", "case_value_listpi", "case_delete_stmt", "case_where_clause",
  "case_disjunct", "case_disjunct_pi", "case_conjunct", "case_conjunct_pi",
  "case_bool", "case_comp", "case_rop", "case_wexpr", "case_wexpr_pi",
  "case_wplus", "case_wmulti", "case_wmultiOpt", "case_wunit",
  "case_query_stmt", "case_select_list", "case_column_list", "case_column_pi",
  "Unknown", "tail_createLeave", "tail_tableLeave", "tail_idenLeave",
  "tail_leftParentheses_Leave", "tail_rightParentheses_Leave", "tail_semicolon_Leave",
  "tail_comma_Leave", "epsilonLeave", "tail_intLeave", "tail_doubleLeave", "tail_primaryLeave", "tail_keyLeave",
  "tail_defaultLeave", "tail_equality_Leave", "tail_plus_Leave", "tail_minus_Leave",
  "tail_multiply_Leave", "tail_divide_Leave", "numberLeave", "tail_insertLeave",
  "tail_intoLeave", "tail_valuesLeave", "tail_deleteLeave", "tail_fromLeave",
  "tail_whereLeave", "tail_or_Or_Leave", "tail_and_And_Leave", "tail_not_Leave",
  "tail_lessThan_GreaterThan_Leave", "tail_equality_Equality_Leave", "tail_greaterThan_Leave",
  "tail_lessThan_Leave", "tail_greaterThan_Equality_Leave", "tail_lessThan_Equality_Leave",
  "tail_selectLeave", "tail_startEndLeave"
};
CSTORE_NS_END
#endif /* ___CSTORE_DBCDECLARATION */
