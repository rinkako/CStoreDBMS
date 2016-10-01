#ifndef ___CSTORE_DBCLL1SYNTAXPARSERMAP
#define ___CSTORE_DBCLL1SYNTAXPARSERMAP
#include "DBCDeclaration.h"
#include "DBCCandidateFunction.h"

CSTORE_NS_BEGIN

class LL1SyntaxParserMap : public DBObject {
public:
  //函数作用： 带尺寸的构造器
  //参数列表：
  //       row 行长度
  //       col 列长度
  //返 回 值： N/A
  LL1SyntaxParserMap(int, int);

  //函数作用： 设置行类型
  //参数列表：
  //       row 行长度
  //      left 非终结符类型
  //返 回 值： N/A
  void SetRow(int, SyntaxType);

  //函数作用： 设置列类型
  //参数列表：
  //       col 行长度
  //     leave Token类型
  //返 回 值： N/A
  void SetCol(int, TokenType);

  //函数作用： 设置节点分析函数
  //参数列表：
  //       row 行指针
  //       col 列指针
  //      proc 候选式函数
  //返 回 值： N/A
  void SetCellular(int, int, CandidateFunction*);

  //函数作用： 设置节点分析函数
  //参数列表：
  //      left 非终结符类型
  //     leave Token类型
  //      proc 候选式函数
  //返 回 值： N/A
  void SetCellular(SyntaxType, TokenType, CandidateFunction*);

  //函数作用： 取得节点的处理函数
  //参数列表：
  //       row 行指针
  //       col 列指针
  //返 回 值： CandidateFunction* 候选式函数指针
  CandidateFunction* GetCFunction(int, int);

  //函数作用： 取得节点的处理函数
  //参数列表：
  //      left 非终结符类型
  //     leave Token类型
  // nilserver epsilon处理器
  //返 回 值： CandidateFunction* 候选式函数指针
  CandidateFunction* GetCFunction(SyntaxType, TokenType, iHandle);

private:
  // 行浮标
  int iRowCount;
  // 列浮标
  int iColCount;
  // 产生式左字典
  SyntaxDictionary iLeftNodes;
  // 产生式右字典
  TokenDictionary iNextLeaves;
  // LL1预测表
  ParserMap iParserMap;
};

CSTORE_NS_END

#endif /* ___CSTORE_DBCLL1SYNTAXPARSERMAP */