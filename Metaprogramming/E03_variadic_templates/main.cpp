#include <iostream>
#include <tuple>
#include <type_traits>

// Generic Print

void
printn()
{
    std::cout << '\n';
}

template <typename LAST>
void
printn(LAST &&t)
{
    std::cout << std::forward<LAST>(t) << '\n';
}

template <typename T0, typename... T1toN>
void
printn(T0 &&t, T1toN &&...rest)
{
    std::cout << std::forward<T0>(t) << ", ";
    printn(std::forward<T1toN>(rest)...); // printn(std::forward<T1>(rest1), std::forward<T2>(rest2), ...)
}

// Tuple Print

template <typename TUPLE, std::size_t... indices>
void
print_tuple_impl(TUPLE &&t, std::index_sequence<indices...>)
{
    printn(std::get<indices>(std::forward<TUPLE>(t))...); // printn(std::get<0>(t), std::get<1>(t), std::get<2>(t)...);
}

template <typename TUPLE>
void
print_tuple(TUPLE &&t)
{
    print_tuple_impl(std::forward<TUPLE>(t),
                     std::make_index_sequence<std::tuple_size<std::remove_reference_t<TUPLE>>::value>{});
}

int
main()
{
    std::cout << std::boolalpha;
    const auto tuple = std::make_tuple(9, "hello", 1.5, true);
    print_tuple(tuple);            // 9, hello, 1.5, true
    printn(9, "hello", 1.5, true); // 9, hello, 1.5, true
    printn();                      //
    printn("hello", "world");      // hello, world
}
