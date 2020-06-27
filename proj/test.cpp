#include <gtest/gtest.h>

extern "C"
{
#include "rg_tree.h"
}

TEST(tmp, tmp){
    printf("[%d]", foo());
    ASSERT_TRUE(0);
}

//-------------------------------------------------------------------------------------
int main(int argc, char **argv) {
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
