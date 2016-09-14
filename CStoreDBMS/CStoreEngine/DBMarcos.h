#ifndef ___CSTORE_DBMARCOS
#define ___CSTORE_DBMARCOS

// 禁止编译器创造拷贝构造函数和赋值函数
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  TypeName& operator=(const TypeName&)

// 为输出提供统一接口
#define PILEPRINT(strpar) std::cout << strpar

// 为换行输出提供统一接口
#define PILEPRINTLN(strpar) std::cout << strpar << std::endl

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