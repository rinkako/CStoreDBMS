#include "DBController.h"
using namespace CStore;

int main(int argc, char* argv[]) {
  DBController* core = DBController::Invoke();
  core->DBInit(argc, argv);
  core->DBStartDash();
  return 0;
}

/**
 * 当前逻辑：
 * Controller   - 前端与后台交互，接收输入字符串  - （输出：string）
 * Connector    - 将查询字符串封装为数据库事务    - （输出：DBTransaction）
 * Bridge       - 调用编译器和数据库引擎处理事务  - （输出：string[]）
 * Lexer        - 将查询单句解析为单词流          - （输出：TokenStream）
 * Parser       - 将单词流解析为语法树            - （输出：SyntaxTreeNode）
 * Pile         - 将语法树解析为执行单元          - （输出：DBCProxy）
 * IDatabase    - 解析执行单元为数据库动作        - （输出：对FileManager各函数的调用）
 * TableMananer - 对要操作的表格进行锁处理        - （记录表格信息，处理加锁解锁） 
 * FileManager  - 处理实际数据库文件操作          - （实际文件操作）
 **/