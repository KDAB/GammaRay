#
# SPDX-FileCopyrightText: 2009 Johns Hopkins University (JHU), All Rights Reserved.
# SPDX-License-Identifier: LicenseRef-CISST

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with no warranty.
# The complete license can be found in LICENSES/LicenseRef-CISST.txt.
#
# --- end cisst license ---

include("${CMAKE_CURRENT_LIST_DIR}/Toolchain-QNX-common.cmake")

# QNX 6.6.0 ships with a QCC 4.7.3 (based on the same GCC version)
#
# HACK: CMake does not support getting compile features from QCC
#   work-around by telling CMake the available compile features manually

# cmake-lint: disable=C0301
set(CMAKE_CXX_COMPILE_FEATURES
    "cxx_std_98;cxx_template_template_parameters;cxx_std_11;cxx_alias_templates;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates"
)
set(CMAKE_CXX98_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters")
# cmake-lint: disable=C0301
set(CMAKE_CXX11_COMPILE_FEATURES
    "cxx_std_11;cxx_alias_templates;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
