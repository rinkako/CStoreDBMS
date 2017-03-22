#ifndef ___CSTORE_DBCSYNTAXTREENODE
#define ___CSTORE_DBCSYNTAXTREENODE
#include "DBCBase.h"

CSTORE_NS_BEGIN

class SyntaxTreeNode : public DBObject {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  SyntaxTreeNode();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  explicit SyntaxTreeNode(const SyntaxTreeNode&);

  //函数作用： 树的文本化函数
  //参数列表： N/A
  //返 回 值： istr 树的字符串表示
  virtual std::string ToString();

  // 绑定处理函数
  CandidateFunction* candidateFunction;
  // 子树向量
  SyntaxTreeList children;
  // 父指针
  SyntaxTreeNode* parent;
  // 命中语法结构类型
  SyntaxType nodeSyntaxType;
  // 命中token附加值
  istr nodeValue;
  // 命中产生式类型
  CFunctionType nodeType;
  // 节点名字
  istr nodeName;
  // 附加值
  double aTag;
  // 错误位
  int errorCode;
  // 错误码
  istr errorInfo;
  // 命中Token所在流
  TokenStream* mappedList;
  // 命中Token位置戳
  int mappedBegin;
  // 命中Token长度
  int mappedLength;

private:
  //函数作用： 树的递归遍历文本化
  //参数列表： N/A
  //返 回 值： istr 树的字符串表示
  void GetTree(istr&, SyntaxTreeNode*, int&) const;

  //函数作用： 树文本化过程的缩进计算
  //参数列表： N/A
  //返 回 值： istr 树的字符串表示
  istr DrawTree(SyntaxTreeNode*) const;
}; /* SyntaxTreeNode */

CSTORE_NS_END

#endif /* ___CSTORE_DBCSYNTAXTREENODE */