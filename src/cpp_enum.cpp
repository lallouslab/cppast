// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_enum.hpp>

#include <cppast/cpp_entity_kind.hpp>

using namespace cppast;

std::unique_ptr<cpp_enum_value> cpp_enum_value::build(const cpp_entity_index& idx, cpp_entity_id id,
                                                      std::string                     name,
                                                      std::unique_ptr<cpp_expression> value)
{
    auto result =
        std::unique_ptr<cpp_enum_value>(new cpp_enum_value(std::move(name), std::move(value)));
    idx.register_entity(std::move(id), type_safe::ref(*result));
    return result;
}

cpp_entity_kind cpp_enum_value::do_get_entity_kind() const noexcept
{
    return cpp_entity_kind::enum_value_t;
}

cpp_entity_kind cpp_enum::do_get_entity_kind() const noexcept
{
    return cpp_entity_kind::enum_t;
}

type_safe::optional<std::string> cpp_enum::do_get_scope_name() const
{
    return scoped_ ? type_safe::make_optional(name()) : type_safe::nullopt;
}