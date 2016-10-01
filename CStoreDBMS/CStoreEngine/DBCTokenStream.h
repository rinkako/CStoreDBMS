#ifndef ___CSTORE_DBCTOKENSTREAM
#define ___CSTORE_DBCTOKENSTREAM
#include "DBCDeclaration.h"
#include "DBCToken.h"

CSTORE_NS_BEGIN

class TokenStream : public DBObject {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  TokenStream();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  explicit TokenStream(const TokenStream&);

  //函数作用： 复位流
  //参数列表： N/A
  //返 回 值： N/A
  void Reset();

  //函数作用： 添加到流
  //参数列表：
  //      _org 待添加的token实例
  //返 回 值： N/A
  void Add(Token*);

  //函数作用： 得到流的长度
  //参数列表： N/A
  //返 回 值： int 单词流的长度
  int Length() const;

  //函数作用： 输出至标准输出流
  //参数列表：
  //        os 标准输出流
  //        ts 待输出单词流
  //返 回 值： ios& 标准输出流
  friend ios& operator<<(ios&, const TokenStream&);

  //函数作用： 取得下一个Token的指针
  //参数列表： N/A
  //返 回 值： Token* 下一Token指针
  Token* Next();

  //函数作用： 取得下一个句子的单词向量
  //参数列表： N/A
  //返 回 值： TokenList 下一句子单词向量
  TokenList NextSentence();

  //函数作用： 单词流的文本化函数
  //参数列表： N/A
  //返 回 值： istr 单词流的字符串表示
  istr ToString();

  // 流容器
  TokenList _tokenContainer;

private:
  // 流指针
  iPtr _iPointer;
  // 句指针
  iPtr _sPointer;
}; /* TokenStream */

CSTORE_NS_END

#endif /* ___CSTORE_DBCTOKENSTREAM */