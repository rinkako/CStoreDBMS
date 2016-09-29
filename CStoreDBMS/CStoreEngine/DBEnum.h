#ifndef ___CSTORE_DBENUM
#define ___CSTORE_DBENUM

// 枚举：数据库运行模式
typedef enum __RunType {
  // 命令直接调用
  RUN_COMMAND,
  // 控制台
  RUN_CONSOLE,
  // SQL文件
  RUN_INFILE
} RunType;

// 枚举：数据库动作
typedef enum __DashType {
  // 空
  dash_nop,
  // 载入
  dash_load,
  // 建表
  dash_create,
  // 检索
  dash_retrieve,
  // 压缩
  dash_compress,
  // 拼表
  dash_join,
  // 查询记录数量
  dash_count,
  // 更新
  dash_update,
  // 插入行
  dash_insert,
  // 删除行
  dash_delete,
  // 查询
  dash_select
} DashType;

// 枚举：字符类型
typedef enum __CharaType {
  // 未知
  cUnknown,
  // 字母
  Letter,
  // 数字
  Number,
  // _
  UnderLine,
  // .
  Dot,
  // ,
  Comma,
  // +
  Plus,
  // -
  Minus,
  // *
  Multiply,
  // /
  Divide,
  // %
  Percent,
  // ^
  Xor,
  // &;
  And,
  // |
  Or,
  // ~
  Reverse,
  // $
  Dollar,
  // <
  LessThan,
  // >
  GreaterThan,
  // (
  LeftParentheses,
  // )
  RightParentheses,
  // [
  LeftBracket,
  // ]
  RightBracket,
  // {
  LeftBrace,
  // }
  RightBrace,
  // !
  Not,
  // #
  Pound,
  // "\\"
  Slash,
  // ?
  Question,
  // '
  Quotation,
  // "
  DoubleQuotation,
  // :
  Colon,
  // ;
  Semicolon,
  // =
  Equality,
  // space Tab \r\n
  Space,
} CharaType;

// 枚举：Token类型
typedef enum __ITokenType {
  // 未知的单词符号
  unknown,
  // "create"
  token_create,
  // "table"
  token_table,
  // iden
  token_iden,
  // "("
  token_LeftParentheses_,
  // ")"
  token_RightParentheses_,
  // ";"
  token_Semicolon_,
  // ","
  token_Comma_,
  // null
  epsilon,
  // "int"
  token_int,
  // "primary"
  token_primary,
  // "key"
  token_key,
  // "default"
  token_default,
  // "="
  token_Equality_,
  // "+"
  token_Plus_,
  // "-"
  token_Minus_,
  // "*"
  token_Multiply_,
  // "/"
  token_Divide_,
  // number
  number,
  // "insert"
  token_insert,
  // "into"
  token_into,
  // "values"
  token_values,
  // "delete"
  token_delete,
  // "from"
  token_from,
  // "where"
  token_where,
  // "||"
  token_Or_Or_,
  // "&&"
  token_And_And_,
  // "!"
  token_Not_,
  // "<>"
  token_LessThan_GreaterThan_,
  // "=="
  token_Equality_Equality_,
  // ">"
  token_GreaterThan_,
  // "<"
  token_LessThan_,
  // ">="
  token_GreaterThan_Equality_,
  // "<="
  token_LessThan_Equality_,
  // "select"
  token_select,
  // #
  token_startEnd,
  // 标识符
  identifier,
  // "load"
  token_load,
  // "retrieve"
  token_retrieve,
  // "compress"
  token_compress,
  // "join"
  token_join,
  // "count"
  token_count,
  // "on"
  token_on,
  // "set"
  token_set
} TokenType;

// 枚举：语法树节点类型
typedef enum __ISyntaxType
{
  // <ssql_stmt> ::= <create_stmt> | <insert_stmt> | <delete_stmt> | <query_stmt>;
  case_ssql_stmt,
  // <create_stmt> ::= "create" "table" identifier "(" <decl_list> ")" ";";
  case_create_stmt,
  // <decl_list> ::= <decl> <decl_listpi>;
  case_decl_list,
  // <decl_listpi> ::= "," <decl> <decl_listpi> | null;
  case_decl_listpi,
  // <decl> ::= identifier "int" <default_spec> | "primary" "key" "(" <column_list> ")";
  case_decl,
  // <default_spec> ::= "default" "=" <sexpr> | null;
  case_default_spec,
  // <sexpr> ::= <smulti> <sexpr_pi>;
  case_sexpr,
  // <sexpr_pi> ::= <splus> <sexpr_pi> | null;
  case_sexpr_pi,
  // <splus> ::= "+" <smulti> | "-" <smulti>;
  case_splus,
  // <smulti> ::= <sunit> <smultiOpt>;
  case_smulti,
  // <smultiOpt> ::= "*" <sunit> | "/" <sunit> | null;
  case_smultiOpt,
  // <sunit> ::= number | "-" <sunit> | "+" <sunit> | "(" <sexpr> ")";
  case_sunit,
  // <insert_stmt> ::= "insert" "into" identifier "(" <column_list> ")" "values" "(" <value_list> ")" ";";
  case_insert_stmt,
  // <value_list> ::= <wexpr> <value_listpi>;
  case_value_list,
  // <value_listpi> ::= "," <wexpr> <value_listpi> | null;
  case_value_listpi,
  // <delete_stmt> ::= "delete" "from" identifier <where_clause> ";";
  case_delete_stmt,
  // <where_clause> ::= "where" <disjunct> | null;
  case_where_clause,
  // <disjunct> ::= <conjunct> <disjunct_pi>;
  case_disjunct,
  // <disjunct_pi> ::= "||" <conjunct> <disjunct_pi> | null;
  case_disjunct_pi,
  // <conjunct> ::= <bool> <conjunct_pi>;
  case_conjunct,
  // <conjunct_pi> ::= "&&" <bool> <conjunct_pi> | null;
  case_conjunct_pi,
  // <bool> ::= "(" <disjunct> ")" | "!" <bool> | <comp>;
  case_bool,
  // <comp> ::= <wexpr> <rop> <wexpr>;
  case_comp,
  // <rop> ::= "<>" | "==" | ">" | "<" | ">=" | "<=" | null;
  case_rop,
  // <wexpr> ::= <wmulti> <wexpr_pi>;
  case_wexpr,
  // <wexpr> ::= <wplus> <wexpr_pi> | null;
  case_wexpr_pi,
  // <wplus> ::= "+" <wmulti> | "-" <wmulti>;
  case_wplus,
  // <wmulti> ::= <wunit> <wmultiOpt>;
  case_wmulti,
  // <wmultiOpt> ::= "*" <wunit> | "/" <wunit> | null;
  case_wmultiOpt,
  // <wunit> ::= number | identifier | "-" <wunit> | "+" <wunit> | "(" <wexpr> ")";
  case_wunit,
  // <query_stmt> ::= "select" <select_list> "from" identifier <where_clause> ";";
  case_query_stmt,
  // <select_list> ::= <column_list> | "*";
  case_select_list,
  // <column_list> ::= identifier <column_pi>;
  case_column_list,
  // <column_pi> ::= "," identifier <column_pi> | null;
  case_column_pi,
  // 未知的语法结点符号
  Unknown,
  // "create"
  tail_createLeave,
  // "table"
  tail_tableLeave,
  // iden
  tail_idenLeave,
  // "("
  tail_leftParentheses_Leave,
  // ")"
  tail_rightParentheses_Leave,
  // ";"
  tail_semicolon_Leave,
  // ","
  tail_comma_Leave,
  // null
  epsilonLeave,
  // "int"
  tail_intLeave,
  // "primary"
  tail_primaryLeave,
  // "key"
  tail_keyLeave,
  // "default"
  tail_defaultLeave,
  // "="
  tail_equality_Leave,
  // "+"
  tail_plus_Leave,
  // "-"
  tail_minus_Leave,
  // "*"
  tail_multiply_Leave,
  // "/"
  tail_divide_Leave,
  // number
  numberLeave,
  // "insert"
  tail_insertLeave,
  // "into"
  tail_intoLeave,
  // "values"
  tail_valuesLeave,
  // "delete"
  tail_deleteLeave,
  // "from"
  tail_fromLeave,
  // "where"
  tail_whereLeave,
  // "||"
  tail_or_Or_Leave,
  // "&&"
  tail_and_And_Leave,
  // "!"
  tail_not_Leave,
  // "<>"
  tail_lessThan_GreaterThan_Leave,
  // "=="
  tail_equality_Equality_Leave,
  // ">"
  tail_greaterThan_Leave,
  // "<"
  tail_lessThan_Leave,
  // ">="
  tail_greaterThan_Equality_Leave,
  // "<="
  tail_lessThan_Equality_Leave,
  // "select"
  tail_selectLeave,
  // #
  tail_startEndLeave,
  // cstore load
  cstore_load,
  // cstore retrieve
  cstore_retrieve,
  // cstore compress
  cstore_compress,
  // cstore join
  cstore_join,
  // cstore count
  cstore_count
} SyntaxType;

// 枚举：语法候选式类型
typedef enum __FunctionType {
  // <ssql_stmt> -> <create_stmt>
  deri___ssql_stmt__create_stmt_1,
  // <ssql_stmt> -> <insert_stmt>
  deri___ssql_stmt__insert_stmt_2,
  // <ssql_stmt> -> <delete_stmt>
  deri___ssql_stmt__delete_stmt_3,
  // <ssql_stmt> -> <query_stmt>
  deri___ssql_stmt__query_stmt_4,
  // <create_stmt> -> "create" "table" id "(" <decl_list> ")" ";"
  deri___create_stmt__decl_list_5,
  // <decl_list> -> <decl> <decl_listpi>
  deri___decl_list__decl__decl_listpi_6,
  // <decl_listpi> -> "," <decl> <decl_listpi>
  deri___decl_listpi__decl__decl_listpi_67,
  // <decl_listpi> -> epsilon
  deri___decl_listpi__epsilon_7,
  // <decl> -> id "int" <default_spec>
  deri___decl__default_spec_8,
  // <decl> -> "primary" "key" "(" <column_list> ")"
  deri___decl__column_list_9,
  // <default_spec> -> "default" "=" sexpr
  deri___default_spec__sexpr_10,
  // <default_spec> -> epsilon
  deri___default_spec__epsilon_11,
  // <sexpr> -> <smulti> <sexpr_pi>
  deri___sexpr__smulti__sexpr_pi_12,
  // <splus> -> "+" <smulti>
  deri___splus__plus_smulti_14,
  // <splus> -> "-" <smulti>
  deri___splus__minus_smulti_15,
  // <sexpr_pi> -> <splus> <sexpr_pi>
  deri___sexpr_pi__splus__sexpr_pi_70,
  // <sexpr_pi> -> epsilon
  deri___sexpr_pi__epsilon_71,
  // <smulti> -> <sunit> <smultiOpt>
  deri___smulti__sunit__smultiOpt_17,
  // <smultiOpt> -> "*" <sunit> <smultiOpt>
  deri___smultiOpt__multi_sunit__smultiOpt_18,
  // <smultiOpt> -> "/" <sunit> <smultiOpt>
  deri___smultiOpt__div_sunit__smultiOpt_19,
  // <smultiOpt> -> epsilon
  deri___smultiOpt__epsilon_20,
  // <sunit> -> number
  deri___sunit__number_21,
  // <sunit> -> "-" <sunit>
  deri___sunit__minus_sunit_22,
  // <sunit> -> "+" <sunit>
  deri___sunit__plus_sunit_23,
  // <sunit> -> "(" <sexpr> ")"
  deri___sunit__brucket_sexpr_24,
  // <insert_stmt> -> "insert" "into" id "(" <column_list> ")" "values" "(" <value_list> ")" ";"
  deri___insert_stmt__column_list__value_list_28,
  // <value_list> -> <wexpr> <value_listpi>
  deri___value_list__sexpr__value_listpi_29,
  // <value_listpi> -> "," <wexpr> <value_listpi>
  deri___value_listpi__sexpr__value_listpi_30,
  // <value_listpi> -> epsilon
  deri___value_listpi__epsilon_31,
  // <delete_stmt> -> "delete" "from" id <where_clause> ";"
  deri___delete_stmt__where_clause_32,
  // <where_clause> -> "where" <disjunct>
  deri___where_clause__disjunct_33,
  // <where_clause> -> epsilon
  deri___where_clause__epsilon_34,
  // <disjunct> -> <conjunct> <disjunct_pi>
  deri___disjunct__conjunct__disjunct_pi_35,
  // <disjunct_pi> -> "||" <conjunct> <disjunct_pi>
  deri___disjunct_pi__conjunct__disjunct_pi_36,
  // <disjunct_pi> -> epsilon
  deri___disjunct_pi__epsilon_37,
  // <conjunct> -> <bool> <conjunct_pi>
  deri___conjunct__bool__conjunct_pi_38,
  // <conjunct_pi> -> "&&" <bool> <conjunct_pi>
  deri___conjunct_pi__bool__conjunct_pi_39,
  // <conjunct_pi> -> epsilon
  deri___conjunct_pi__epsilon_40,
  // <bool> -> "!" <bool>
  deri___bool__not_bool_42,
  // <bool> -> <comp>
  deri___bool__comp_43,
  // <comp> -> <wexpr> <rop> <wexpr>
  deri___comp__wexpr__rop__wexpr_44,
  // <rop> -> "<>"
  deri___rop__lessgreater_58,
  // <rop> -> "=="
  deri___rop__equalequal_59,
  // <rop> -> ">"
  deri___rop__greater_60,
  // <rop> -> "<"
  deri___rop__less_61,
  // <rop> -> ">="
  deri___rop__greaterequal_62,
  // <rop> -> "<="
  deri___rop__lessequal_63,
  // <rop> -> epsilon
  deri___rop__epsilon_80,
  // <wexpr> -> <wmulti> <wexpr_pi>
  deri___wexpr__wmulti__wexpr_pi_45,
  // <wexpr> -> epsilon
  deri___wexpr__epsilon_81,
  // <wplus> -> "+" <wmulti>
  deri___wplus__plus_wmulti_46,
  // <wplus> -> "-" <wmulti>
  deri___wplus__minus_wmulti_47,
  // <wexpr_pi> -> <wplus> <wexpr_pi>
  deri___wexpr_pi__wplus__wexpr_pi_72,
  // <wexpr_pi> -> epsilon
  deri___wexpr_pi__epsilon_73,
  // <wmulti> -> <wunit> <wmultiOpt>
  deri___wmulti__wunit__wmultiOpt_49,
  // <wmultiOpt> -> "*" <wunit> <wmultiOpt>
  deri___wmultiOpt__multi_wunit__wmultiOpt_50,
  // <wmultiOpt> -> "/" <wunit> <wmultiOpt>
  deri___wmultiOpt__div_wunit__wmultiOpt_51,
  // <wmultiOpt> -> epsilon
  deri___wmultiOpt__epsilon_52,
  // <wunit> -> number
  deri___wunit__number_53,
  // <wunit> -> iden
  deri___wunit__iden_54,
  // <wunit> -> "-" <wunit>
  deri___wunit__minus_wunit_55,
  // <wunit> -> "+" <wunit>
  deri___wunit__plus_wunit_56,
  // <wunit> -> "(" <disjunct> ")"
  deri___wunit__brucket_disjunct_57,
  // <query_stmt> -> "select" <select_list> "from" id <where_clause> ";"
  deri___query_stmt__select_list__where_clause_64,
  // <select_list> -> <column_list>
  deri___select_list__column_list_65,
  // <select_list> -> "*"
  deri___select_list__star_66,
  // <column_list> -> id <column_pi>
  deri___column_list__column_pi_25,
  // <column_pi> -> "," id <column_pi>
  deri___column_pi__comma_column_pi_26,
  // <column_pi> -> epsilon
  deri___column_pi__epsilon_27,
  // BOUNDARY
  DERI_UMI_BOUNDARY,
  // LeavePoint "create"
  umi_create,
  // LeavePoint "table"
  umi_table,
  // LeavePoint iden
  umi_iden,
  // LeavePoint "("
  umi_leftParentheses_,
  // LeavePoint ")"
  umi_rightParentheses_,
  // LeavePoint ";"
  umi_semicolon_,
  // LeavePoint ","
  umi_comma_,
  // LeavePoint null
  umi_epsilon,
  // LeavePoint "int"
  umi_int,
  // LeavePoint "primary"
  umi_primary,
  // LeavePoint "key"
  umi_key,
  // LeavePoint "default"
  umi_default,
  // LeavePoint "="
  umi_equality_,
  // LeavePoint "+"
  umi_plus_,
  // LeavePoint "-"
  umi_minus_,
  // LeavePoint "*"
  umi_multiply_,
  // LeavePoint "/"
  umi_divide_,
  // LeavePoint number
  umi_number,
  // LeavePoint "insert"
  umi_insert,
  // LeavePoint "into"
  umi_into,
  // LeavePoint "values"
  umi_values,
  // LeavePoint "delete"
  umi_delete,
  // LeavePoint "from"
  umi_from,
  // LeavePoint "where"
  umi_where,
  // LeavePoint "||"
  umi_or_Or_,
  // LeavePoint "&&"
  umi_and_And_,
  // LeavePoint "!"
  umi_not_,
  // LeavePoint "<>"
  umi_lessThan_GreaterThan_,
  // LeavePoint "=="
  umi_equality_Equality_,
  // LeavePoint ">"
  umi_greaterThan_,
  // LeavePoint "<"
  umi_lessThan_,
  // LeavePoint ">="
  umi_greaterThan_Equality_,
  // LeavePoint "<="
  umi_lessThan_Equality_,
  // LeavePoint "select"
  umi_select,
  // LeavePoint #
  umi_startEnd,
  // Cstore Node
  umi_cstore,
  // ERROR
  umi_errorEnd
} CFunctionType;

// 枚举：数据库操作类型
typedef enum __DBOperationType {

} DBOperationType;

// 枚举：数据表锁状态
typedef enum __DBTableLockType {
  // 未加锁
  tblock_none,
  // 共享锁
  tblock_share,
  // 互斥锁
  tblock_mutex
} TableLockType;

#endif /* ___CSTORE_DBENUM */