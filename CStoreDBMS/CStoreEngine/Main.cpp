#include "DBServices.h"

int main(int argc, char* argv[]) {
  DBServices* core = DBServices::getInstance();
  core->DBController(argc, argv);
  core->DBExecutor(); 
  return 0;
}
