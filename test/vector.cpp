#include <cmath>
#include <vector.hpp>
#include <catch.hpp>

using namespace Catch::literals;

TEMPLATE_TEST_CASE("vector functions", "[vector][template]", float) {
    SECTION("degToRad") {
        REQUIRE(degToRad<TestType>(0) == 0_a);
        REQUIRE(degToRad<TestType>(90) == Approx(pi/2));
        REQUIRE(degToRad<TestType>(180) == Approx(pi));
        REQUIRE(degToRad<TestType>(-90) == Approx(-pi/2));
    }

    SECTION("radToDeg") {
        REQUIRE(radToDeg<TestType>(0) == 0_a);
        REQUIRE(radToDeg<TestType>(pi/2) == 90_a);
        REQUIRE(radToDeg<TestType>(pi) == 180_a);
        REQUIRE(radToDeg<TestType>(-pi/2) == -90_a);
    }

    SECTION("dot") {
        REQUIRE(dot<TestType>({0, 0}, {3, 4}) == 0_a);
        REQUIRE(dot<TestType>({4, -2}, {2, 1.5}) == 5_a);
    }

    SECTION("norm") {
        REQUIRE(norm<TestType>({0, 0}) == 0_a);
        REQUIRE(norm<TestType>({1, 1}) == Approx(std::sqrt(2)));
        REQUIRE(norm<TestType>({-3, -4}) == 5_a);
        REQUIRE(norm<TestType>({3, -4}) == 5_a);
    }

    SECTION("norm2") {
        REQUIRE(norm2<TestType>({0, 0}) == 0_a);
        REQUIRE(norm2<TestType>({1, -1}) == 2_a);
        REQUIRE(norm2<TestType>({-3, 4}) == 25_a);
    }

    SECTION("angle") {
        REQUIRE(angle<TestType>({1, 0}) == 0_a);
        REQUIRE(angle<TestType>({1, 1}) == Approx(pi/4));
        REQUIRE(angle<TestType>({33, -33}) == Approx(-pi/4));
        REQUIRE(angle<TestType>({0, 0}, {1, 0}, {1, 3.11f}) == Approx(pi/2));
        REQUIRE(angle<TestType>({0, 0}, {1, 0}, {1, -3.11f}) == Approx(3*pi/2));
        REQUIRE(angle<TestType>({0, 1}, {0, 0}, {1, -1}) == Approx(3*pi/4));
        REQUIRE(angle<TestType>({0, 1}, {0, 0}, {-1, -1}) == Approx(5*pi/4));
    }

    SECTION("cross") {
        REQUIRE(cross<TestType>({1, 0}, {3, 0}) == 0_a);
        REQUIRE(cross<TestType>({1, 2}, {3, 4}) == -2_a);
    }

    SECTION("perpendicular") {
        REQUIRE(perpendicular<TestType>({1, 0}, true) == Vector2<TestType>{0, -1});
        REQUIRE(perpendicular<TestType>({1, 0}, false) == Vector2<TestType>{0, 1});
        REQUIRE(perpendicular<TestType>({0, 22}, true) == Vector2<TestType>{22, 0});
        REQUIRE(perpendicular<TestType>({0, 11}, false) == Vector2<TestType>{-11, 0});
        REQUIRE(perpendicular<TestType>({0, 11}, {1, 1}) == Vector2<TestType>{11, 0});
        REQUIRE(perpendicular<TestType>({11, 0}, {-1, -1}) == Vector2<TestType>{0, -11});
        REQUIRE(perpendicular<TestType>({0, 11}, {-1, -1}) == Vector2<TestType>{-11, 0});
    }

    SECTION("rotate") {
        REQUIRE(rotate<TestType>({1, 0}, pi/2).x == Approx(0).margin(1));
        REQUIRE(rotate<TestType>({1, 0}, pi/2).y == 1_a);
        REQUIRE(rotate<TestType>({10, 0}, pi).x == -10_a);
        REQUIRE(rotate<TestType>({10, 0}, pi).y == Approx(0).margin(1));
        REQUIRE(rotate<TestType>({2, 0}, -pi/4).x == Approx(std::sqrt(2.f)));
        REQUIRE(rotate<TestType>({2, 0}, -pi/4).y == Approx(-std::sqrt(2.f)));
    }

    SECTION("intersection") {
        Vector2<TestType> A{1, 1}, B{3, 1};
        SECTION("intersection is between the points") {
            Vector2<TestType> C{2, 4}, D{2, 0};
            REQUIRE(intersection(A, B, C, D).first == 0.5_a);
            REQUIRE(intersection(A, B, C, D).second == 0.75_a);
        }
        SECTION("intersection is outside the points") {
            Vector2<TestType> C{2, 0}, D{2, -4};
            REQUIRE(intersection(A, B, C, D).second == -0.25_a);
        }
    }

    SECTION("closestPoint") {
        Vector2<TestType> A{1, 1}, B{3, 1};
        SECTION("P is between A and B") {
            Vector2<TestType> P{2, 10};
            REQUIRE(closestPoint(A, B, P).x == 2_a);
            REQUIRE(closestPoint(A, B, P).y == 1_a);
        }
        SECTION("P is closer to A") {
            Vector2<TestType> P{0, 0};
            REQUIRE(closestPoint(A, B, P).x == 1_a);
            REQUIRE(closestPoint(A, B, P).y == 1_a);
        }
        SECTION("P is closer to B") {
            Vector2<TestType> P{10, 10};
            REQUIRE(closestPoint(A, B, P).x == 3_a);
            REQUIRE(closestPoint(A, B, P).y == 1_a);
        }
    }

    SECTION("clampVector") {
        REQUIRE(clampVector<TestType>({10, 10}, {1, 1}, {5, 5}).x == 5_a);
        REQUIRE(clampVector<TestType>({10, 10}, {1, 1}, {5, 5}).y == 5_a);
        REQUIRE(clampVector<TestType>({1, 1}, {0, 0}, {10, 10}).x == 1_a);
        REQUIRE(clampVector<TestType>({1, 1}, {0, 0}, {10, 10}).y == 1_a);
    }
}
