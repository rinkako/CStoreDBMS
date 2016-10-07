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
    CSCommonUtil::StringBuilder sb(this->GetTypename() + " [Name:" + this->TableName + ", Cols:{");
    if (this->PiList.size() > 0) {
      sb.Append(this->PiList[0] + " " + this->PiTypeList[0]);
      for (int it = 1; it < this->PiList.size(); it++) {
        sb.Append("|" + this->PiList[it] + " " + this->PiTypeList[it]);
      }
    }
    sb.Append("}, Compressed:").Append(this->IsSorted ? "Y" : "N").Append("]");
    return sb.ToString();
  };

  //函数作用： 获取指定列名的类型名
  //参数列表：
  //       col 列名
  //返 回 值： 列的类型名
  std::string GetColType(std::string col) {
    int idx;
    for (idx = 0; idx < this->PiList.size(); idx++) {
      if (this->PiList[idx] == col) {
        break;
      }
    }
    if (idx == this->PiList.size()) {
      TRACE("Exception: expect to get type of col: " + col + " in table: " + this->TableName + " but not exist");
      return "";
    }
    return this->PiTypeList[idx];
  }

  // 表名
  std::string TableName;
  // 原始文件名
  std::string OriginalFileName;
  // 列向量
  std::vector<std::string> PiList;
  // 列类型向量
  std::vector<std::string> PiTypeList;
  // 列名所对应的文件名
  std::map<std::string, std::string> PiFileNameList;
  // 是否已经排序压缩
  bool IsSorted;
  // 排序压缩完后的列名所对应的文件名
  std::map<std::string, std::string> CompressedPiFileNameList;

private:
  // 阻止拷贝构造
  DISALLOW_COPY_AND_ASSIGN(DBTable);
}; /* DBTable */

CSTORE_NS_END

#endif /* ___CSTORE_DBTABLE */