#ifndef _DBSERVICE_
#define _DBSERVICE_

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "FileManager.h"
#include "QueryManager.h"

typedef enum __STATE {
  SNOP,
  LOAD_ORDERS,
  RETRIEVE_ORDERS,
  LOAD_CUSTOMER,
  COMPRESS_ORDERS_1,
  JOIN,
  COUNT,
  TEST
} _gState;


class DBServices {
public:
  void DBController(int argc, char* argv[]);
  void DBExecutor();
  _gState DBState();
  static DBServices* getInstance();

private:
  int _ourKey;
  _gState __global;
  static DBServices* __instance;
  FileManager* __fileInstance;
  QueryManager* __queryInstance;
  DBServices();
};

#endif
