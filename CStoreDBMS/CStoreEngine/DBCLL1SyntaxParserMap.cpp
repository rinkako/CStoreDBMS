#include "DBCLL1SyntaxParserMap.h"
CSTORE_NS_BEGIN
// LL1SyntaxParserMap带尺寸的构造器
LL1SyntaxParserMap::LL1SyntaxParserMap(int row, int col)
  : DBObject("LL1SyntaxParserMap", this) {
  this->iParserMap = new CandidateFunction**[row];
  for (iPtr i = 0; i < row; i++) {
    iParserMap[i] = new CandidateFunction*[col];
  }
  iLeftNodes.clear();
  iNextLeaves.clear();
  iRowCount = row;
  iColCount = col;
}

// LL1SyntaxParserMap设置行类型
void LL1SyntaxParserMap::SetRow(int row, SyntaxType left) {
  if (0 <= row && row < this->iRowCount) {
    iLeftNodes.insert(SyntaxPair(left, row));
  }
}

// LL1SyntaxParserMap设置列类型
void LL1SyntaxParserMap::SetCol(int col, DBTokenType leave) {
  if (0 <= col && col < this->iColCount) {
    iNextLeaves.insert(TokenPair(leave, col));
  }
}

// LL1SyntaxParserMap设置节点分析函数
void LL1SyntaxParserMap::SetCellular(int row, int col, CandidateFunction* proc) {
  if (0 <= row && row < this->iRowCount &&
    0 <= col && col < this->iColCount) {
    this->iParserMap[row][col] = proc;
  }
}

// LL1SyntaxParserMap设置给定产生式的函数
void LL1SyntaxParserMap::SetCellular(SyntaxType left, DBTokenType leave, CandidateFunction* proc) {
  this->SetCellular(this->iLeftNodes[left], this->iNextLeaves[leave], proc);
}

// LL1SyntaxParserMap取得节点的处理函数
CandidateFunction* LL1SyntaxParserMap::GetCFunction(int row, int col) {
  return this->iParserMap[row][col];
}

// LL1SyntaxParserMap取得产生式的处理函数
CandidateFunction* LL1SyntaxParserMap::GetCFunction(SyntaxType left, DBTokenType leave, iHandle nilserver) {
  if (left == SyntaxType::epsilonLeave) {
    return new CandidateFunction(nilserver, CFunctionType::umi_epsilon);
  }
  CandidateFunction* candidator = this->GetCFunction(this->iLeftNodes[left], this->iNextLeaves[leave]);
  return candidator == NULL ? new CandidateFunction(NULL, CFunctionType::umi_errorEnd) : candidator;
}
CSTORE_NS_END