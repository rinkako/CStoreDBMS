#ifndef ___CSTORE_DBTABLE
#define ___CSTORE_DBTABLE
#include "DBBase.h"

CSTORE_NS_BEGIN

class DBTable : public DBObject {
public:
  //函数作用： 构造器
  //参数列表：
  //   tabName 表名
  //  fileName 表源文件
  //返 回 值： N/A
  DBTable(const std::string tabName, const std::string fileName)
    :DBObject("DBTable", this) {
    this->IsSorted = false;
    this->TableName = tabName;
    this->OriginalFileName = fileName;
  }

  //函数作用： 重写字符串化方法
  //参数列表： N/A
  //返 回 值： 表的说明
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

private:
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBTable);
}; /* DBTable */

CSTORE_NS_END

#endif /* ___CSTORE_DBTABLE */