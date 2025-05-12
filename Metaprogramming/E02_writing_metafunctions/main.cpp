#include <iostream>
#include <string>
#include <tuple>
#include <vector>

// if_

template <bool condition, typename THEN, typename ELSE>
struct if_;

template <typename THEN, typename ELSE>
struct if_<true, THEN, ELSE>
{
    using type = THEN;
};

template <typename THEN, typename ELSE>
struct if_<false, THEN, ELSE>
{
    using type = ELSE;
};

template <bool condition, typename THEN, typename ELSE>
using if_t = if_<condition, THEN, ELSE>::type;

// contains_type

template <typename SEARCH, typename TUPLE, size_t start_from = 0>
struct contains_type : if_t< // IF
                           std::is_same_v<std::tuple_element_t<start_from, TUPLE>, SEARCH>,
                           // THEN
                           std::true_type,
                           // ELSE
                           if_t<
                               // IF
                               (start_from == std::tuple_size<TUPLE>::value - 1),
                               // THEN
                               std::false_type,
                               // ELSE
                               contains_type<SEARCH, TUPLE, start_from + 1>>>
{
};

template <typename SEARCH>
struct contains_type<SEARCH, std::tuple<>, 0> : std::false_type
{
};

template <typename SEARCH, typename TUPLE, size_t start_from = 0>
inline constexpr bool contains_type_v = contains_type<SEARCH, TUPLE, start_from>::value;

// contains

bool
contains(const std::string &search, const std::vector<std::string> &v, size_t start_from = 0)
{
    if (v[start_from] == search)
    {
        return true;
    }

    if (start_from == v.size() - 1)
    {
        return false; // we are at the last element
    }

    return contains(search, v, start_from + 1);
}

int
main()
{
    std::cout << std::boolalpha;

    std::vector<std::string> vec{"int", "bool", "float"};
    std::cout << contains("bool", vec) << '\n';                     // true
    std::cout << contains("double", vec) << '\n';                   // false

    std::tuple<int, bool, float> tuple1;
    std::cout << contains_type_v<bool, decltype(tuple1)> << '\n';   // true
    std::cout << contains_type_v<double, decltype(tuple1)> << '\n'; // false

    std::tuple<> tuple2;
    std::cout << contains_type_v<double, decltype(tuple2)> << '\n'; // false
}
