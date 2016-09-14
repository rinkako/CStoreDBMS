#include "DBCSyntaxTreeNode.h"

// SyntaxTreeNode构造器
SyntaxTreeNode::SyntaxTreeNode() {
  children.clear();
  parent = NULL;
  candidateFunction = NULL;
}

// SyntaxTreeNode拷贝构造器
SyntaxTreeNode::SyntaxTreeNode(const SyntaxTreeNode& _other) {
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
void SyntaxTreeNode::GetTree(istr &builder, SyntaxTreeNode* myNode, int &identation)
{
  // 如果空就没必要继续了
  if (myNode == NULL) {
    return;
  }
  // 画树
  builder += DrawTree(myNode) + myNode->nodeName;
  if (myNode->nodeSyntaxType >= SyntaxType::Unknown
    && myNode->nodeSyntaxType != SyntaxType::epsilonLeave
    && myNode->nodeSyntaxType != SyntaxType::tail_startEndLeave) {
    builder += " (" + myNode->nodeValue + ")";
  }
  builder += NEWLINE;
  // 缩进并打印结果
  identation++;
  for (int i = 0; i < (int)myNode->children.size(); i++) {
    GetTree(builder, myNode->children[i], identation);
  }
  // 回归缩进
  identation--;
}

// SyntaxTreeNode获取缩进
istr SyntaxTreeNode::DrawTree(SyntaxTreeNode* myNode) {
  // 若空就不需要继续了
  if (myNode == NULL) {
    return "";
  }
  // 取父母节点，若空就不需要画线了
  SyntaxTreeNode* parent = myNode->parent;
  if (parent == NULL) {
    return "";
  }
  // 否则查询祖父母节点来看父母节点的排位
  std::vector<bool> lstline;
  while (parent != NULL)
  {
    SyntaxTreeNode* pp = parent->parent;
    int indexOfParent = 0;
    if (pp != NULL) {
      for (; indexOfParent < (int)pp->children.size(); indexOfParent++) {
        if (parent == pp->children[indexOfParent]) {
          break;
        }
      }
      lstline.push_back(indexOfParent < (int)pp->children.size() - 1);
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
  for (; indexOfParent < (int)parent->children.size(); indexOfParent++) {
    if (myNode == parent->children[indexOfParent]) {
      break;
    }
  }
  // 如果是最后一个就不要出头了
  if (indexOfParent < (int)parent->children.size() - 1) {
    builder += "├─";
  }
  else {
    builder += "└─";
  }
  return builder;
}