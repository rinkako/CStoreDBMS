#include "FileManager.h"
#include <limits.h>
#include <string.h>
#include <string>
#include <stdio.h>

CSTORE_NS_BEGIN
#define COMPRESS_SIZE (64 * SIZE_PAGE)
#define EXSORT_SIZE (64 * SIZE_PAGE)
#define COLUMNBATCHSIZE (64 * SIZE_PAGE)
using namespace std;

int externSortOrdersBufferPool[64 * SIZE_PAGE];
int externSortCustkeyBufferPool[64 * SIZE_PAGE];
int compressedCustkeyBufferPool[64 * SIZE_PAGE];

int ordersOrderkeyBufferPool[64 * SIZE_PAGE];
double ordersTotalpriceBufferPool[64 * SIZE_PAGE];
int ordersShippriorityBufferPool[64 * SIZE_PAGE];
int pagecounter = 0;
int importcountercustomer = 0;

int page_ptr = 0;
int count_spilt = 0;
int maxcount = 0;
char page_buf[SIZE_PAGE];

//函数作用： 分批载入表格文件
//参数列表：
//     times 当前轮数
//    piList 要载入的列名向量
//   bufList 输出的缓冲区名向量
//返 回 值： N/A
bool FileManager::LoadTableBatch(int& times, FILE* fptr, std::string tname, std::vector<int>& colindice,
  std::vector<std::string>& bufList, std::vector<std::string>& typeList, int totalCol, int& inCounter) {
  int count_reco = 0;
  int content_ptr = 0;
  char* content_buf = (char*)this->allocator->Alloc("content_buf_" + tname, sizeof(char)* 128);
  while (count_reco < SIZE_PAGE) {
    // page_buf被读完
    if (page_ptr == maxcount) {
      std::fseek(fptr, times++ * SIZE_PAGE * sizeof(char), SEEK_SET);
      maxcount = std::fread(page_buf, sizeof(char), SIZE_PAGE, fptr);
      page_ptr = 0;
    }
    // 已经不能为page_buf读到任何数据
    if (maxcount == 0) {
      break;
    }
    // 分析字符，转换并存入相应缓冲区
    if (page_buf[page_ptr] != '|' && page_buf[page_ptr] != '\n') {
      content_buf[content_ptr++] = page_buf[page_ptr];
    }
    else {
      count_spilt++;
      content_buf[content_ptr] = '\0';
      if (count_spilt == totalCol) {
        count_reco++;
        count_spilt = -1;
      }
      else {
        for (int ci = 0; ci < colindice.size(); ci++) {
          if (count_spilt == colindice[ci]) {
            if (typeList[ci] == "DOUBLE") {
              ((double*)this->allocator->Get(bufList[ci]))[count_reco] = std::atof(content_buf);
              break;
            }
            else {
              ((int*)this->allocator->Get(bufList[ci]))[count_reco] = std::atoi(content_buf);
              break;
            }
          }
        }
      }
      content_ptr = 0;
    }
    page_ptr++;
  }
  // 输出读入的记录数目
  inCounter = count_reco;
  this->allocator->Free("content_buf_" + tname);
  return maxcount == 0;
}

//函数作用： 载入表格
//参数列表：
//       tab 表对象
//返 回 值： 操作成功与否
bool FileManager::LoadTable(DBTable& tab) {
  // 打开读入文件
  std::string filePath = tab.TableName + ".tbl";
  FILE* fin = fopen(filePath.c_str(), "rb");
  if (fin == NULL) {
    fin = fopen(("../" + filePath).c_str(), "rb");
  }
  if (fin == NULL) {
    TRACE("Exception: cannot load table from file: " + tab.TableName);
    return false;
  }
  // 读表头分析列位置
  char Header[SIZE_PAGE];
  fgets(Header, SIZE_PAGE, fin);
  std::string strHeader(Header);
  std::vector<std::string> hItems = CSCommonUtil::CStrSplit(strHeader, "|");
  std::vector<int> colIndices;
  for (int j = 0; j < tab.PiList.size(); j++) {
    bool existFlag = false;
    for (int i = 0; i < hItems.size(); i++) {
      if (tab.PiList[j] == hItems[i]) {
        colIndices.push_back(i);
        existFlag = true;
        break;
      }
    }
    if (existFlag == false) {
      TRACE("Column not exist in file: " + tab.PiList[j]);
      return false;
    }
  }
  // 构造列存储文件名并清掉已有文件
  for (int i = 0; i < tab.PiList.size(); i++) {
    tab.PiFileNameList[tab.PiList[i]] = tab.TableName + "_" + tab.PiList[i] + ".db";
    FILE* fdel = std::fopen(tab.PiFileNameList[tab.PiList[i]].c_str(), "w");
    std::fclose(fdel);
  }
  // 为列们开辟文件句柄
  FILE** fColPtrArr = new FILE*[tab.PiList.size()];
  for (int i = 0; i < tab.PiList.size(); i++) {
    fColPtrArr[i] = std::fopen(tab.PiFileNameList[tab.PiList[i]].c_str(), "ab");
  }
  // 分配缓冲区
  std::vector<std::string> bufferNameVec;
  for (int i = 0; i < tab.PiList.size(); i++) {
    std::string bufName = tab.TableName + "@" + tab.PiList[i];
    bufferNameVec.push_back(bufName);
    if (tab.PiTypeList[i] == "DOUBLE") {
      this->allocator->Alloc(bufName, SIZE_PAGE * sizeof(double));
    }
    else {
      this->allocator->Alloc(bufName, SIZE_PAGE * sizeof(int));
    }
  }
  // 分页读入
  bool finflag = false;
  int pCounter = 0;
  int inCounter = 0;
  do {
    // 不断地读入文件到缓冲区
    finflag = this->LoadTableBatch(pCounter, fin, tab.TableName, colIndices, bufferNameVec, tab.PiTypeList, hItems.size(), inCounter);
    // 将缓冲区内容写到文件
    for (int i = 0; i < tab.PiFileNameList.size(); i++) {
      if (tab.PiTypeList[i] == "DOUBLE") {
        std::fwrite(this->allocator->Get(bufferNameVec[i]), sizeof(double), inCounter, fColPtrArr[i]);
      } else {
        std::fwrite(this->allocator->Get(bufferNameVec[i]), sizeof(int), inCounter, fColPtrArr[i]);
      }
    }
  } while (finflag != true);
  // 保存文件并释放资源
  for (int i = 0; i < tab.PiList.size(); i++) {
    std::fclose(fColPtrArr[i]);
    if (fColPtrArr[i] != NULL) {
      delete fColPtrArr[i];
      fColPtrArr[i] = NULL;
    }
  }
  if (fColPtrArr != NULL) {
    delete fColPtrArr;
    fColPtrArr = NULL;
  }
  std::fclose(fin);
  for (int i = 0; i < bufferNameVec.size(); i++) {
    this->allocator->Free(bufferNameVec[i]);
  }
}

//函数作用： 通过主键检索表格
//参数列表：
//       tab 表对象
//       key 主键的值
//    record 记录对应的字符串
//返 回 值： 操作成功与否
bool FileManager::Retrieve(DBTable& tab, int key, std::string& record) {
  // 先搜索主键
  bool _successFlag = false;
  bool _failFlag = false;
  std::string buffername = "retrieve_buffer_" + tab.TableName;
  int* bufferPtr = (int*)this->allocator->Alloc(buffername, COLUMNBATCHSIZE * sizeof(int));
  int pcounter = 0;
  int sindex = 0;
  int _pc = 0; // 页码
  int _si = 0; // 页偏移
  int maxcount = COLUMNBATCHSIZE;
  // 分页读入
  do {
    this->LoadColumnBatch(tab, tab.PiList[0], tab.PiTypeList[0], pcounter++, maxcount, bufferPtr);
    if (maxcount == 0) {
      _failFlag = true;
      break;
    }
    if (bufferPtr[maxcount - 1] < key) {
      continue;
    }
    if (bufferPtr[0] > key) {
      _failFlag = true;
      break;
    }
    // 二分查找
    int mid = 0, low = 0, high = maxcount - 1;
    while (low <= high) {
      mid = (low + high) / 2;
      if (bufferPtr[mid] == key) {
        sindex = mid;
        _successFlag = true;
        break;
      }
      else if (bufferPtr[mid] > key) {
        high = mid - 1;
      }
      else if (bufferPtr[mid] < key) {
        low = mid + 1;
      }
    }

  } while (_successFlag != true);
  // 释放资源
  this->allocator->Free(buffername);
  // 处理结果，返回给Service
  if (_failFlag == true) {
    record = "";
    return false;
  }
  else {
    _pc = pcounter - 1;
    _si = sindex;
  }
  // 再检索其他列
  CSCommonUtil::StringBuilder recordSb;
  for (int i = 1; i < tab.PiList.size(); i++) {
    std::string outstr = "";
    this->RetrieveValueByOffset(tab, tab.PiList[i], tab.PiTypeList[i], _pc, _si, outstr);
    recordSb.Append(outstr);
  }
  // 返回她
  record = recordSb.ToString();
  return true;
}

//函数作用： 分批载入列文件
//参数列表：
//       tab 表对象
//   colName 要载入的列名
//   colType 列类型
//     times 当前轮数
// inCounter 读取的数量
//返 回 值： 操作成功与否
bool FileManager::LoadColumnBatch(DBTable& tab, std::string& colName, std::string& colType, int times, int& inCounter, void* buffer) {
  FILE* fin = std::fopen((tab.PiFileNameList[colName]).c_str(), "rb");
  if (fin == NULL) {
    TRACE("Exception: cannot load column file: " + tab.PiFileNameList[colName]);
    return false;
  }
  std::fseek(fin, (long)(times * COLUMNBATCHSIZE * sizeof(int)), SEEK_SET);
  if (fin == 0) {
    return false;
  }
  if (colType == "DOUBLE") {
    inCounter = std::fread(buffer, sizeof(double), COLUMNBATCHSIZE, fin);
  }
  else {
    inCounter = std::fread(buffer, sizeof(int), COLUMNBATCHSIZE, fin);
  }
  std::fclose(fin);
  return true;
}

//函数作用： 通过偏移量取出对应位置的值的字符串
//参数列表：
//       tab 表对象
//   colName 列名
//      page 页码
//    offset 页偏移
//    outStr 等价的字符串
//返 回 值： 操作成功与否
bool FileManager::RetrieveValueByOffset(DBTable& tab, std::string colName, std::string colType, int page, int offset, std::string &outStr) {
  FILE* fin = std::fopen((tab.PiFileNameList[colName]).c_str(), "rb");
  if (fin == NULL) {
    TRACE("cannot load column file: " + tab.PiFileNameList[colName]);
    return false;
  }
  if (colType == "DOUBLE") {
    double tdouble = 0;
    std::fseek(fin, (long)((page * COLUMNBATCHSIZE + offset) * sizeof(double)), SEEK_SET);
    std::fread(&tdouble, sizeof(double), 1, fin);
    outStr = CSCommonUtil::StringBuilder().Append(tdouble).ToString();
    return true;
  }
  else {
    int tint = 0;
    std::fseek(fin, (long)((page * COLUMNBATCHSIZE + offset) * sizeof(int)), SEEK_SET);
    std::fread(&tint, sizeof(int), 1, fin);
    outStr = CSCommonUtil::StringBuilder().Append(tint).ToString();
    return true;
  }
  return false;
}

//函数作用： 外排序
//参数列表：
//       tab 表对象
//       col 排序的列
//返 回 值： N/A
void FileManager::externSort() {
  int maxcount = 1024;
  int pc = 0;
  // 形成序列
  while (maxcount > 0) {
    // 每次读入128页
    getEXOrdersBuffer(pc, maxcount);
    getEXCustkeyBuffer(pc, maxcount);
    if (maxcount == 0) break;
    // 然后关于custkey列做内排序
    innerSort(externSortCustkeyBufferPool, externSortOrdersBufferPool, maxcount);
    // 内排序完毕后写临时文件
    char strBuffer[10];
    std::string outFileName = "temp_exSortTempCustkey";
    sprintf(strBuffer, "%d", pagecounter);
    outFileName += strBuffer;
    FILE* fout = fopen(outFileName.c_str(), "wb");
    fwrite(externSortCustkeyBufferPool, sizeof(int), maxcount, fout);
    fclose(fout);
    outFileName = "temp_exSortTempOrders";
    outFileName += strBuffer;
    fout = fopen(outFileName.c_str(), "wb");
    fwrite(externSortOrdersBufferPool, sizeof(int), maxcount, fout);
    fclose(fout);
    pagecounter++;
    pc++;
  }
  // 多路归并
  int min = INT_MAX - 1;
  int minindex = -1;
  int outputBuffer[SIZE_PAGE];
  int syncBuffer[SIZE_PAGE];
  int outputPointer = 0;
  int mergeBufferCapacity = (int)((126.0f * SIZE_PAGE / (double)(2.0 * pagecounter)));
  int freadflag = 0;
  string inFileName;
  int* mergePointer = new int[pagecounter];
  int* mergeTimes = new int[pagecounter];
  for (int i = 0; i < pagecounter; i++) {
    mergePointer[i] = mergeBufferCapacity;
    mergeTimes[i] = 0;
  }
  int** mergeBuffer = new int*[2 * pagecounter];
  for (int i = 0; i < 2 * pagecounter; i++) {
    mergeBuffer[i] = new int[mergeBufferCapacity];
  }
  while (true) {
    // 先看看是否需要重入缓冲区，因为是同步的，所以2倍pagecounter后半部分不需要去迭代她
    for (int i = 0; i < pagecounter; i++) {
      // 如果这个缓冲区读完了，那么就要重新载入并定位指针
      if (mergePointer[i] == mergeBufferCapacity) {
        memset(mergeBuffer[i], 0, mergeBufferCapacity * sizeof(int));
        // 构造文件名
        char fstrBuffer[10];
        inFileName = "temp_exSortTempCustkey";
        sprintf(fstrBuffer, "%d", i);
        inFileName += fstrBuffer;
        // 继续把文件剩余部分放入缓冲区
        FILE* fin = fopen(inFileName.c_str(), "rb");
        fseek(fin, (long)(mergeTimes[i] * mergeBufferCapacity * sizeof(int)), SEEK_SET);
        freadflag = fread(mergeBuffer[i], sizeof(int), mergeBufferCapacity, fin);
        fclose(fin);
        // 再次构造文件名，这次是对orderkey进行的
        inFileName = "temp_exSortTempOrders";
        sprintf(fstrBuffer, "%d", i);
        inFileName += fstrBuffer;
        // 再次读文件并放缓冲区
        fin = fopen(inFileName.c_str(), "rb");
        fseek(fin, (long)(mergeTimes[i] * mergeBufferCapacity * sizeof(int)), SEEK_SET);
        freadflag = fread(mergeBuffer[pagecounter + i], sizeof(int), mergeBufferCapacity, fin);
        fclose(fin);
        // 回归扫描指针，递增页面记录指针
        mergePointer[i] = 0;
        mergeTimes[i]++;
      }
    }
    // 关于custkey进行，所以一倍pagecounter之后的页面是不需要迭代的
    for (int i = 0; i < pagecounter; i++) {
      // 如果还有数据，则比较数据
      if (mergeBuffer[i][mergePointer[i]] < min && mergeBuffer[i][mergePointer[i]] != 0) {
        // 对custkey而言，她需要去比较每个游标的位置，挑选最小并放入缓冲区
        min = mergeBuffer[i][mergePointer[i]];
        minindex = i;
      }
    }
    // 如果最后一轮完成了但是缓冲区还有东西就输出
    if (minindex == -1) {
      if (outputPointer > 0) {
        writeCustkeyOutputBufferToFile(outputBuffer, outputPointer);
        writeOrdersOutputBufferToFile(syncBuffer, outputPointer);
        outputPointer = 0;
      }
      break;
    }
    // 每一轮完成后，挑出最小的写入缓冲区，这时候二倍pagecounter处的order列也要同步写入
    outputBuffer[outputPointer] = min;
    // 对orderkey而言，她只需要跟着custkey的游标进行就好了
    syncBuffer[outputPointer++] = mergeBuffer[pagecounter + minindex][mergePointer[minindex]++];
    // 如果缓冲区写满了，那么输出到文件里并清空缓冲区
    if (outputPointer == SIZE_PAGE || (freadflag == 0 && outputPointer > 0)) {
      writeCustkeyOutputBufferToFile(outputBuffer, outputPointer);
      writeOrdersOutputBufferToFile(syncBuffer, outputPointer);
      outputPointer = 0;
    }
    // 如果已经彻底读完了就出来，否则重置临时变量
    if (freadflag == 0) {
      break;
    }
    else {
      min = INT_MAX - 1;
      minindex = -1;
    }
  }
  // 压缩
  compressCustkey();
}

//函数作用： 压缩表
//参数列表：
//       tab 表对象
//       col 压缩的列
//返 回 值： N/A
void FileManager::compressCustkey() {
  int compressBuffer[COMPRESS_SIZE];
  int outputBuffer[COMPRESS_SIZE];
  int frontPointer = 0;
  int rearPointer = 0;
  int currentValue = 0;
  int outputPointer = 0;
  int maxcount = 0;
  int ftimes = 0;
  int lastValue = 0;
  int lastLength = 0;
  bool firstRound = true;
  bool finishFlag = false;
  FILE* fin = fopen("custkey_sorted.db", "rb");
  while (true) {
    memset(compressBuffer, 0, sizeof(int)* COMPRESS_SIZE);
    maxcount = fread(compressBuffer, sizeof(int), COMPRESS_SIZE, fin);
    firstRound = true;
    if (maxcount == 0 && outputPointer == 0) {
      break;
    }
    if (maxcount > 0) {
      currentValue = compressBuffer[0];
      // 如果lastValue与currentValue已经不一一样了，那么直接追加
      if (currentValue != lastValue && lastValue != 0) {
        outputBuffer[outputPointer++] = lastValue;
        outputBuffer[outputPointer++] = lastLength;
        lastLength = 0;
        lastValue = 0;
        firstRound = false;
        if (outputPointer == COMPRESS_SIZE) {
          writeCompressedCustkeyToFile(outputBuffer, COMPRESS_SIZE);
          outputPointer = 0;
        }
      }
    }
    while (1) {
      // 如果值改变了，那么重新定位指针位置
      if (currentValue != compressBuffer[rearPointer]) {
        outputBuffer[outputPointer++] = currentValue;
        // 如果这是第一次扫描这个缓冲区那要考虑将上一次末尾的值追加到行程长度里
        if (firstRound == true) {
          outputBuffer[outputPointer++] = rearPointer - frontPointer + lastLength;
          lastLength = 0;
          lastValue = 0;
          firstRound = false;
        }
        else
        {
          outputBuffer[outputPointer++] = rearPointer - frontPointer;
        }
        // 移动缓冲区指针
        currentValue = compressBuffer[rearPointer];
        frontPointer = rearPointer;
        rearPointer = rearPointer;
      }
      // 如果指针到达了末尾，则把这段末尾连续数据信息记录起来然后清空缓存区
      else if (rearPointer == COMPRESS_SIZE - 1) {
        lastValue = currentValue;
        lastLength = rearPointer - frontPointer + 1;
        frontPointer = 0;
        rearPointer = 0;
        break;
      }
      // 如果后面已经没有数据了就预备结束
      else if (compressBuffer[rearPointer] == 0) {
        finishFlag = true;
        break;
      }
      // 否则移动指针
      else {
        rearPointer++;
      }
      // 如果缓冲区满了，那么把缓冲区写到文件
      if (outputPointer == COMPRESS_SIZE) {
        writeCompressedCustkeyToFile(outputBuffer, COMPRESS_SIZE);
        outputPointer = 0;
      }
      // 如果这是最后一轮但是缓冲区还有数据那就写出去
      else if (maxcount == 0 && outputPointer > 0) {
        writeCompressedCustkeyToFile(outputBuffer, outputPointer);
        outputPointer = 0;
        finishFlag = true;
      }
      // 如果已经没数据就出来
      else if (maxcount == 0) {
        finishFlag = true;
        break;
      }
    }
    // 看看能否安全地退出
    if (finishFlag == true) {
      if (outputPointer > 0) {
        writeCompressedCustkeyToFile(outputBuffer, outputPointer);
        outputPointer = 0;
      }
      break;
    }
    // 重定位文件指针
    fseek(fin, (long)(COMPRESS_SIZE * (++ftimes) * sizeof(int)), SEEK_SET);
  }
}

//函数作用： 自然连接表
//参数列表：
//      tab1 表对象
//      tab2 表对象
//返 回 值： N/A
void FileManager::join() {
  // 缓冲区
  int joinCustkeyBuffer[32 * SIZE_PAGE];
  int joinCustomerBuffer[32 * SIZE_PAGE];
  int joinOutputOrderkeyBuffer[32 * SIZE_PAGE];
  int joinOutputCustkeyBuffer[32 * SIZE_PAGE];
  // 缓冲区指针
  int joinCustkeyBufferPointer = 0;
  int joinCustomerBufferPointer = 0;
  int joinOutputPointer = 0;
  // 文件指针
  FILE* joinFileOrderkey = fopen("orderkey_sorted.db", "rb");
  FILE* joinFileCustkey = fopen("custkey_compressed.db", "rb");
  FILE* joinFileCustomer = fopen("customer_custkey.db", "rb");
  // 计数变量
  int custkeyMaxcount = 0;
  int customerMaxcount = 0;
  int custkeyTime = 0;
  int customerTime = 0;
  long int offsetLength = 0;
  // 标识变量
  bool custkeyFinishFlag = false;
  bool customerFinishFlag = false;
  // 读入第一组数据
  custkeyMaxcount = fread(joinCustkeyBuffer, sizeof(int), 32 * SIZE_PAGE, joinFileCustkey);
  customerMaxcount = fread(joinCustomerBuffer, sizeof(int), 32 * SIZE_PAGE, joinFileCustomer);
  while (true) {
    // 如果当前指针指向两边是对等的，那么读入这个对应数据的orderkey写输出缓冲区
    if (joinCustkeyBuffer[joinCustkeyBufferPointer] != 0 && joinCustomerBuffer[joinCustomerBufferPointer] != 0) {
      if (joinCustkeyBuffer[joinCustkeyBufferPointer] == joinCustomerBuffer[joinCustomerBufferPointer]) {
        // 记录游程
        int value = joinCustkeyBuffer[joinCustkeyBufferPointer++];
        int length = joinCustkeyBuffer[joinCustkeyBufferPointer++];
        // 写到输出缓冲区
        for (int _wi = 0; _wi < length; _wi++) {
          joinOutputCustkeyBuffer[joinOutputPointer] = value;
          fseek(joinFileOrderkey, (offsetLength + _wi) * sizeof(int), SEEK_SET);
          fread(joinOutputOrderkeyBuffer + joinOutputPointer, sizeof(int), 1, joinFileOrderkey);
          joinOutputPointer++;
          // 如果输出缓冲区满，那么写文件并清缓存区
          if (joinOutputPointer == 32 * SIZE_PAGE) {
            writeJoinedItemToFile(joinOutputOrderkeyBuffer, joinOutputCustkeyBuffer, joinOutputPointer);
            joinOutputPointer = 0;
          }
        }
        // 跳游程
        offsetLength += length;
      }
      // 否则，移动那个小的指针向下
      else if (joinCustkeyBuffer[joinCustkeyBufferPointer] > joinCustomerBuffer[joinCustomerBufferPointer]) {
        // 这时customer比较小，所以只需要移动一格
        joinCustomerBufferPointer += 1;
      }
      else if (joinCustkeyBuffer[joinCustkeyBufferPointer] < joinCustomerBuffer[joinCustomerBufferPointer]) {
        // 跳游程
        offsetLength += joinCustkeyBuffer[joinCustkeyBufferPointer + 1];
        // 这时custkey比较小，所以必须移动两格来越过游程量
        joinCustkeyBufferPointer += 2;
      }
    }
    // 如果已经有指针到达了0的位置，那么没有继续进行的意义了
    else {
      break;
    }
    // 如果有哪个缓冲区到达边界，那么就要重入
    if (joinOutputPointer == 32 * SIZE_PAGE) {
      writeJoinedItemToFile(joinOutputOrderkeyBuffer, joinOutputCustkeyBuffer, joinOutputPointer);
      joinOutputPointer = 0;
    }
    if (joinCustkeyBufferPointer == 32 * SIZE_PAGE) {
      memset(joinCustkeyBuffer, 0, 32 * SIZE_PAGE * sizeof(int));
      fseek(joinFileCustkey, (long)(++custkeyTime * 32 * SIZE_PAGE * sizeof(int)), SEEK_SET);
      custkeyMaxcount = fread(joinCustkeyBuffer, sizeof(int), 32 * SIZE_PAGE, joinFileCustkey);
      joinCustkeyBufferPointer = 0;
      if (custkeyMaxcount < 32 * SIZE_PAGE) {
        custkeyFinishFlag = true;
      }
    }
    if (joinCustomerBufferPointer == 32 * SIZE_PAGE) {
      memset(joinCustomerBuffer, 0, 32 * SIZE_PAGE * sizeof(int));
      fseek(joinFileCustomer, (long)(++customerTime * 32 * SIZE_PAGE * sizeof(int)), SEEK_SET);
      customerMaxcount = fread(joinCustomerBuffer, sizeof(int), 32 * SIZE_PAGE, joinFileCustomer);
      joinCustomerBufferPointer = 0;
      if (customerMaxcount < 32 * SIZE_PAGE) {
        customerFinishFlag = true;
      }
    }
    // 如果做完了但缓冲区还有东西那就清空，否则退出
    if ((custkeyMaxcount == 0 || customerMaxcount == 0) && (joinOutputPointer > 0)) {
      writeJoinedItemToFile(joinOutputOrderkeyBuffer, joinOutputCustkeyBuffer, joinOutputPointer);
      joinOutputPointer = 0;
    }
    else if ((custkeyMaxcount == 0 || customerMaxcount == 0) && (joinOutputPointer == 0)) {
      break;
    }

  }
  // 确认安全地退出
  if (joinOutputPointer > 0) {
    writeJoinedItemToFile(joinOutputOrderkeyBuffer, joinOutputCustkeyBuffer, joinOutputPointer);
    joinOutputPointer = 0;
  }
  // 保存文件
  fclose(joinFileOrderkey);
  fclose(joinFileCustkey);
  fclose(joinFileCustomer);
}

//函数作用： 写出连接后的表
//参数列表：
//   bufList 缓冲区名向量
//_incounter 最大数量
//返 回 值： N/A
void FileManager::writeJoinedItemToFile(int* _orderBuffer, int* _custBuffer, int _incounter) {
  //FILE* joinOutOrderkey = fopen("joined_orderkey.db", "ab");
  //FILE* joinOutCustkey = fopen("joined_custkey.db", "ab");
  //fwrite(_orderBuffer, sizeof(int), _incounter, joinOutOrderkey);
  //fwrite(_custBuffer, sizeof(int), _incounter, joinOutCustkey);
  //fclose(joinOutOrderkey);
  //fclose(joinOutCustkey);
  for (int i = 0; i < _incounter; i++)
  {
    std::cout << _custBuffer[i] << " " << _orderBuffer[i] << std::endl;
  }
}

//函数作用： 获取游程编码后表记录总数
//参数列表：
//       tab 表对象
//返 回 值： 记录总数
int FileManager::count() {
  FILE* fin = fopen("custkey_compressed.db", "rb");
  fseek(fin, 0L, SEEK_SET);
  int ctr = 0;
  int atr = 0;
  int buf[1024];
  int it = 0;
  do {
    memset(buf, 0, sizeof(int)* 1024);
    ctr = fread(buf, sizeof(int), 1024, fin);
    for (int j = 0; j < ctr; j++) {
      if (j % 2 == 1) {
        atr += buf[j];
      }
    }
    fseek(fin, (long)(sizeof(int)* 1024 * ++it), SEEK_SET);
  } while (ctr != 0);
  return atr;
}

int* FileManager::getOrderkeyBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("orderkey_sorted.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(ordersOrderkeyBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return ordersOrderkeyBufferPool;
}

double *FileManager::getTotalpriceBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("totalprice_sorted.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(ordersTotalpriceBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return ordersTotalpriceBufferPool;
}

int* FileManager::getShippriorityBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("shippriority_sorted.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(ordersShippriorityBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return ordersShippriorityBufferPool;
}

void FileManager::getEXOrdersBuffer(int _times, int &_maxcount) {
  FILE* fin = fopen("orders_orderkey.db", "rb");
  fseek(fin, (long)(_times * 64 * SIZE_PAGE * sizeof(int)), SEEK_SET);
  if (fin == 0) return;
  _maxcount = fread(externSortOrdersBufferPool, sizeof(int), 64 * SIZE_PAGE, fin);
  fclose(fin);
}

void FileManager::getEXCustkeyBuffer(int _times, int &_maxcount) {
  FILE* fin = fopen("orders_custkey.db", "rb");
  fseek(fin, (long)(_times * 64 * SIZE_PAGE * sizeof(int)), SEEK_SET);
  if (fin == 0) return;
  _maxcount = fread(externSortCustkeyBufferPool, sizeof(int), 64 * SIZE_PAGE, fin);
  fclose(fin);
}

int* FileManager::getCompressedCustkeyBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("custkey_compressed.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(compressedCustkeyBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return compressedCustkeyBufferPool;
}

void writeCompressedCustkeyToTemp(int* _orgBuffer, int _incounter) {
	FILE* fout = fopen("custkey_compressed_temp.db", "ab");
	fwrite(_orgBuffer, sizeof(int), _incounter, fout);
	fclose(fout);
}

int* getCompressedCustkeyTempBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("custkey_compressed_temp.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(compressedCustkeyBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return compressedCustkeyBufferPool;
}

void writeOrderkeyToTemp(int* _orgBuffer, int _incounter) {
	FILE* fout = fopen("orders_orderkey_temp.db", "ab");
	fwrite(_orgBuffer, sizeof(int), _incounter, fout);
	fclose(fout);
}

int* getOrderkeyTempBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("orders_orderkey_temp.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(ordersOrderkeyBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return ordersOrderkeyBufferPool;
}

void writeTotalpriceToTemp(double* _orgBuffer, int _incounter) {
	FILE* fout = fopen("orders_totalprice_temp.db", "ab");
	fwrite(_orgBuffer, sizeof(double), _incounter, fout);
	fclose(fout);
}

double* getTotalpriceTempBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("orders_totalprice_temp.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(ordersTotalpriceBufferPool, sizeof(double), 65536, fin);
	fclose(fin);
	return ordersTotalpriceBufferPool;
}

void writeShippriorityToTemp(int* _orgBuffer, int _incounter) {
	FILE* fout = fopen("orders_shippriority_temp.db", "ab");
	fwrite(_orgBuffer, sizeof(int), _incounter, fout);
	fclose(fout);
}

int* getShippriorityTempBuffer(int _times, int &_maxcount) {
	FILE* fin = fopen("orders_shippriority_temp.db", "rb");
	if (fin == NULL) return NULL;
	fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
	if (fin == 0) {
		return NULL;
	}
	_maxcount = fread(ordersShippriorityBufferPool, sizeof(int), 65536, fin);
	fclose(fin);
	return ordersShippriorityBufferPool;
}

int FileManager::getCustKeyRunLength(int _key) {
	bool _successFlag = false;
	bool _failFlag = false;
	int* bufferPtr = NULL;
	int* tempPtr = NULL;
	int pcounter = 0;
	int tempPcounter = 0;
	int iindex = 0;
	int maxcount = 65536;
	int tempMaxcount = 65536;
	int runLength = 0;
	// 分页读入
	do {
		bufferPtr = getCompressedCustkeyBuffer(pcounter++, maxcount);
		if (maxcount == 0) {
			_failFlag = true;
			break;
		}
		// 累加行程长度
		if (bufferPtr[maxcount - 2] < _key) {
			for (int i = 1; i < maxcount; i += 2) {
				runLength += bufferPtr[i];
			}
			continue;
		}
		if (bufferPtr[0] > _key) {
			_failFlag = true;
			break;
		}

		// 二分查找
		int mid = 0, low = 0, high = maxcount - 2;
		while (low <= high) {
			mid = (low + high) / 2;
			if (mid % 2 == 1) {
				mid -= 1;
			}
			if (bufferPtr[mid] == _key) {
				iindex = mid;
				_successFlag = true;
				bufferPtr[mid + 1] ++;
				break;
			}
			else if (low == high) {
				_successFlag = true;
				iindex = low;

				// 当前页插入点后面的item移到临时buffer
				//tempPtr = &bufferPtr[iindex];
				tempPtr = bufferPtr + iindex;
				writeCompressedCustkeyToTemp(tempPtr, maxcount - iindex);

				// 剩余所有页移到临时buffer
				bufferPtr = getCompressedCustkeyBuffer(pcounter++, maxcount);
				while (bufferPtr != NULL) {
					writeCompressedCustkeyToTemp(bufferPtr, maxcount);
					bufferPtr = getCompressedCustkeyBuffer(pcounter++, maxcount);
				}

				// 插入新item到compressedcustkeyFile
				int * newItem = new int[2];
				newItem[0] = _key;
				newItem[1] = 1;
				writeCompressedCustkeyToFile(newItem, 2);

				// 把临时buffer的compressedcustkey复写回compressedcustkeyFile
				tempPtr = getCompressedCustkeyTempBuffer(tempPcounter++, tempMaxcount);
				while (tempPtr != NULL) {
					writeCompressedCustkeyToFile(tempPtr, tempMaxcount);
					tempPtr = getCompressedCustkeyTempBuffer(tempPcounter++, tempMaxcount);
				}

				// 删掉临时buffer文件
				remove("custkey_compressed_temp.db");

				for (int i = 1; i < iindex; i += 2) {
					runLength += bufferPtr[i];
				}
				break;
			}
			else if (bufferPtr[mid] > _key) {
				high = mid - 2;
			}
			else if (bufferPtr[mid] < _key) {
				low = mid + 2;
			}
		}

	} while (_successFlag != true);

	if (_successFlag == true) {
		return runLength;
	}
	else {
		return NULL;
	}
}

bool FileManager::insertIntoOrders(int orderkeyBuffer, int custkeyBuffer, double totalpriceBuffer, int shippriorityBuffer) {
	bool _keyExistFlag = false;
	int runLength = 0;
	int poffset = 0;
	int offset = 0;
	int pcounter = 0;

	int* bufferOrderkey = NULL;
	int* bufferOrderkeyTemp = NULL;
	double* bufferTotalprice = NULL;
	double* bufferTotalpriceTemp = NULL;
	int* bufferShippriority = NULL;
	int* bufferShippriorityTemp = NULL;

	int maxcount1 = 0, maxcount2 = 0, maxcount3 = 0;

	FILE *fin1, *fin2, *fin3;

	FILE *fout1, *fout2, *fout3;

	fout1 = fopen("orderkey_sorted.db", "ab");
	fout2 = fopen("totalprice_sorted.db", "ab");
	fout3 = fopen("shippriority_sorted.db", "ab");

	//判断表中主键的存在性
	_keyExistFlag = select_orders_orderkey(orderkeyBuffer);
	if (_keyExistFlag == true) {
		cout << "FILE CANNOT INSERT: primary key conflicts" << endl;
		return false;
	}
	else {
		// 不存在主键，插入
		runLength = getCustKeyRunLength(custkeyBuffer);
		if (runLength == NULL) {
			return false;
		}
		poffset = runLength / (64 * SIZE_PAGE);
		offset = runLength - (poffset * 64 * SIZE_PAGE);
		
		// 插入orderkey
		pcounter = poffset;
		bufferOrderkey = getOrderkeyBuffer(pcounter++, maxcount1);
		bufferOrderkeyTemp = bufferOrderkey + offset + 1;
		writeOrderkeyToTemp(bufferOrderkeyTemp, maxcount1 - offset);
		bufferOrderkey = getOrderkeyBuffer(pcounter++, maxcount1);
		while (bufferOrderkey != NULL) {
			writeOrderkeyToTemp(bufferOrderkey, maxcount1);
			bufferOrderkey = getOrderkeyBuffer(pcounter++, maxcount1);
		}
		fwrite(&orderkeyBuffer, sizeof(int), 1, fout1);
		pcounter = 0;
		bufferOrderkeyTemp = getOrderkeyTempBuffer(pcounter++, maxcount1);
		while (bufferOrderkeyTemp != NULL) {
			writeOrdersOutputBufferToFile(bufferOrderkeyTemp, maxcount1);
			bufferOrderkeyTemp = getOrderkeyTempBuffer(pcounter++, maxcount1);
		}
		remove("orders_orderkey_temp.db");

		// 插入totalprice
		pcounter = poffset;
		bufferTotalprice = getTotalpriceBuffer(pcounter++, maxcount2);
		bufferTotalpriceTemp = bufferTotalprice + offset + 1;
		writeTotalpriceToTemp(bufferTotalpriceTemp, maxcount2 - offset);
		bufferTotalprice = getTotalpriceBuffer(pcounter++, maxcount2);
		while (bufferTotalprice != NULL) {
			writeTotalpriceToTemp(bufferTotalprice, maxcount2);
			bufferTotalprice = getTotalpriceBuffer(pcounter++, maxcount2);
		}
		fwrite(&totalpriceBuffer, sizeof(double), 1, fout2);
		pcounter = 0;
		bufferTotalpriceTemp = getTotalpriceTempBuffer(pcounter++, maxcount2);
		while (bufferOrderkeyTemp != NULL) {
			writeTotalpriceOutputBufferToFile(bufferTotalpriceTemp, maxcount2);
			bufferTotalpriceTemp = getTotalpriceTempBuffer(pcounter++, maxcount2);
		}
		remove("orders_totalprice_temp.db");

		// 插入shippriority
		pcounter = poffset;
		bufferShippriority = getShippriorityBuffer(pcounter++, maxcount3);
		bufferShippriorityTemp = bufferShippriority + offset + 1;
		writeShippriorityToTemp(bufferShippriorityTemp, maxcount3 - offset);
		bufferShippriority = getShippriorityBuffer(pcounter++, maxcount3);
		while (bufferShippriority != NULL) {
			writeShippriorityToTemp(bufferShippriority, maxcount3);
			bufferShippriority = getShippriorityBuffer(pcounter++, maxcount3);
		}
		fwrite(&shippriorityBuffer, sizeof(int), 1, fout3);
		pcounter = 0;
		bufferShippriorityTemp = getShippriorityTempBuffer(pcounter++, maxcount3);
		while (bufferShippriorityTemp != NULL){
			writeShippriorityOutputBufferToFile(bufferShippriorityTemp, maxcount3);
			bufferShippriorityTemp = getShippriorityTempBuffer(pcounter++, maxcount3);
		}
		remove("orders_shippriority_temp.db");
	}
	return true;
}

void FileManager::writeCompressedCustkeyToFile(int* _orgBuffer, int _incounter) {
  FILE* fout = fopen("custkey_compressed.db", "ab");
  fwrite(_orgBuffer, sizeof(int), _incounter, fout);
  fclose(fout);
}

void FileManager::writeOrdersOutputBufferToFile(int* _orgBuffer, int _incounter) {
  FILE* fout = fopen("orderkey_sorted.db", "ab");
  fwrite(_orgBuffer, sizeof(int), _incounter, fout);
  fclose(fout);
}

void FileManager::writeCustkeyOutputBufferToFile(int* _orgBuffer, int _incounter) {
  FILE* fout = fopen("custkey_sorted.db", "ab");
  fwrite(_orgBuffer, sizeof(int), _incounter, fout);
  fclose(fout);
}

void FileManager::writeTotalpriceOutputBufferToFile(double* _orgBuffer, int _incounter) {
	FILE* fout = fopen("totalprice_sorted.db", "ab");
	fwrite(_orgBuffer, sizeof(double), _incounter, fout);
	fclose(fout);
}

void FileManager::writeShippriorityOutputBufferToFile(int* _orgBuffer, int _incounter) {
	FILE* fout = fopen("shippriority_sorted.db", "ab");
	fwrite(_orgBuffer, sizeof(int), _incounter, fout);
	fclose(fout);
}

bool FileManager::select_orders_orderkey(int _key) {
  return false;
  bool _successFlag = false;
  bool _failFlag = false;
  int* bufferPtr = (int*)this->allocator->Alloc("select_orders_orderkey_buffer", COLUMNBATCHSIZE * sizeof(int));
  int pcounter = 0;
  int sindex = 0;
  int maxcount = 65536;
  // 分页读入
  do {
    //bufferPtr = this->LoadColumnBatch(getOrdersBuffer(pcounter++, maxcount);
    if (maxcount == 0) {
      _failFlag = true;
      break;
    }
    if (bufferPtr[maxcount - 1] < _key) {
      continue;
    }
    if (bufferPtr[0] > _key) {
      _failFlag = true;
      break;
    }

    // 二分查找
    int mid = 0, low = 0, high = maxcount - 1;
    while (low <= high) {
      mid = (low + high) / 2;
      if (bufferPtr[mid] == _key) {
        sindex = mid;
        _successFlag = true;
        break;
      }
      else if (bufferPtr[mid] > _key) {
        high = mid - 1;
      }
      else if (bufferPtr[mid] < _key) {
        low = mid + 1;
      }
    }

  } while (_successFlag != true);
  // 处理结果，返回给Service
  if (_failFlag == true) {
    return false;
  }
  else {
    //_pc = pcounter - 1;
    //_si = sindex;
    return true;
  }
}

//函数作用：  内排序
//参数列表：
// _orgBuffer 原串
//_syncBuffer 伴随串
//  _maxcount 串长度
//返 回 值：  N/A
void FileManager::innerSort(int* _orgBuffer, int* _syncBuffer, int _maxcount) {
  quickSort(_orgBuffer, _syncBuffer, 0, _maxcount - 1);
}

//函数作用： 快速排序
//参数列表：
//      _org 原串
//     _sync 伴随串
//       low 低位
//      high 高位
//返 回 值： N/A
void FileManager::quickSort(int _org[], int _sync[], int low, int high) {
  if (low < high) {
    int _key = partition(_org, _sync, low, high);
    quickSort(_org, _sync, low, _key - 1);
    quickSort(_org, _sync, _key + 1, high);
  }
}

//函数作用： 三数取中分割分区
//参数列表：
//      _org 原串
//     _sync 伴随串
//      _low 低位
//     _high 高位
//返 回 值： 主元下标
int FileManager::partition(int* _org, int *_sync, int _low, int _high) {
  // 交换，使得 data[_mid] < data[_low] < data[_high]
  int _mid = (_low + _high) / 2;
  if (_org[_low] < _org[_mid]) {
    swap(_org[_low], _org[_mid]);
    swap(_sync[_low], _sync[_mid]);
  }
  if (_org[_high] < _org[_mid]) {
    swap(_org[_high], _org[_mid]);
    swap(_sync[_high], _sync[_mid]);
  }
  if (_org[_high] < _org[_low]) {
    swap(_org[_high], _org[_low]);
    swap(_sync[_high], _sync[_low]);
  }
  // 以data[_low]这个居中值作为主元
  int key = _org[_low];
  int i = _low;

  for (int j = _low + 1; j <= _high; j++) {
    if (_org[j] <= key) {
      i++;
      if (i != j) {
        swap(_org[i], _org[j]);
        swap(_sync[i], _sync[j]);
      }
    }
  }
  swap(_org[i], _org[_low]);
  swap(_sync[i], _sync[_low]);
  return i;
}

//函数作用： 交换两个整形数
//参数列表：
//         a 交换变量
//         b 交换变量
//返 回 值： N/A
void FileManager::swap(int& a, int& b) {
  int temp = a;
  a = b;
  b = temp;
}

//函数作用： 工厂方法，获得类的唯一实例
//参数列表： N/A
//返 回 值： 文件管理器的唯一实例
FileManager* FileManager::GetInstance() {
  return FileManager::__instance == NULL ? 
    FileManager::__instance = new FileManager() : FileManager::__instance;
}

//函数作用： 私有的构造器
//参数列表： N/A
//返 回 值： N/A
FileManager::FileManager() {
  this->allocator = DBAllocator::GetInstance();
  pos = 0;
  tpos = 0;
}

// 唯一实例
FileManager* FileManager::__instance = NULL;

CSTORE_NS_END