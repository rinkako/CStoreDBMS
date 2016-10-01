#ifndef ___CSTORE_ISYNTAXPARSER
#define ___CSTORE_ISYNTAXPARSER
#include "DBCBase.h"

class ISyntaxParser {
public:
  //函数作用： 启动语法分析器，必须由子类具体实现
  //参数列表： N/A
  //返 回 值： SyntaxTreeNode* 匹配完毕的语法树
  virtual SyntaxTreeNode* Parse() = 0;

  //函数作用： 获取Token流指针
  //参数列表： N/A
  //返 回 值： TokenStream* token流的指针
  virtual TokenStream* GetTokenStream();

  //函数作用： 设置Token流指针
  //参数列表： 
  //  _myProxy Token流指针
  //  _mySVect 句子向量指针
  //返 回 值： N/A
  virtual void SetTokenStream(TokenStream*, StrVec*);

  //函数作用： 获取Sentence向量指针
  //参数列表： N/A
  //返 回 值： StrVec* Sentence向量的指针
  virtual StrVec* GetSectencePtr();

private:
  //函数作用： 设置Sentence向量指针
  //参数列表： 
  //  _myProxy Token流指针
  //返 回 值： N/A
  virtual void SetSentencePtr(StrVec*);

  //函数作用： 复位匹配器
  //参数列表： N/A
  //返 回 值： N/A
  virtual void Reset() = 0;

  // 流指针
  TokenStream* istream;
  // 句指针
  StrVec* isentence;
}; /* ISyntaxParser */

class LL1SyntaxParser : public ISyntaxParser, DBObject {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  LL1SyntaxParser();

  //函数作用： 带token流和sentence向量的构造函数
  //参数列表：
  //  _myProxy Token流指针
  //  _mySVect 句子向量指针
  //返 回 值： N/A
  LL1SyntaxParser(TokenStream*, StrVec*);

  //函数作用： 复位语法匹配器
  //参数列表： N/A
  //返 回 值： N/A
  virtual void Reset();

  //函数作用： 启动语法分析器
  //参数列表： N/A
  //返 回 值： SyntaxTreeNode* 匹配完毕的语法树
  virtual SyntaxTreeNode* Parse();

  //函数作用： 取语法展开式子节点向量
  //参数列表：
  //     _type 展开式类型
  //返 回 值： SyntaxVector 产生式子节点类型向量
  SyntaxVector GetVector(CFunctionType) const;

  //函数作用： 取匹配栈指针
  //参数列表： N/A
  //返 回 值： SyntaxStack* 匹配栈的指针
  SyntaxStack* GetStack();

  //函数作用： 取下一节点的指针
  //参数列表：
  //      _res 父母节点
  //   _parser 匹配器指针
  //返 回 值： SyntaxTreeNode* 下一个拿去展开的产生式
  SyntaxTreeNode* NextNode(SyntaxTreeNode*, LL1SyntaxParser*);

  // 下一Token指针
  iPtr iPTRnextToken = 0;
  // 下一Senctence的指针
  iPtr iPTRnextSectence = 0;

private:
  //函数作用： 递增句子指针
  //参数列表： N/A
  //返 回 值： N/A
  void Dash();

  //函数作用： 处理CSTORE特殊保留字
  //参数列表：
  //    xtoken 下一token
  //   curRoot 当前根节点
  //返 回 值： 是否是CSTORE特有命令
  bool CSTOREQL(Token*, SyntaxTreeNode*&);

  //函数作用： 初始化预测分析表
  //参数列表： N/A
  //返 回 值： N/A
  void InitMap();

  //函数作用： 初始化链接向量
  //参数列表： N/A
  //返 回 值： N/A
  void InitLinkerVector();

  //函数作用： 初始化分析表行列
  //参数列表： N/A
  //返 回 值： N/A
  void InitMapProperties();

  //函数作用： 初始化LL1文法
  //参数列表： N/A
  //返 回 值： N/A
  void InitCellular();

  //函数作用： 错误中断处理函数
  //参数列表： N/A
  //返 回 值： N/A
  void iException();

  // 候选式类型的向量
  SyntaxDict iDict;
  // 匹配栈
  SyntaxStack iParseStack;
  // 预测分析表
  LL1SyntaxParserMap* iMap = NULL;
}; /* LL1SyntaxParser */


#endif /* ___CSTORE_ISYNTAXPARSER */