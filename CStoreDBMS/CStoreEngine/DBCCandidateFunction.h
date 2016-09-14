#ifndef ___CSTORE_DBCCANDIDATEFUNCTION
#define ___CSTORE_DBCCANDIDATEFUNCTION
#include "DBCDeclaration.h"

class CandidateFunction {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  CandidateFunction();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  CandidateFunction(const CandidateFunction&);

  //函数作用： 带产生式的构造函数
  //参数列表：
  //     _proc 特殊处理的函数指针
  //       _pt 产生式类型
  //返 回 值： N/A
  CandidateFunction(iHandle, CFunctionType);

  //函数作用： 绑定产生式函数和类型
  //参数列表：
  //     _proc 特殊处理的函数指针
  //       _pt 产生式类型
  //返 回 值： N/A
  void SetProc(iHandle, CFunctionType);

  //函数作用： 获得产生式函数
  //参数列表： N/A
  //返 回 值： iHandle 产生式的处理函数
  iHandle GetProc();

  //函数作用： 获得产生式函数
  //参数列表： N/A
  //返 回 值： CFunctionType 产生式类型
  CFunctionType GetType();

  //函数作用： 调用产生式处理函数
  //参数列表：
  //     _root 匹配树根节点
  //   _parser 匹配器指针
  //   _syntax 语法类型
  //   _detail 节点信息
  //返 回 值： iHandle 产生式的处理函数
  SyntaxTreeNode* Call(SyntaxTreeNode*, void*, SyntaxType, istr);

private:
  // 产生式特殊处理指针
  iHandle candidateProcesser = NULL;
  // 产生式类型
  CFunctionType candidateType;
};

#endif /* ___CSTORE_DBCCANDIDATEFUNCTION */