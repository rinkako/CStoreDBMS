#include "DBController.h"
#include "DBCBase.h"
#include <iostream>

// 私有的构造器
DBController::DBController() { }

// 工厂方法，获得类的唯一实例
DBController* DBController::Invoke() {
  return DBController::Instance == NULL ?
    new DBController() : DBController::Instance;
}

// 初始化当前的数据库引擎
void DBController::DBInit(int argc, char* argv[]) {
  // 设置运行路径
  this->runPath = argv[0];
  // 命令行直接执行时
  if (argc == 1) {
    this->runType = RunType::RUN_CONSOLE;
  }
  // 其她状态构造查询语句
  else {
    this->runType = RunType::RUN_COMMAND;
    std::string queryBuilder = "";
    for (int qb = 1; qb < argc; qb++) {
      queryBuilder += " " + std::string(argv[qb]);
    }
    queryBuilder += SENTENCETERMINATOR;
    this->tempQuery = queryBuilder;
    PILEPRINTLN(">> Build Query From Terminal Input: " + queryBuilder);
  }
}

// 将前端动作转化为后台动作
void DBController::DBStartDash() {
  switch (this->runType) {
  case RunType::RUN_CONSOLE:
    PILEPRINTLN(">> Welcome to Simplified CStore DBMS <<");
    break;
  case RunType::RUN_COMMAND:
    
    break;
  default:
    PILEPRINTLN(">>> ERROR: DBMS Runstate Not Clear");
    break;
  }
}

// 获取当前数据库状态
RunType DBController::GetRunType() {
  return this->runType;
}

// 获取数据库程序在系统的运行路径
std::string DBController::GetRunPath() {
  return this->runPath;
}

// 唯一实例
DBController* DBController::Instance = NULL;