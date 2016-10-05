#ifndef ___CSTORE_CSDATABASE
#define ___CSTORE_CSDATABASE
#include "DBCBase.h"
#include "FileManager.h"
#include "TableManager.h"

CSTORE_NS_BEGIN

class CSDatabase {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  CSDatabase();

  //函数作用： 列出所有的表
  //参数列表： N/A
  //返 回 值： N/A
  void ShowTable();

  //函数作用： 复位数据库
  //参数列表： N/A
  //返 回 值： N/A
  void Reset();

  //函数作用： 返回表的数量
  //参数列表： N/A
  //返 回 值： int 数据库中表的数量
  int Size();

  //函数作用： 解释执行一条SSQL语句
  //参数列表：
  //     proxy 解释执行的命令
  //返 回 值： bool 是否成功解释
  bool Interpreter(DBCProxy&);

  //函数作用： 查询一个执行列的字典值
  //参数列表：
  //  paraname 列的名字
  //返 回 值： int 该列在向量中的位置
  int Reference(istr);

  //函数作用： 抽象语法树求表达式值
  //参数列表：
  //    mynode 递归节点
  //    myexec 数据库指针
  //   myproxy 代理器指针
  //返 回 值： bool 表达式真值
  bool static AST(SyntaxTreeNode*, CSDatabase*, DBCProxy*);

private:
  //函数作用： 异常处理函数
  //参数列表：
  //     _info 异常信息
  //    _index 异常句子编号
  //返 回 值： bool 恒为false，因为执行失败
  bool iException(istr, int = -1);

  //函数作用： 查询一个表是否存在
  //参数列表：
  //      name 表的名字
  //返 回 值： bool 表的存在性
  bool Exist(istr);

  //函数作用： 建立一个新表
  //参数列表：
  //      name 表名
  //        pi 列向量
  //    pitype 列类型向量
  //返 回 值： 操作成功与否
  bool Create(istr, StrVec&, StrVec&);

  //函数作用： 删除一个表中的指定行
  //参数列表：
  //      name 表名
  //   condVec 条件列
  //      cond 条件子句树的指针
  //    iproxy 代理指针
  //返 回 值： 操作成功与否
  bool Delete(istr, StrVec&, SyntaxTreeNode* = NULL, DBCProxy* = NULL);

  //函数作用： 向一个表发出查询
  //参数列表：
  //      name 表名
  //    pilist 插值列向量
  //   pivalue 插值值向量
  //  errorbit 除零错误位
  //返 回 值： 操作成功与否
  bool Insert(istr, StrVec&, IntVec&, bool&);

  //函数作用： 向一个表发出查询
  //参数列表：
  //      name 表名
  //        pi 查询列向量
  //      star 是否通配所有列
  //      cond 条件子句树的指针
  //    iproxy 代理指针
  //返 回 值： 操作成功与否
  bool Select(istr, StrVec&, bool, StrVec&, SyntaxTreeNode* = NULL, DBCProxy* = NULL);

  //函数作用： 将输入载入表中
  //参数列表：
  //     tname 表名
  //返 回 值： 操作成功与否
  bool Load(istr);

  //函数作用： 通过主键获取记录
  //参数列表：
  //     tname 表名
  //      tkey 主键的值
  //返 回 值： 操作成功与否
  bool Retrieve(istr, int);

  //函数作用： 压缩表
  //参数列表：
  //     tname 表名
  //        pi 要压缩的列名
  //返 回 值： 操作成功与否
  bool Compress(istr, istr);

  //函数作用： 自然连接表
  //参数列表：
  //    t1name 表名
  //    t2name 表名
  //返 回 值： 操作成功与否
  bool Join(istr, istr);

  //函数作用： 计算记录条目
  //参数列表：
  //     tname 表名
  //返 回 值： 操作成功与否
  bool Count(istr);

  // 互斥量
  std::mutex dbMutex;
  // 文件管理器
  FileManager* fileMana;
  // 表管理器
  TableManager* tableMana;
  // 条件子句字典
  TablePileDictionary _param;
}; /* CSDatabase */

CSTORE_NS_END

#endif /* ___CSTORE_CSDATABASE */