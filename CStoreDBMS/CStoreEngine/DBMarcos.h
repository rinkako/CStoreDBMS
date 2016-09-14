#ifndef ___CSTORE_DBMARCOS
#define ___CSTORE_DBMARCOS

// A macro to disallow the copy constructor and operator= functions 
// This should be used in the priavte:declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  TypeName& operator=(const TypeName&)

#define FZERO 1e-15
#define NEWLINE "\n"
#define COLSTARTNUM 1
#define ROWSTARTNUM 1
#define LL1PARSERMAPROW 69
#define LL1PARSERMAPCOL 35
#define SENTENCETERMINATOR ';'
#define FOREVER while (true)
#define PILEPRINT(strpar) std::cout << strpar
#define PILEPRINTLN(strpar) std::cout << strpar << std::endl

#endif // ___CSTORE_DBMARCOS