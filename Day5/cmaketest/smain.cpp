#include "json/json.h"
#include <fstream>
#include <iostream>
 
int main(int argc, char* argv[]) {
  Json::Value root;
  std::ifstream ifs;
  ifs.open(argv[1]);

  Json::CharReaderBuilder builder;
  builder["collectComments"] = true;
  JSONCPP_STRING errs;
  if (!parseFromStream(builder, ifs, &root, &errs)) {
    std::cout << errs << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << root << std::endl;
  return EXIT_SUCCESS;
}