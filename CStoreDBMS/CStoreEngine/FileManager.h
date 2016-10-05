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
  // inCounter 读取的数量
  //返 回 值： 操作成功与否
  bool LoadTableBatch(int& times, FILE* fptr, std::string tname, std::vector<int>& colindice, std::vector<std::string>& bufList, std::vector<std::string>& typeList, int totalCol, int& inCounter);

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






  /* OLD FUNCTIONS */
  //bool getMiniOrders(int &times, int* custkeyBuffer, int* orderkeyBuffer, double* totalpriceBuffer, int* shippriorityBuffer);
  //bool getMiniCustomers(int &times, int* custkeyBuffer);
  //int* getOrdersBuffer(int _times, int &_maxcount);
  //void importOrders();
  //void importCustomers();
  void getEXOrdersBuffer(int _times, int &_maxcount);
  void getEXCustkeyBuffer(int _times, int &_maxcount);
  void externSort();
  void innerSort(int* _orgBuffer, int* _syncBuffer, int _maxcount);
  void writeCustkeyOutputBufferToFile(int* _orgBuffer, int _incounter);
  void writeOrdersOutputBufferToFile(int* _orgBuffer, int _incounter);
  void compressCustkey();
  void join();
  void writeJoinedItemToFile(int* _orderBuffer, int* _custBuffer, int _incounter);
  void writeCompressedCustkeyToFile(int* _orgBuffer, int _incounter);
  void quickSort(int _org[], int _sync[], int low, int high);
  void swap(int& a, int& b);
  int partition(int* _org, int *_sync, int _low, int _high);
  int count();
  
  bool insertIntoOrders(int orderkeyBuffer, int custkeyBuffer, double totalpriceBuffer, int shippriorityBuffer);
  bool select_orders_orderkey(int _key);
  int getCustKeyRunLength(int _key);
  int* getCompressedCustkeyBuffer(int _times, int &_maxcount);
  int* getOrderkeyBuffer(int _times, int &_maxcount);
  double* getTotalpriceBuffer(int _times, int &_maxcount);
  int* getShippriorityBuffer(int _times, int &_maxcount);
  void writeTotalpriceOutputBufferToFile(double* _orgBuffer, int _incounter);
  void writeShippriorityOutputBufferToFile(int* _orgBuffer, int _incounter);

private:
  long int pos;
  long int tpos;
  int page[1024];
  
  //int Buffer_custkey[1024];
  //int Buffer_orderkey[1024];
  //double Buffer_totalprice[1024];
  //int Buffer_shippriority[1024];

  DBAllocator* allocator;

  static FileManager* __instance;
  FileManager();
};

CSTORE_NS_END
#endif /* ___CSTORE_FILEMANAGER */