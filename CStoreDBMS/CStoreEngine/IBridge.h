#ifndef ___CSTORE_IBRIDGE
#define ___CSTORE_IBRIDGE
#include "DBCBase.h"
#include "ILexicalAnalyzer.h"
#include "ISyntaxParser.h"
#include "IDatabase.h"
#include "IPile.h"

class IBridge {
public:
  //函数作用： 工厂方法
  //参数列表： N/A
  //返 回 值： IBridge* 桥的唯一实例
  static IBridge* Invoke();

  //函数作用： 桥的初始化方法，指定运行方式
  //参数列表：
  //   _myArgc 程序参数列表长度
  //   _myArgv 程序参数列表
  //返 回 值： N/A
  void Init(int, char**);

  //函数作用： 启动解释器
  //参数列表：
  //   isDebug 是否为调试模式
  //返 回 值： N/A
  void StartDash(bool = false);

private:
  //函数作用： 单例模式私有构造函数
  //参数列表： N/A
  //返 回 值： N/A
  IBridge();

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
  // 唯一单例
  static IBridge* iInstance;
}; /* IBridge */

#endif /* ___CSTORE_IBRIDGE */