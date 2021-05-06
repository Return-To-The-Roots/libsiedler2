// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "libsiedler2/loadMapping.h"
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <utility>
#include <vector>

using Value = std::pair<unsigned, std::string>;
// LCOV_EXCL_START
BOOST_TEST_DONT_PRINT_LOG_VALUE(Value)
// LCOV_EXCL_STOP

BOOST_AUTO_TEST_SUITE(LoadMapping)

BOOST_AUTO_TEST_CASE(SimpleMapping)
{
    std::stringstream s;
    s << "0 a\n"
      << "1 bb\n"
      << "2 c123\n"
      << "5 d e ff\n"
      << "\n" // empty line
      << "10 some\tvalue\n"
      << "7 foobar\n"
      << "8\t1124\n";
    s.seekg(0);
    std::vector<Value> values;
    libsiedler2::loadMapping(s, [&values](unsigned idx, const std::string& value) { values.emplace_back(idx, value); });
    BOOST_TEST_REQUIRE(values.size() == 7u);
    BOOST_TEST(values[0] == Value(0, "a"));
    BOOST_TEST(values[1] == Value(1, "bb"));
    BOOST_TEST(values[2] == Value(2, "c123"));
    BOOST_TEST(values[3] == Value(5, "d e ff"));
    BOOST_TEST(values[4] == Value(10, "some\tvalue"));
    BOOST_TEST(values[5] == Value(7, "foobar"));
    BOOST_TEST(values[6] == Value(8, "1124"));
}

BOOST_AUTO_TEST_CASE(IgnoreComments)
{
    std::stringstream s;
    s << "#0 a\n"
      << "1 bb\n"
      << "# 2 c123\n"
      << "# 5 d e ff\n"
      << "10 #some\tvalue\n"
      << "7 foobar\n";
    s.seekg(0);
    std::vector<Value> values;
    libsiedler2::loadMapping(s, [&values](unsigned idx, const std::string& value) { values.emplace_back(idx, value); });
    BOOST_TEST(values[0] == Value(1, "bb"));
    BOOST_TEST(values[1] == Value(10, "#some\tvalue"));
    BOOST_TEST(values[2] == Value(7, "foobar"));
}

static auto makeExceptionCheck(std::string msgPart)
{
    return [msgPart = std::move(msgPart)](const std::runtime_error& e) {
        return std::string(e.what()).find(msgPart) != std::string::npos;
    };
}

BOOST_AUTO_TEST_CASE(DetectErrors)
{
    const auto doNothing = [](unsigned, const std::string&) {};
    std::stringstream s("1234\n");
    s.seekg(0);
    BOOST_CHECK_EXCEPTION(libsiedler2::loadMapping(s, doNothing), std::runtime_error,
                          makeExceptionCheck("No index or value"));
    s.str("-12 ss");
    s.seekg(0);
    BOOST_CHECK_EXCEPTION(libsiedler2::loadMapping(s, doNothing), std::runtime_error,
                          makeExceptionCheck("Invalid index: -12"));
    s.str("ff ss");
    s.seekg(0);
    BOOST_CHECK_EXCEPTION(libsiedler2::loadMapping(s, doNothing), std::runtime_error,
                          makeExceptionCheck("Invalid index: ff"));
    // Correct line reported
    s.str("-1 ss\n2 d");
    s.seekg(0);
    BOOST_CHECK_EXCEPTION(libsiedler2::loadMapping(s, doNothing), std::runtime_error,
                          makeExceptionCheck("Error at line 1"));
    s.str("#\n-1 ss\n2 d");
    s.seekg(0);
    BOOST_CHECK_EXCEPTION(libsiedler2::loadMapping(s, doNothing), std::runtime_error,
                          makeExceptionCheck("Error at line 2"));
    s.str("0 d\n#\n-1 ss\n2 d");
    s.seekg(0);
    BOOST_CHECK_EXCEPTION(libsiedler2::loadMapping(s, doNothing), std::runtime_error,
                          makeExceptionCheck("Error at line 3"));
}

BOOST_AUTO_TEST_SUITE_END()
