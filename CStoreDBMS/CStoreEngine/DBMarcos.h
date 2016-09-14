#ifndef __CSTORE_DBMARCOS
#define __CSTORE_DBMARCOS

// A macro to disallow the copy constructor and operator= functions 
// This should be used in the priavte:declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  TypeName& operator=(const TypeName&)

#endif // __CSTORE_DBMARCOS