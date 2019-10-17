#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "jugador.hpp"
#include "monton.hpp"

TEST_CASE("Constructor"){
    SUBCASE("hand is empty")
        CHECK_EQ(Jugador().getMano().size(), 0);
}

TEST_CASE("Drawing tiles"){
    Jugador j;
    Monton m;
    SUBCASE("hand size increases by 1")
        j.robar(m);
        CHECK_EQ(j.getMano().size(), 1);
        j.robar(m);
        CHECK_EQ(j.getMano().size(), 2);
}