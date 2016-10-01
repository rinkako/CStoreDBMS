#include "DBController.h"
using namespace CStore;

int main(int argc, char* argv[]) {
  DBController* core = DBController::Invoke();
  core->DBInit(argc, argv);
  core->DBStartDash();
  return 0;
}