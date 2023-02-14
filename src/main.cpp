/**
 * @file main.cpp
 * @author Sean Kelley
 * @date 2023-02-01
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <chrono> // for timing
#include <limits> // numeric_limits
#include <memory> // for smart pointers


// Project files
#include "MipComp.hpp"


/****************** MAIN FUNCTION **********************/
int main(int argc, char** argv) {

  // executable and testfile should be only arguments
  assert(argc == 2);

  MipComp testRunner(argv[1]);

} /* main */

// a function to unit test the MipComp class


