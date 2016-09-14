#include "QueryManager.h"
#include <stdio.h>

QueryManager* QueryManager::getInstance() {
  if (_queryInstance == NULL) {
    _queryInstance = new QueryManager();
  }
  return _queryInstance;
}

// 对orders进行挑选
bool QueryManager::select_orders(int _key, int &_pc, int &_si) {
  bool _successFlag = false;
  bool _failFlag = false;
  int* bufferPtr = NULL;
  int pcounter = 0;
  int sindex = 0;
  int maxcount = 65536;
  // 分页读入
  do {
    bufferPtr = _fileInstance->getOrdersBuffer(pcounter++, maxcount);
    if (maxcount == 0) {
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
    _pc = pcounter - 1;
    _si = sindex;
    return true;
  }
}

void QueryManager::select_orders_output(int _key){
  int _pc, _si;
  int temp_int;
  double temp_db;
  bool _sFlag = this->select_orders(_key, _pc, _si);
  if (_sFlag == false) {
    printf("NULL\n");
    return;
  }
  printf("%d ", _key);
  FILE* fin1 = fopen("task1_orders_custkey.db", "rb");
  fseek(fin1, (long)((_pc * 65536 + _si) * sizeof(int)), SEEK_SET);
  fread(&temp_int, sizeof(int), 1, fin1);
  printf("%d ", temp_int);
  FILE* fin2 = fopen("task1_orders_totalprice.db", "rb");
  fseek(fin2, (long)((_pc * 65536 + _si) * sizeof(double)), SEEK_SET);
  fread(&temp_db, sizeof(double), 1, fin2);
  printf("%.2f ", temp_db);
  FILE* fin3 = fopen("task1_orders_shippriority.db", "rb");
  fseek(fin3, (long)((_pc * 65536 + _si) * sizeof(int)), SEEK_SET);
  fread(&temp_int, sizeof(int), 1, fin3);
  printf("%d\n", temp_int);
}

QueryManager::QueryManager() {
  _queryState = QNOP;
}

QueryManager* QueryManager::_queryInstance = NULL;
