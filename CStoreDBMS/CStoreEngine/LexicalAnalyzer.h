#ifndef ___CSTORE_ILEXICALANALYZER
#define ___CSTORE_ILEXICALANALYZER
#include "DBCBase.h"

CSTORE_NS_BEGIN

class LexicalAnalyzer : public DBObject {
public:
  //函数作用： 构造函数
  //参数列表： N/A 
  //返 回 值： N/A
  LexicalAnalyzer();

  //函数作用： 带源码的构造函数
  //参数列表： 
  //    _src   待处理源代码
  //返 回 值： N/A
  explicit LexicalAnalyzer(const istr&);

  //函数作用： 启动SSQL词法分析DFA自动机
  //参数列表： N/A
  //返 回 值： TokenStream* 对源代码分析完毕的token流
  TokenStream* Analyze();

  //函数作用： 设置待处理源代码
  //参数列表：
  //    _src   源代码字符串
  //返 回 值： N/A
  void SetSourceCode(const istr&);

  //函数作用： 获取源代码
  //参数列表： N/A
  //返 回 值： std::string 源代码的字符串
  istr GetSourceCode();

  //函数作用： 复位词法分析器
  //参数列表： N/A
  //返 回 值： N/A
  void Reset();

  //函数作用： 取得句子容器
  //参数列表： N/A
  //返 回 值： StrVec* 储存着所有源代码的句子的容器指针
  StrVec* GetStrVec();

private:
  //函数作用： 计算指针跳跃和列数修正
  //参数列表：
  //      _go  扫过的距离
  //返 回 值： N/A
  void Jump(int);

  //函数作用： DFA
  //参数列表： N/A
  //返 回 值： Token* 下一个Token的指针
  Token* NextToken();

  //函数作用： 获取器，得到空白对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetSpace(Token*);

  //函数作用： 获取器，得到未知字符对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetUnknown(Token*);

  //函数作用： 获取器，得到标识符对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetIdentifier(Token*);

  //函数作用： 获取器，得到常数对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetConstantNumber(Token*);

  //函数作用： 获取器，得到单字符符号对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetOneCharaCalculator(Token*);

  //函数作用： 获取器，得到可能为双字符对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetTwoCharaCalculator(Token*);

  //函数作用： 获取器，得到保留字对应的Token
  //参数列表：
  //   *result 结果Token输出变量
  //返 回 值： Boolean 是否成功得到了Token
  bool GetReservedCalculator(Token*);

  //函数作用： 判断一个字符属于哪种类型
  //参数列表：
  //        c  带判断字符
  //返 回 值： CharaType 字符c的类型
  CharaType GetCharType(const char&);

  // 源代码字串
  istr sourceCode = "";
  // Token流
  TokenStream* currentList;
  // 行游标
  int currentLine;
  // 列游标
  int currentColumn;
  // 次字符指针
  iPtr PTRnextLetter;
  // 临时句子
  istr TempStr = "";
  // 句子容器
  StrVec sentenceContainer;
}; /* LexicalAnalyzer */

CSTORE_NS_END

#endif /* ___CSTORE_ILEXICALANALYZER */