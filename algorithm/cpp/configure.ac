AC_INIT([algorithm], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

: ${CXXFLAGS="-g -O2 -Wall -Werror"}
AC_PROG_CXX

# check that compiler supports c++17 and set -std=c++17
AX_CXX_COMPILE_STDCXX_17([noext], [mandatory])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])

AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_ERROR([pthread is not installed.])])

# AC_CHECK_LIB doesnt work well with C++ linking. Instead, we use a test program to see if linking of gtest works.
AC_LANG(C++)
LIBS="-lgtest $LIBS"
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <gtest/gtest.h>], [EXPECT_EQ(1,1)])],
    [HAVE_GTEST=1],
    [AC_MSG_ERROR([gtest is not installed.])])

# nlohmann-json is a header only library.
AC_CHECK_HEADER([nlohmann/json.hpp], [], [AC_MSG_ERROR([nlohmann-json is not installed.])])

# we need gmock headers even though we don't link it
AC_CHECK_HEADER([gmock/gmock.h], [], [AC_MSG_ERROR([gmock is not installed.])])

AC_OUTPUT
