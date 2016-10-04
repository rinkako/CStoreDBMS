#include "DBController.h"

CSTORE_NS_BEGIN

// 私有的构造器
DBController::DBController()
  :DBObject("DBController", this) {
  this->connector = DBConnectionPool::GetInstance();
}

// 析构器
DBController::~DBController() {
  if (this->connector != NULL) {
    delete this->connector;
  }
  this->connector = NULL;
}

// 工厂方法，获得类的唯一实例
DBController* DBController::Invoke() {
  return DBController::Instance == NULL ?
    new DBController() : DBController::Instance;
}

// 将前端动作转化为后台动作
void DBController::StartDash(int argc, char* argv[]) {
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
  switch (this->runType) {
  case RunType::RUN_CONSOLE:
    this->Terminal();
    break;
  case RunType::RUN_COMMAND:
    this->Dash(this->tempQuery);
    break;
  default:
    PILEPRINTLN(">>>>>>>> EXCEPTION: DBMS Runstate Not Clear");
    break;
  }
}

// 处理控制台终端输入
void DBController::Terminal() {
  PILEPRINTLN(">> Welcome to Simplifie-CStore DBMS <<");
  bool rflag = true;
  bool breakFlag = false;
  FOREVER {
    // 读取控制台命令以分号结束
    if (this->DebugMode) {
      PILEPRINT("SSQL# ");
    }
    else {
      PILEPRINT("SSQL> ");
    }
    char charBuilder;
    CSCommonUtil::StringBuilder queryBuilder;
    do {
      charBuilder = getchar();
      if (charBuilder == '\n') {
        charBuilder = ' ';
        if (rflag) {
          PILEPRINT("    > ");
        }
        rflag = true;
      }
      if (breakFlag) {
        breakFlag = false;
      }
      else {
        queryBuilder.Append(charBuilder);
      }
    } while (charBuilder != SENTENCETERMINATOR);
    // 读取命令完成
    breakFlag = true;
    rflag = false;
    std::string commitQuery = queryBuilder.ToString();
    std::string _query = commitQuery;
    std::transform(_query.begin(), _query.end(), _query.begin(), tolower);
    // 特殊命令的判断
    if (_query == "exit;") {
      TRACE("Good Bye!");
      return;
    }
    else if (this->ReservedRouter(_query) == true) {
      continue;
    }
    // 提交给连接器
    this->connector->SetDebug(this->DebugMode);
    this->connector->Commit(commitQuery);
    // 换行，准备接收下一次输入
    PILEPRINT(NEWLINE);
  }
}

// 处理查询语句输入
void DBController::Dash(const std::string& query) {
  std::string _query = query;
  std::transform(_query.begin(), _query.end(), _query.begin(), tolower);
  // 特殊命令的判断
  if (_query == "exit;") {
    TRACE("Good Bye!");
    return;
  }
  if (this->ReservedRouter(_query) == true) {
    return;
  }
  // 提交给连接器
  this->connector->SetDebug(this->DebugMode);
  this->connector->Commit(query);
}

// 处理特殊命令
bool DBController::ReservedRouter(const std::string& query) {
  if (query == "debug;") {
    this->DebugMode = true;
    return true;
  }
  else if (query == "no debug;") {
    this->DebugMode = false;
    return true;
  }
  else if (query == "show transactions;") {

    return true;
  }
  else if (query == "show tables;") {
    
    return true;
  }
  return false;
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

CSTORE_NS_END
