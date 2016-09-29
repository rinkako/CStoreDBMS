#ifndef ___CSTORE_DBTABLE
#define ___CSTORE_DBTABLE
#include "DBBase.h"
#include <string>
#include <vector>

class DBTable {
public:
  // 表名
  std::string TableName;

  // 原始文件名
  std::string OriginalFileName;

  // 列向量
  std::vector<std::string> PiList;

  // 列名所对应的文件名
  std::vector<std::string> PiFileNameList;

  // 是否已经排序压缩
  bool IsSorted;

  // 排序压缩完后的列名所对应的文件名
  std::vector<std::string> CompressedPiFileNameList;
}; /* DBTable */

#endif /* ___CSTORE_DBTABLE */