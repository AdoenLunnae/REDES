#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "doctest.h"
#include "ficha.hpp"

TEST_SUITE_BEGIN("Ficha");

TEST_CASE("Constructor"){
    SUBCASE("Valid parameters"){
        Ficha f(3,4);
        CHECK_EQ(f.getN1(), 3);
        CHECK_EQ(f.getN2(), 4);
    }

    SUBCASE("Invalid parameters: negative"){
        CHECK_THROWS_WITH(Ficha(-1, 3), "Invalid Argument");
        CHECK_THROWS_WITH(Ficha(4, -3), "Invalid Argument");
    }


    SUBCASE("Invalid parameters: too big"){
        CHECK_THROWS_WITH(Ficha(7, 3), "Invalid Argument");
        CHECK_THROWS_WITH(Ficha(4, 8), "Invalid Argument");
    }
}

TEST_CASE("Comparator"){
    SUBCASE("Two tiles in the same position")
        CHECK(Ficha(1,3) == Ficha(1,3));

    SUBCASE("Two tiles in opposite position")
        CHECK(Ficha(1,3) == Ficha(3,1));
    
    SUBCASE("Different Tiles")
        CHECK_FALSE(Ficha(1, 3) == Ficha(1, 4));
        CHECK_FALSE(Ficha(1, 3) == Ficha(2, 3));
}

TEST_CASE("Rotation"){
    Ficha f(3,1);
    f.rotar();
    CHECK_EQ(f.getN1(), 1);
    CHECK_EQ(f.getN2(), 3);
}

TEST_SUITE_END();