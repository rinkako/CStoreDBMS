#include "DBController.h"
#include "DBAllocator.h"

int main(int argc, char* argv[]) {
  CStore::DBController::Invoke()->StartDash(argc, argv);
  CStore::DBAllocator::Collapse();
  return 0;
}
