#include "IBridge.h"

// IBridge构造函数
IBridge::IBridge() {
  sourceCodeTokenStream = NULL;
  currentSentence.clear();
  currentSentenceTokenStream.Reset();
  iDB.Reset();
  Pile.Reset();
  currentTree = NULL;
  istr sourceCode = "";
  istr sourcePath = "";
  iType = RunType::RUN_CONSOLE;
}

// IBridge工厂方法
IBridge* IBridge::Invoke() {
  return iInstance == NULL ? iInstance = new IBridge() : iInstance;
}

// IBridge初始化方法
void IBridge::Init(int _myArgc, char* _myArgv[]) {
  // 没有附加参数就进入命令行模式
  if (_myArgc == 1) {
    PILEPRINTLN(NEWLINE
      << "SSQL Interpreter v1.0" << NEWLINE
      << "Copyright Group 12 of CAS 2012 SYSU" << NEWLINE
      << "-----------------------------------------" << NEWLINE);
    this->sourcePath = _myArgv[0];
    this->iType = RunType::RUN_CONSOLE;
  }
  // 否则是读文件的情况
  else {
    this->sourcePath = _myArgv[1];
    this->iType = RunType::RUN_INFILE;
    this->sourceCode = readCode(this->sourcePath);
  }
}

// IBridge执行解释器
void IBridge::StartDash(bool isDebug) {
  // 控制台运行情况
  if (this->iType == RunType::RUN_CONSOLE) {
    istr commandBuffer;
    bool rflag = true;
    bool breakFlag = false;
    FOREVER {
      // 读取控制台命令以分号结束
      if (isDebug) {
        PILEPRINT("SSQL# ");
      }
      else {
        PILEPRINT("SSQL> ");
      }
      char charBuilder;
      istr commandBuilder = "";
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
          commandBuilder += charBuilder;
        }
      } while (charBuilder != SENTENCETERMINATOR);
      breakFlag = true;
      rflag = false;
      // 特殊命令的判断
      if (commandBuilder == "exit;") {
        PILEPRINTLN("Good Bye!");
        return;
      }
      else if (commandBuilder == "show table;") {
        iDB.ShowTable();
        continue;
      }
      else if (commandBuilder == "debug;") {
        isDebug = true;
        continue;
      }
      else if (commandBuilder == "no debug;") {
        isDebug = false;
        continue;
      }
      // 复位并启动词法分析
      sourceCode = commandBuilder;
      iLexiana.Reset();
      iLexiana.SetSourceCode(sourceCode);
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
      currentTree = iParser.Parse();
      if (isDebug) {
        PILEPRINTLN(">>> Parser Output:");
        if (currentTree != NULL) {
          PILEPRINTLN(currentTree->ToString());
        }
      }
      // 语义分析
      if (currentTree != NULL) {
        Pile.SetParseTree(currentTree);
        currentProxy = Pile.Semanticer();
        // 解释执行
        if (isDebug) {
          PILEPRINTLN(">>> Interpreter Output:");
        }
        iDB.Interpreter(currentProxy);
      }
      if (isDebug) {
        PILEPRINTLN("====================== Interpreted.");
      }
      PILEPRINT(NEWLINE);
    }
  }
  // 文件读入情况
  else if (this->iType == RunType::RUN_INFILE) {
    // 词法分析
    iLexiana.SetSourceCode(sourceCode);
    sourceCodeTokenStream = iLexiana.Analyze();
    if (isDebug) {
      PILEPRINTLN(">>> Lexical Analyzer Output:");
      PILEPRINTLN(*sourceCodeTokenStream);
    }
    int spointer = 0;
    FOREVER {
      currentSentence = sourceCodeTokenStream->NextSentence();
      if (!currentSentence.size()) {
        break;
      }
      currentSentenceTokenStream.Reset();
      for (int i = 0; i < (int)currentSentence.size(); i++) {
        currentSentenceTokenStream.Add(currentSentence[i]);
      }
      // 语法分析
      PILEPRINTLN(">> " + iLexiana.GetStrVec()->at(spointer++));
      iParser.SetTokenStream(&currentSentenceTokenStream, iLexiana.GetStrVec());
      currentTree = iParser.Parse();
      if (isDebug) {
        PILEPRINTLN(">>> Parser Output:");
        if (currentTree != NULL) {
          PILEPRINTLN(currentTree->ToString());
        }
      }
      // 语义分析
      if (currentTree != NULL) {
        Pile.SetParseTree(currentTree);
        currentProxy = Pile.Semanticer();
        if (isDebug) {
          PILEPRINTLN(">>> Interpreter Output:");
        }
        // 解释执行
        iDB.Interpreter(currentProxy);
      }
      if (isDebug) {
        PILEPRINTLN("====================== Interpreted.");
      }
      PILEPRINT(NEWLINE);
    }
    // 执行完毕，等待回车退出
    PILEPRINTLN("\n# All interpretation missions accomplished!");
    PILEPRINTLN("# Press Enter to exit");
    char charBuilder;
    FOREVER {
      charBuilder = getchar();
      if (charBuilder == '\n') {
        PILEPRINTLN("# Good Bye!");
        break;
      }
    }
  }
}

// IBridge读取文件代码
istr IBridge::readCode(istr path) {
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

// IBridge唯一单例的定义
IBridge* IBridge::iInstance = NULL;