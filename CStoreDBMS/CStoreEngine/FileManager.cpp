#include "FileManager.h"
#include <limits.h>
#include <string.h>
#include <string>

#define COMPRESS_SIZE 65536

using namespace std;

int externSortOrdersBufferPool[64 * SIZE_PAGE];
int externSortCustkeyBufferPool[64 * SIZE_PAGE];
int pagecounter = 0;
int importcounter = 0;
int importcountercustomer = 0;

char page_buf[SIZE_PAGE];
int page_ptr = 0;
int count_spilt = 0;
int maxcount = 0;

bool FileManager::getMiniOrders(
  int &times,
  int* orderkeyBuffer,
  int* custkeyBuffer,
  double* totalpriceBuffer,
  int* shippriorityBuffer) {
  memset(custkeyBuffer, 0, sizeof(custkeyBuffer));
  memset(orderkeyBuffer, 0, sizeof(orderkeyBuffer));
  memset(totalpriceBuffer, 0, sizeof(totalpriceBuffer));
  memset(shippriorityBuffer, 0, sizeof(shippriorityBuffer));

  FILE* fin = fopen("../orders.tbl", "rb");
  if (fin == NULL) {
    fin = fopen("orders.tbl", "rb");
  }
  if (fin == NULL) {
    std::cout << "FILE CANNOT READ: orders.tbl" << std::endl;
  }
  int count_reco = 0;
  int content_ptr = 0;
  char content_buf[128];

  while (count_reco < SIZE_PAGE) {
    // page_buf被读完
    if (page_ptr == maxcount) {
      fseek(fin, times++ * SIZE_PAGE * sizeof(char), SEEK_SET);
      maxcount = fread(page_buf, sizeof(char), SIZE_PAGE, fin);
      page_ptr = 0;
    }
    // 已经不能为page_buf读到任何数据
    if (maxcount == 0)
      break;
    // 分析字符，转换并存入相应缓冲区
    if (page_buf[page_ptr] != '|' && page_buf[page_ptr] != '\n') {
      content_buf[content_ptr++] = page_buf[page_ptr];
    }
    else {
      count_spilt++;
      content_buf[content_ptr] = '\0';
      switch (count_spilt) {
      case 1: orderkeyBuffer[count_reco] = atoi(content_buf); break;
      case 2: custkeyBuffer[count_reco] = atoi(content_buf); break;
      case 4: totalpriceBuffer[count_reco] = atof(content_buf); break;
      case 8: shippriorityBuffer[count_reco] = atoi(content_buf); break;
      case 9: count_reco++; count_spilt = -1; break;
      default:
        break;
      }
      content_ptr = 0;
    }
    page_ptr++;
  }
  fclose(fin);
  importcounter = count_reco;
  if (maxcount == 0) {
    return true;
  }
  else {
    return false;
  }
}

bool FileManager::getMiniCustomers(int &times, int* custkeyBuffer) {
  memset(custkeyBuffer, 0, sizeof(custkeyBuffer));
  FILE* fin = fopen("../customer.tbl", "rb");
  if (fin == NULL) {
    fin = fopen("customer.tbl", "rb");
  }
  if (fin == NULL) {
    std::cout << "FILE CANNOT READ: customer.tbl" << std::endl;
  }
  int count_reco = 0;
  int content_ptr = 0;
  char content_buf[128];

  while (count_reco < SIZE_PAGE) {
    // page_buf被读完
    if (page_ptr == maxcount) {
      fseek(fin, times++ * SIZE_PAGE * sizeof(char), SEEK_SET);
      maxcount = fread(page_buf, sizeof(char), SIZE_PAGE, fin);
      page_ptr = 0;
    }
    // 已经不能为page_buf读到任何数据
    if (maxcount == 0)
      break;
    // 分析字符，转换并存入相应缓冲区
    if (page_buf[page_ptr] != '|' && page_buf[page_ptr] != '\n') {
      content_buf[content_ptr++] = page_buf[page_ptr];
    }
    else {
      count_spilt++;
      content_buf[content_ptr] = '\0';
      switch (count_spilt) {
      case 1: custkeyBuffer[count_reco] = atoi(content_buf); break;
      case 8: count_reco++; count_spilt = -1; break;
      default:
        break;
      }
      content_ptr = 0;
    }
    page_ptr++;
  }
  fclose(fin);
  importcountercustomer = count_reco;
  if (maxcount == 0) {
    return true;
  }
  else {
    return false;
  }
}

void FileManager::importOrders() {
  __sflag = WRITING;
  bool finflag = false;
  int pcounter = 0;
  FILE *fout1, *fout2, *fout3, *fout4;
  // 如果文件已经存在，则清掉文件
  FILE *fdel = fopen("task1_orders_orderkey.db", "w");
  fclose(fdel);
  fdel = fopen("task1_orders_custkey.db", "w");
  fclose(fdel);
  fdel = fopen("task1_orders_totalprice.db", "w");
  fclose(fdel);
  fdel = fopen("task1_orders_shippriority.db", "w");
  fclose(fdel);
  fout1 = fopen("task1_orders_orderkey.db", "ab");
  fout2 = fopen("task1_orders_custkey.db", "ab");
  fout3 = fopen("task1_orders_totalprice.db", "ab");
  fout4 = fopen("task1_orders_shippriority.db", "ab");
  do {
    // 先不断地读入文件到缓冲区
    finflag = getMiniOrders(
      pcounter,
      Buffer_orderkey,
      Buffer_custkey,
      Buffer_totalprice,
      Buffer_shippriority
      );
    // 然后将缓冲区内容写到四个文件里
    fwrite(Buffer_orderkey, sizeof(int), importcounter, fout1);
    fwrite(Buffer_custkey, sizeof(int), importcounter, fout2);
    fwrite(Buffer_totalprice, sizeof(double), importcounter, fout3);
    fwrite(Buffer_shippriority, sizeof(int), importcounter, fout4);
  } while (finflag != true);
  fclose(fout1);
  fclose(fout2);
  fclose(fout3);
  fclose(fout4);
  __sflag = FNOP;
}

void FileManager::importCustomers() {
  __sflag = WRITING;
  bool finflag = false;
  int pcounter = 0;
  FILE *fout1;
  FILE *fdel = fopen("task3_customer_custkey.db", "w");
  fclose(fdel);
  fout1 = fopen("task3_customer_custkey.db", "ab");
  do {
    finflag = getMiniCustomers(
      pcounter,
      Buffer_orderkey
      );
    fwrite(Buffer_orderkey, sizeof(int), importcountercustomer, fout1);
  } while (finflag != true);
  fclose(fout1);
  __sflag = FNOP;
}

int* FileManager::getOrdersBuffer(int _times, int &_maxcount) {
  FILE* fin = fopen("task1_orders_orderkey.db", "rb");
  if (fin == NULL) return NULL;
  fseek(fin, (long)(_times * 65536 * sizeof(int)), SEEK_SET);
  if (fin == 0) {
    return NULL;
  }
  _maxcount = fread(externSortOrdersBufferPool, sizeof(int), 65536, fin);
  fclose(fin);
  return externSortOrdersBufferPool;
}

void FileManager::getEXOrdersBuffer(int _times, int &_maxcount) {
  FILE* fin = fopen("task1_orders_orderkey.db", "rb");
  fseek(fin, (long)(_times * 64 * SIZE_PAGE * sizeof(int)), SEEK_SET);
  if (fin == 0) return;
  _maxcount = fread(externSortOrdersBufferPool, sizeof(int), 64 * SIZE_PAGE, fin);
  fclose(fin);
}

void FileManager::getEXCustkeyBuffer(int _times, int &_maxcount) {
  FILE* fin = fopen("task1_orders_custkey.db", "rb");
  fseek(fin, (long)(_times * 64 * SIZE_PAGE * sizeof(int)), SEEK_SET);
  if (fin == 0) return;
  _maxcount = fread(externSortCustkeyBufferPool, sizeof(int), 64 * SIZE_PAGE, fin);
  fclose(fin);
}

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
  FILE* fin = fopen("task2_custkey_sorted.db", "rb");
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
  FILE* joinFileOrderkey = fopen("task2_orderkey_sorted.db", "rb");
  FILE* joinFileCustkey = fopen("task2_custkey_compressed.db", "rb");
  FILE* joinFileCustomer = fopen("task3_customer_custkey.db", "rb");
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

int FileManager::count() {
  FILE* fin = fopen("task2_custkey_compressed.db", "rb");
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

void FileManager::writeJoinedItemToFile(int* _orderBuffer, int* _custBuffer, int _incounter) {
  //FILE* joinOutOrderkey = fopen("task3_joined_orderkey.db", "ab");
  //FILE* joinOutCustkey = fopen("task3_joined_custkey.db", "ab");
  //fwrite(_orderBuffer, sizeof(int), _incounter, joinOutOrderkey);
  //fwrite(_custBuffer, sizeof(int), _incounter, joinOutCustkey);
  //fclose(joinOutOrderkey);
  //fclose(joinOutCustkey);
  for (int i = 0; i < _incounter; i++)
  {
    std::cout << _custBuffer[i] << " " << _orderBuffer[i] << std::endl;
  }
}

void FileManager::writeCompressedCustkeyToFile(int* _orgBuffer, int _incounter) {
  FILE* fout = fopen("task2_custkey_compressed.db", "ab");
  fwrite(_orgBuffer, sizeof(int), _incounter, fout);
  fclose(fout);
}

void FileManager::writeOrdersOutputBufferToFile(int* _orgBuffer, int _incounter) {
  FILE* fout = fopen("task2_orderkey_sorted.db", "ab");
  fwrite(_orgBuffer, sizeof(int), _incounter, fout);
  fclose(fout);
}

void FileManager::writeCustkeyOutputBufferToFile(int* _orgBuffer, int _incounter) {
  FILE* fout = fopen("task2_custkey_sorted.db", "ab");
  fwrite(_orgBuffer, sizeof(int), _incounter, fout);
  fclose(fout);
}

void FileManager::innerSort(int* _orgBuffer, int* _syncBuffer, int _maxcount) {
  quickSort(_orgBuffer, _syncBuffer, 0, _maxcount - 1);
}

void FileManager::quickSort(int _org[], int _sync[], int low, int high) {
  if (low < high) {
    int _key = partition(_org, _sync, low, high);
    quickSort(_org, _sync, low, _key - 1);
    quickSort(_org, _sync, _key + 1, high);
  }
}

// 三数取中分割方法，返回主元下标。
// _sync[]跟随_org[]变化。
// 保证key的左边都比key小，右边都比key大。
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

void FileManager::swap(int& a, int& b) {
  int temp = a;
  a = b;
  b = temp;
}

// ===================  SYSTEM FUNS  =================== //
FileManager* FileManager::getInstance() {
  if (__instance == NULL) {
    __instance = new FileManager();
  }
  return __instance;
}

FileManager::FileManager() {
  __sflag = FNOP;
  pos = 0;
  tpos = 0;
}

FileManager* FileManager::__instance = NULL;
