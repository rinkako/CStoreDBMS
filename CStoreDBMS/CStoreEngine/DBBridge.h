#ifndef ___CSTORE_DBBRIDGE
#define ___CSTORE_DBBRIDGE
#include "DBCBase.h"
#include "LexicalAnalyzer.h"
#include "ISyntaxParser.h"
#include "CSDatabase.h"
#include "Pile.h"

CSTORE_NS_BEGIN

class DBBridge : public DBObject {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  DBBridge();

  //函数作用： 启动解释器
  //参数列表：
  //     trans 事务
  //   isDebug 是否为调试模式
  //返 回 值： N/A
  void StartTransaction(DBTransaction&, bool = false);

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
  CSDatabase iDB;
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
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBBridge);
}; /* DBBridge */

CSTORE_NS_END

#endif /* ___CSTORE_DBBRIDGE */