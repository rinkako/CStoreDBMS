#include "DBCSyntaxTreeNode.h"

CSTORE_NS_BEGIN

// SyntaxTreeNode构造器
SyntaxTreeNode::SyntaxTreeNode()
  : DBObject("SyntaxTreeNode", this) {
  children.clear();
  parent = nullptr;
  candidateFunction = nullptr;
}

// SyntaxTreeNode拷贝构造器
SyntaxTreeNode::SyntaxTreeNode(const SyntaxTreeNode& _other)
  : DBObject("SyntaxTreeNode", this) {
  this->candidateFunction = _other.candidateFunction;
  this->parent = _other.parent;
  this->errorCode = _other.errorCode;
  this->errorInfo = _other.errorInfo;
  this->mappedBegin = _other.mappedBegin;
  this->mappedLength = _other.mappedLength;
  this->mappedList = _other.mappedList;
  this->children = _other.children;
  this->nodeValue = _other.nodeValue;
  this->nodeName = _other.nodeName;
  this->nodeType = _other.nodeType;
  this->aTag = _other.aTag;
}

// SyntaxTreeNode输出函数
istr SyntaxTreeNode::ToString() {
  istr builder = "";
  int identation = 0;
  GetTree(builder, this, identation);
  return builder;
}

// SyntaxTreeNode递归遍历显示
void SyntaxTreeNode::GetTree(istr &builder, SyntaxTreeNode* myNode, int &identation) const {
  // 如果空就没必要继续了
  if (myNode == nullptr) {
    return;
  }
  // 画树
  builder += DrawTree(myNode) + myNode->nodeName;
  if (myNode->nodeSyntaxType >= SyntaxType::SyntaxUnknown
    && myNode->nodeSyntaxType != SyntaxType::epsilonLeave
    && myNode->nodeSyntaxType != SyntaxType::tail_startEndLeave) {
    builder += " (" + myNode->nodeValue + ")";
  }
  builder += NEWLINE;
  // 缩进并打印结果
  identation++;
  for (int i = 0; i < static_cast<int>(myNode->children.size()); i++) {
    GetTree(builder, myNode->children[i], identation);
  }
  // 回归缩进
  identation--;
}

// SyntaxTreeNode获取缩进
istr SyntaxTreeNode::DrawTree(SyntaxTreeNode* myNode) const {
  // 若空就不需要继续了
  if (myNode == nullptr) {
    return "";
  }
  // 取父母节点，若空就不需要画线了
  SyntaxTreeNode* parent = myNode->parent;
  if (parent == nullptr) {
    return "";
  }
  // 否则查询祖父母节点来看父母节点的排位
  std::vector<bool> lstline;
  while (parent != nullptr)
  {
    SyntaxTreeNode* pp = parent->parent;
    int indexOfParent = 0;
    if (pp != nullptr) {
      for (; indexOfParent < static_cast<int>(pp->children.size()); indexOfParent++) {
        if (parent == pp->children[indexOfParent]) {
          break;
        }
      }
      lstline.push_back(indexOfParent < static_cast<int>(pp->children.size()) - 1);
    }
    parent = pp;
  }
  // 画纵向线
  istr builder = "";
  for (int i = lstline.size() - 1; i >= 0; i--) {
    builder += lstline[i] ? "│  " : "    ";
  }
  // 获得自己在兄弟姐妹中的排行
  parent = myNode->parent;
  int indexOfParent = 0;
  for (; indexOfParent < static_cast<int>(parent->children.size()); indexOfParent++) {
    if (myNode == parent->children[indexOfParent]) {
      break;
    }
  }
  // 如果是最后一个就不要出头了
  if (indexOfParent < static_cast<int>(parent->children.size()) - 1) {
    builder += "├─";
  }
  else {
    builder += "└─";
  }
  return builder;
}

CSTORE_NS_END
