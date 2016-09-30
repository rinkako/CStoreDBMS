#ifndef ___CSTORE_DBTABLE
#define ___CSTORE_DBTABLE
#include "DBBase.h"
#include <string>
#include <vector>

class DBTable : public DBObject {
public:
  // 构造器
  CONSTRUCTOR_SIMPLE_DBOBJECT(DBTable, "DBTable");

  // 重载字符串化方法
  virtual std::string ToString() {
    std::string sb = this->GetTypename() + " [Name:" + this->TableName + ", Cols:{";
    if (this->PiList.size() > 0) {
      sb += PiList[0];
      for (int it = 1; it < this->PiList.size(); it++) {
        sb += "|" + PiList[it];
      }
    }
    sb += "}, Compressed:" + this->IsSorted ? "Y" : "N";
    sb += "]";
    return sb;
  };

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