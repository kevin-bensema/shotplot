#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <Core/UnitConverter.h>

using Catch::Matchers::WithinAbs;

TEST_CASE("Smoke test: UnitConverter pixel/inch round-trip", "[smoke][units]")
{
    constexpr double ppi = 96.0;
    double inches = UnitConverter::pixelsToInches(200.0, ppi);
    CHECK_THAT(UnitConverter::inchesToPixels(inches, ppi), WithinAbs(200.0, 1e-9));
}
