#ifndef ___CSTORE_DBBRIDGE
#define ___CSTORE_DBBRIDGE
#include "DBCBase.h"
#include "ILexicalAnalyzer.h"
#include "ISyntaxParser.h"
#include "IDatabase.h"
#include "IPile.h"

class DBBridge {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  DBBridge();

  //函数作用： 桥的初始化方法，指定运行方式
  //参数列表：
  //   gbState 运行状态 
  //     query 要执行的查询语句
  //返 回 值： N/A
  void Init(RunType, const istr& = "");

  //函数作用： 启动解释器
  //参数列表：
  //   isDebug 是否为调试模式
  //返 回 值： N/A
  void StartDash(bool = false);

private:
  //函数作用： 从文件读入源代码
  //参数列表： 
  //      path 文件路径
  //返 回 值： istr 包含整个文件代码的字符串
  istr readCode(istr);

  // 词法分析器
  LexicalAnalyzer iLexiana;
  // 语法分析器
  LL1SyntaxParser iParser;
  // 语义分析器
  IPile Pile;
  // 语句解释器
  IDatabase iDB;
  // 源代码单词流
  TokenStream* sourceCodeTokenStream;
  // 当前句子
  TokenList currentSentence;
  // 当前句子单词流
  TokenStream currentSentenceTokenStream;
  // 当前句子语法树
  SyntaxTreeNode* currentTree;
  // 当前代理
  DBCProxy currentProxy;
  // 源代码
  istr sourceCode;
  // 源代码路径
  istr sourcePath;
  // 运行类型
  RunType iType;

  DISALLOW_COPY_AND_ASSIGN(DBBridge);
}; /* DBBridge */

#endif /* ___CSTORE_DBBRIDGE */