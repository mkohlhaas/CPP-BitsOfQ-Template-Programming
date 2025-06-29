#include <iostream>
#include <tuple>
#include <type_traits>

// printn

template <typename T>
void
printn(T &&t)
{
    std::cout << std::forward<T>(t) << std::endl;
}

template <typename T, typename... Ts>
void
printn(T &&t, Ts &&...ts)
{
    std::cout << std::forward<T>(t) << ", ";
    printn(std::forward<Ts>(ts)...);
}

// print_tuple

template <typename TUPLE, std::size_t... indices>
void
print_tuple_impl(TUPLE &&t, std::index_sequence<indices...>)
{
    printn(std::get<indices>(std::forward<TUPLE>(t))...);
}

template <typename TUPLE>
void
print_tuple(TUPLE &&t)
{
    std::cerr << __PRETTY_FUNCTION__ << std::endl;

    print_tuple_impl(std::forward<TUPLE>(t),
                     std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<TUPLE>>>{});
}

int
main()
{
    std::cout << std::boolalpha;

    const auto tuple = std::make_tuple(9, "hello", 1.5, true);

    print_tuple(std::make_tuple(9, "hello", 1.5, true)); // 9, hello, 1.5, true
    print_tuple(tuple);                                  // 9, hello, 1.5, true

    printn(9, "hello", 1.5, true);                       // 9, hello, 1.5, true
    printn("hello", "world");                            // hello, world
}
