// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_enum.hpp>

#include "test_parser.hpp"

using namespace cppast;

TEST_CASE("cpp_enum")
{
    auto code = R"(
enum ignore_me : int;

enum a
{
    a_a,
    a_b = 42,
    a_c,
    a_d = a_a + 2,
};

enum class ignore_me2;

enum class b : int
{
    b_a,
    b_b = 42,
    b_c
};

)";

    cpp_entity_index idx;
    auto             file = parse(idx, "cpp_enum.cpp", code);
    auto count            = test_visit<cpp_enum>(*file, [&](const cpp_enum& e) {
        if (e.name() == "a")
        {
            REQUIRE(!e.is_scoped());
            REQUIRE(!e.underlying_type());

            auto no_vals = 0u;
            for (auto& val : e)
            {
                if (val.name() == "a_a" || val.name() == "a_c")
                {
                    ++no_vals;
                    REQUIRE(!val.value());
                }
                else if (val.name() == "a_b")
                {
                    ++no_vals;
                    REQUIRE(val.value());
                    auto& expr = val.value().value();
                    REQUIRE(expr.kind() == cpp_expression_kind::unexposed);
                    REQUIRE(static_cast<const cpp_unexposed_expression&>(expr).expression()
                            == "42");
                    REQUIRE(
                        equal_types(idx, expr.type(), *cpp_builtin_type::build("unsigned int")));
                }
                else if (val.name() == "a_d")
                {
                    ++no_vals;
                    REQUIRE(val.value());
                    auto& expr = val.value().value();
                    // this is unexposed for some reason
                    REQUIRE(expr.kind() == cpp_expression_kind::unexposed);
                    REQUIRE(static_cast<const cpp_unexposed_expression&>(expr).expression()
                            == "a_a+2");
                    REQUIRE(
                        equal_types(idx, expr.type(), *cpp_builtin_type::build("unsigned int")));
                }
                else
                    REQUIRE(false);
            }
            REQUIRE(no_vals == 4u);
        }
        else if (e.name() == "b")
        {
            REQUIRE(e.is_scoped());
            REQUIRE(e.underlying_type());
            REQUIRE(equal_types(idx, e.underlying_type().value(), *cpp_builtin_type::build("int")));

            auto no_vals = 0u;
            for (auto& val : e)
            {
                REQUIRE(full_name(val) == "b::" + val.name());
                if (val.name() == "b_a" || val.name() == "b_c")
                {
                    ++no_vals;
                    REQUIRE(!val.value());
                }
                else if (val.name() == "b_b")
                {
                    ++no_vals;
                    REQUIRE(val.value());
                    auto& expr = val.value().value();
                    REQUIRE(expr.kind() == cpp_expression_kind::literal);
                    REQUIRE(static_cast<const cpp_literal_expression&>(expr).value() == "42");
                    REQUIRE(equal_types(idx, expr.type(), *cpp_builtin_type::build("int")));
                }
                else
                    REQUIRE(false);
            }
            REQUIRE(no_vals == 3u);
        }
        else
            REQUIRE(false);
    });
    REQUIRE(count == 2u);
}