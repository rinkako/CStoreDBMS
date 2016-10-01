#ifndef ___CSTORE_DBMARCOS
#define ___CSTORE_DBMARCOS
#include <stdio.h>
#include <ctime>

// 禁止编译器创造拷贝构造函数和赋值函数
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  TypeName& operator=(const TypeName&)

#define CONSTRUCTOR_SIMPLE_DBOBJECT(Type, TypeName) \
  Type() : DBObject(TypeName, this) { }

// 为输出提供统一接口
#define PILEPRINT(strpar) std::cout << strpar

// 为换行输出提供统一接口
#define PILEPRINTLN(strpar) std::cout << strpar << std::endl

// 为详细跟踪信息提供统一接口
#define TRACE(strpar) { std::string __CSPUREFILE__(__FILE__); \
  int __CSPUREFILEPOS__ = __CSPUREFILE__.rfind('\\'); \
  time_t __CSTRACETIMESTAMP__ = time(NULL); \
  tm* __CSTRACETIMESTAMPPTR__ = localtime(&__CSTRACETIMESTAMP__); \
  printf("[%02d:%02d:%02d %s:%d %s] Trace Info", __CSTRACETIMESTAMPPTR__->tm_hour, __CSTRACETIMESTAMPPTR__->tm_min, __CSTRACETIMESTAMPPTR__->tm_sec, \
  __CSPUREFILE__.substr(__CSPUREFILEPOS__ + 1, __CSPUREFILE__.length() - __CSPUREFILEPOS__ - 1).c_str(), \
  __LINE__, __FUNCTION__); std::cout << std::endl << strpar << std::endl; }

// 永久循环接口
#define FOREVER while (true)

// 浮点数零阈值
#define FZERO 1e-15

// 为换行提供统一符号
#define NEWLINE "\n"

// 为查询语句提供统一的结束符号
#define SENTENCETERMINATOR ';'

// 编译器Parser的文法表起始列号
#define COLSTARTNUM 1

// 编译器Parser的文法表起始行号
#define ROWSTARTNUM 1

// 编译器Parser的文法表行总数
#define LL1PARSERMAPROW 69

// 编译器Parser的文法表列总数
#define LL1PARSERMAPCOL 35

#endif /* ___CSTORE_DBMARCOS */