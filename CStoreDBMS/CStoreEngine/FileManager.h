#ifndef ___CSTORE_FILEMANAGER
#define ___CSTORE_FILEMANAGER
#include "DBBase.h"
#include "DBTable.hpp"
#include "DBAllocator.h"
CSTORE_NS_BEGIN

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

#define SIZE_LINE 512
#define SIZE_PAGE 1024

class FileManager {
public:
  //函数作用： 工厂方法，获得类的唯一实例
  //参数列表： N/A
  //返 回 值： 文件管理器的唯一实例
  static FileManager* GetInstance();

  //函数作用： 分批载入表格文件
  //参数列表：
  //     times 当前轮数
  //      fptr 表源文件指针
  // colindice 列下标
  //   bufList 输出的缓冲区名向量
  //  typeList 类型向量
  //  totalCol 总列数
  //    offset 表头偏移量
  // inCounter 读取的数量
  //返 回 值： 操作成功与否
  bool LoadTableBatch(int& times, FILE* fptr, std::string tname, std::vector<int>& colindice, std::vector<std::string>& bufList, std::vector<std::string>& typeList, int totalCol, int offset, int& inCounter);

  //函数作用： 载入表格
  //参数列表：
  //       tab 表对象
  //返 回 值： 操作成功与否
  bool LoadTable(DBTable& tab);
  
  //函数作用： 分批载入列文件
  //参数列表：
  //       tab 表对象
  //   colName 要载入的列名
  //   colType 列类型
  //     times 当前轮数
  // inCounter 读取的数量
  //返 回 值： 操作成功与否
  bool LoadColumnBatch(DBTable& tab, std::string& colName, std::string& colType, int times, int& inCounter, void* buffer);

  //函数作用： 通过主键检索表格
  //参数列表：
  //       tab 表对象
  //       key 主键的值
  //    record 记录对应的字符串
  //返 回 值： 操作成功与否
  bool Retrieve(DBTable& tab, int key, std::string& record);

  //函数作用： 通过偏移量取出对应位置的值的字符串
  //参数列表：
  //       tab 表对象
  //   colName 列名
  //   colType 列类型
  //      page 页码
  //    offset 页偏移
  //    outStr 等价的字符串
  //返 回 值： 操作成功与否
  bool RetrieveValueByOffset(DBTable& tab, std::string colName, std::string colType, int page, int offset, std::string &outStr);

  //函数作用： 获取表记录总数
  //参数列表：
  //       tab 表对象
  //      ccol 参照列的列名
  //返 回 值： 记录总数
  int Count(DBTable&, std::string ccol);

  //函数作用： 自然连接表
  //参数列表：
  //      tab1 表对象
  //      tab2 表对象
  //返 回 值： N/A
  void Join();

  //函数作用： 外排序
  //参数列表：
  //       tab 表对象
  //       col 排序的列
  //      sync 同步变化列
  //返 回 值： N/A
  void ExternSort(DBTable& tab, std::string col, std::string cType, std::string sync, std::string sType);

  //函数作用： 将缓冲区写到文件
  //参数列表：
  // orgBuffer 缓冲区指针
  // incounter 缓冲区长度
  //     fname 文件名
  //返 回 值： N/A
  void WriteBufferToFile(int* _orgBuffer, int _incounter, std::string fname);

  //函数作用： 压缩表
  //参数列表：
  //       tab 表对象
  //       col 压缩的列
  //     cType 列类型
  //返 回 值： N/A
  void Compress(DBTable& tab, std::string col, std::string cType);


private:
  //函数作用： 私有的构造器
  //参数列表： N/A
  //返 回 值： N/A
  FileManager();

  //函数作用： 快速排序
  //参数列表：
  //      _org 原串
  //     _sync 伴随串
  //       low 低位
  //      high 高位
  //返 回 值： N/A
  void quickSort(int _org[], int _sync[], int low, int high);

  //函数作用： 交换两个整形数
  //参数列表：
  //         a 交换变量
  //         b 交换变量
  //返 回 值： N/A
  void swap(int& a, int& b);

  //函数作用： 三数取中分割分区
  //参数列表：
  //      _org 原串
  //     _sync 伴随串
  //      _low 低位
  //     _high 高位
  //返 回 值： 主元下标
  int partition(int* _org, int *_sync, int _low, int _high);

  //函数作用：  内排序
  //参数列表：
  // _orgBuffer 原串
  //_syncBuffer 伴随串
  //  _maxcount 串长度
  //返 回 值：  N/A
  void innerSort(int* _orgBuffer, int* _syncBuffer, int _maxcount);

  // 内存管理器
  DBAllocator* allocator;
  // 唯一实例
  static FileManager* Instance;

};

CSTORE_NS_END
#endif /* ___CSTORE_FILEMANAGER */