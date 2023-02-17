/**
 * @file TestMipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// unit test library
#include "catch.hpp"

// project modules
#include "MipComp.hpp"


TEST_CASE( "MipComp::MipComp", "[MipComp]" ) {

  MipComp testRunner("/Users/sean/Documents/school/phd/research/warm_starting/vws/test/datasets/testfiles/mat_rhs_bnd_obj_series_1.test");

  REQUIRE( testRunner.timeout == 180 );
  REQUIRE( testRunner.mipModels.size() == 50 );
}

