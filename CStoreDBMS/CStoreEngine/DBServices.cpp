#include "DBServices.h"
#include <string.h>
#include <stdio.h>

void DBServices::DBController(int argc, char* argv[]) {
  // 从参数分析进行何种事务
  if (argc == 2) {
    if (strcmp(argv[1], "join") == 0) {
      __global = JOIN;
    }
    else if (strcmp(argv[1], "count") == 0) {
      __global = COUNT;
    }
    else if (strcmp(argv[1], "test") == 0) {
      __global = TEST;
    }
  }
  else if (argc == 3) {
    if (strcmp(argv[1], "load") == 0 &&
      strcmp(argv[2], "orders") == 0) {
      __global = LOAD_ORDERS;
    }
    else if (strcmp(argv[1], "retrieve") == 0 &&
      strcmp(argv[2], "orders") == 0) {
      __global = RETRIEVE_ORDERS;
    }
    else if (strcmp(argv[1], "load") == 0 &&
      strcmp(argv[2], "customer") == 0) {
      __global = LOAD_CUSTOMER;
    }
  }
  else if (argc == 4) {
    if (strcmp(argv[1], "compress") == 0 &&
      strcmp(argv[2], "orders") == 0 &&
      strcmp(argv[3], "1") == 0) {
      __global = COMPRESS_ORDERS_1;
    }
  }
}

// 处理器
void DBServices::DBExecutor() {
  // 如果没有参数，那么结束程序
  if (__global == SNOP) { return; }
  // 如果有参数，则调用相应的服务程序
  if (__global == LOAD_ORDERS) {
    __fileInstance->importOrders();
  }
  else if (__global == RETRIEVE_ORDERS) {
    int _queryKey;
    while (scanf("%d", &_queryKey) != EOF) {
      __queryInstance->select_orders_output(_queryKey);
    }
  }
  else if (__global == COMPRESS_ORDERS_1) {
    __fileInstance->externSort();
  }
  else if (__global == TEST) {
    __fileInstance->compressCustkey();
  }
  else if (__global == LOAD_CUSTOMER) {
    __fileInstance->importCustomers();
  }
  else if (__global == JOIN) {
    __fileInstance->join();
  }
  else if (__global == COUNT) {
    printf("Count: %d\n", __fileInstance->count());
  }
}

_gState DBServices::DBState() {
  return __global;
}

// 单例函数
DBServices* DBServices::getInstance() {
  if (__instance == NULL) {
    __instance = new DBServices();
  }
  return __instance;
}

DBServices::DBServices() {
  __global = SNOP;
  __fileInstance = FileManager::getInstance();
  __queryInstance = QueryManager::getInstance();
}

DBServices* DBServices::__instance = NULL;
