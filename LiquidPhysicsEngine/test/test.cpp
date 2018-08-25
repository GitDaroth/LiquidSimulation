#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "SPHKernels/DefaultKernel.h"

TEST_CASE( "Calc kernel weights", "[kernel_weights]" ) {

    LiPhEn::DefaultKernel kernel;
    kernel.setRadius(1.f);

    float epsilon = 0.00001f;
    REQUIRE( abs(kernel.calcKernelWeight(1.f) - 0.f) < epsilon );
}