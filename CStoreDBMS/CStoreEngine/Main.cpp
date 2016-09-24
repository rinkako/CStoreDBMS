#include "DBController.h"

int main(int argc, char* argv[]) {
  DBController* core = DBController::Invoke();
  core->DBStartDash(argc, argv);
  core->DBExecute();
  return 0;
}
