#include "DBController.h"

int main(int argc, char* argv[]) {
  CStore::DBController::Invoke()->StartDash(argc, argv);
  CStore::DBAllocator::GetInstance()->Collapse();
  return 0;
}