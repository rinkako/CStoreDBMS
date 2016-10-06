#include "DBBridge.h"

CSTORE_NS_BEGIN

// IBridge构造函数
DBBridge::DBBridge()
  :DBObject("DBBridge", this) {
  sourceCodeTokenStream = NULL;
  currentSentence.clear();
  currentSentenceTokenStream.Reset();
  Pile.Reset();
  currentTree = NULL;
  istr sourceCode = "";
  istr sourcePath = "";
}

// IBridge执行解释器
void DBBridge::StartTransaction(DBTransaction& trans, bool isDebug) {
  // 复位并启动词法分析
  this->sourceCode = trans.GetCode();
  iLexiana.Reset();
  iLexiana.SetSourceCode(this->sourceCode);
  sourceCodeTokenStream = iLexiana.Analyze();
  if (isDebug) {
    PILEPRINTLN(">>> Lexical Analyzer Output:");
    PILEPRINTLN(*sourceCodeTokenStream);
  }
  currentSentence = sourceCodeTokenStream->NextSentence();
  currentSentenceTokenStream.Reset();
  for (int i = 0; i < (int)currentSentence.size(); i++) {
    currentSentenceTokenStream.Add(currentSentence[i]);
  }
  // 语法分析
  iParser.SetTokenStream(&currentSentenceTokenStream, iLexiana.GetStrVec());
  iParser.iPTRnextSectence = 0;
  if (isDebug) {
    PILEPRINTLN(">>> Parser Output:");
  }
  currentTree = iParser.Parse();
  if (isDebug && currentTree != NULL) {
    PILEPRINTLN(currentTree->ToString());
  }
  // 语义分析
  if (currentTree != NULL) {
    Pile.SetParseTree(currentTree);
    currentProxy = Pile.Semanticer();
    // 解释执行
    if (isDebug) {
      PILEPRINTLN(">>> Interpreter Output:");
    }
    iDB.Interpreter(currentProxy, trans);
  }
}

// IBridge读取文件代码
istr DBBridge::readCode(istr path) {
  ifile infile;
  istr code = "";
  // 打开文件
  infile.open(path, std::ios::in);
  if (!infile) {
    // 打开失败，返回NULL
    PILEPRINTLN("# Exception: Open file failed!");
    return "";
  }
  istr str;
  while (getline(infile, str)) {
    code = code + str + "\n";
  }
  infile.close();
  return code;
}

CSTORE_NS_END
