#ifndef _DBFM_
#define _DBFM_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#define SIZE_LINE 512
#define SIZE_PAGE 1024

typedef enum __FILE_STATE {
  FNOP,
  READING,
  WRITING
}FILE_STATE;

class FileManager {
public:
  static FileManager* getInstance();
  bool getMiniOrders(int &times, int* custkeyBuffer, int* orderkeyBuffer, double* totalpriceBuffer, int* shippriorityBuffer);
  bool getMiniCustomers(int &times, int* custkeyBuffer);
  int* getOrdersBuffer(int _times, int &_maxcount);
  void importOrders();
  void importCustomers();
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

private:
  long int pos;
  long int tpos;
  int page[1024];
  
  int Buffer_custkey[1024];
  int Buffer_orderkey[1024];
  double Buffer_totalprice[1024];
  int Buffer_shippriority[1024];

  FILE_STATE __sflag;
  static FileManager* __instance;
  FileManager();
};

#endif
