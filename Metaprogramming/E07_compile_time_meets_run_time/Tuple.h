#pragma once

#include "Metaprogramming.h"
#include <functional>
#include <type_traits>

namespace bits_of_q
{
    template <typename... ELEMS>
    struct Tuple
    {
        constexpr Tuple() = default;
    };

    // specialization for tuple with at least one element
    template <typename ELEM0, typename... ELEMS1toN>
    struct Tuple<ELEM0, ELEMS1toN...> : Tuple<ELEMS1toN...>
    {
        ELEM0 data;

        template <typename T, typename... Ts>
        explicit constexpr Tuple(T &&e0, Ts &&...rest) : Tuple<ELEMS1toN...>(std::forward<Ts>(rest)...), data(std::forward<T>(e0))
        {
            // std::forward keeps the value category of its argument
            // lvalue will be passed as lvalue, rvalue will be passed as rvalue.
        }
    };

    // deduction guide to make template argument deduction for constructors work (C++17)
    template <typename T, typename... Ts>
    // Tuple(T, Ts...) -> Tuple<T, Ts...>;
    Tuple(T, Ts...) -> Tuple<std::unwrap_ref_decay_t<T>, std::unwrap_ref_decay_t<Ts>...>;

    // See cpp reference (https://en.cppreference.com/w/cpp/utility/functional/unwrap_reference.html)
    static_assert(std::is_same_v<std::unwrap_ref_decay_t<int>, int>);
    static_assert(std::is_same_v<std::unwrap_ref_decay_t<const int>, int>);
    static_assert(std::is_same_v<std::unwrap_ref_decay_t<const int &>, int>);

    template <typename... ELEMS>
    constexpr auto
    make_tuple(ELEMS &&...elems)
    {
        return Tuple<std::unwrap_ref_decay_t<ELEMS>...>{std::forward<ELEMS>(elems)...};
    }

    // get

    namespace detail
    {
        template <size_t i, typename TUPLE>
        struct get_impl : get_impl<i - 1, pop_front_t<TUPLE>>
        {
        };

        template <typename TUPLE>
        struct get_impl<0, TUPLE>
        {
            template <typename T>
            constexpr static decltype(auto)
            get(T &&t)
            {
                // See cpp reference (https://en.cppreference.com/w/cpp/types/is_lvalue_reference.html)
                static_assert(std::is_lvalue_reference_v<int> == false);
                static_assert(std::is_lvalue_reference_v<int &> == true);
                static_assert(std::is_lvalue_reference_v<int &&> == false);

                constexpr bool T_is_lvalue_ref = std::is_lvalue_reference_v<T>;
                constexpr bool T_is_const      = std::is_const_v<std::remove_reference_t<T>>;

                using data_t = front_t<TUPLE>;

                // return proper casts
                if constexpr (T_is_lvalue_ref && T_is_const)
                {
                    return static_cast<const data_t &>(static_cast<const TUPLE &>(t).data);
                }
                else if constexpr (T_is_lvalue_ref && !T_is_const)
                {
                    return static_cast<data_t &>(static_cast<TUPLE &>(t).data);
                }
                else if constexpr (!T_is_lvalue_ref && T_is_const)
                {
                    return static_cast<const data_t &&>(static_cast<const TUPLE &&>(t).data);
                }
                else if constexpr (!T_is_lvalue_ref && !T_is_const)
                {
                    return static_cast<data_t &&>(static_cast<TUPLE &&>(t).data);
                }
            }
        };
    } // namespace detail

    // decltype(auto) (https://stackoverflow.com/a/24109800/19143961)
    // In generic code: Perfectly forward a return type without knowing whether you are dealing with a reference or a value.

    template <size_t i, typename TUPLE>
    constexpr decltype(auto)
    get(TUPLE &&tuple)
    {
        return detail::get_impl<i, std::remove_cvref_t<TUPLE>>::get(std::forward<TUPLE>(tuple));
    }
} // namespace bits_of_q
