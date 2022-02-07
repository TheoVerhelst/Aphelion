#include <polygon.hpp>
#include <catch.hpp>

using namespace Catch::literals;

TEST_CASE("polygon functions", "[polygon]") {
    const ConvexPolygon box{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
    const ConvexPolygon triangle{{{0, 0}, {1, 0}, {0, 1}}};

    SECTION("contains") {
        REQUIRE(triangle.contains({0.1f, 0.1f}));
        REQUIRE(triangle.contains({0, 0}));
        REQUIRE(not triangle.contains({-0.1f, 1}));
        REQUIRE(box.contains({0.5f, 0.5f}));
        REQUIRE(box.contains({0, 0}));
        REQUIRE(box.contains({1, 1}));
        REQUIRE(not box.contains({-0.1f, 1}));
    }

    SECTION("areaAndCenterOfMass") {
        auto [triangleArea, triangleCenterOfMass] = triangle.areaAndCenterOfMass();
        REQUIRE(triangleArea == 0.5_a);
        REQUIRE(triangleCenterOfMass.x == Approx(1./3.));
        REQUIRE(triangleCenterOfMass.y == Approx(1./3.));
        auto [boxArea, boxCenterOfMass] = box.areaAndCenterOfMass();
        REQUIRE(boxArea == 1_a);
        REQUIRE(boxCenterOfMass.x == Approx(1./2.));
        REQUIRE(boxCenterOfMass.y == Approx(1./2.));
    }

    SECTION("momentOfInertia") {
        REQUIRE(triangle.momentOfInertia(1, {0, 0}) == Approx(1./6.));
        REQUIRE(box.momentOfInertia(1, {0, 0.5}) == Approx(5./12.));
    }

    SECTION("boxContains") {
        const std::array<Vector2f, 4> arrayBox{{{0, 0}, {0, 1}, {1, 1}, {1, 0}}};
        REQUIRE(boxContains(arrayBox, {0.5f, 0.5f}));
        REQUIRE(boxContains(arrayBox, {0, 0}));
        REQUIRE(boxContains(arrayBox, {1, 1}));
        REQUIRE(not boxContains(arrayBox, {-0.1f, 1}));
    }

    SECTION("supportFunction") {
        REQUIRE(triangle.supportFunction({1, 0}).x == 1_a);
        REQUIRE(triangle.supportFunction({1, 0}).y == 0_a);
        REQUIRE(triangle.supportFunction({1, 2}).x == 0_a);
        REQUIRE(triangle.supportFunction({1, 2}).y == 1_a);
        REQUIRE(triangle.supportFunction({-1, -1}).x == 0_a);
        REQUIRE(triangle.supportFunction({-1, -1}).y == 0_a);
        REQUIRE(box.supportFunction({1, 1}).x == 1_a);
        REQUIRE(box.supportFunction({1, 1}).y == 1_a);
        REQUIRE(box.supportFunction({1, 2}).x == 1_a);
        REQUIRE(box.supportFunction({1, 2}).y == 1_a);
        REQUIRE(box.supportFunction({-1, -1}).x == 0_a);
        REQUIRE(box.supportFunction({-1, -1}).y == 0_a);
    }
}
