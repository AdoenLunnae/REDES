#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "monton.hpp"
#include<algorithm>

TEST_CASE("Constructor"){
    SUBCASE("There are 28 tiles"){
        Monton m;
        CHECK_EQ(m.getRestantes().size(), 28);
    }
}

TEST_CASE("Take tile"){
    Monton m;
    m.cogerFicha(1,5);

    SUBCASE("there is one less tile")
        CHECK_EQ(m.getRestantes().size(), 27);

    SUBCASE("target tile is no longer in the pile") {
                auto  pos = std::find(m.getRestantes().begin(), m.getRestantes().end(), Ficha(1,5));
                CHECK_EQ(pos, m.getRestantes().end());
    }

    SUBCASE("returns false if the tile wasn't in the pile")
        CHECK_FALSE(m.cogerFicha(1,5));
}