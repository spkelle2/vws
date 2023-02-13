/**
 * @file MipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <sys/stat.h> // stat

#include "MipComp.hpp"

/** Constructor */
MipComp::MipComp(char * filePath){

  // check if filePath exists
  struct stat buffer;
  assert(stat (filePath, &buffer) == 0);

  // try to read in the instances in the file
  std::ifstream file(filePath);
  std::string str;
  std::regex filePattern("\\[.*\\]");
  while (std::getline(file, str)){
    std::cout << "processing string!\n";
    if (!std::regex_search(str, filePattern)) {
      std::cout << str << " \tMATCHES\t " << std::endl;
    }
  }

} /* Constructor */