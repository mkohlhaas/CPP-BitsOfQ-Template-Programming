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

template <typename TYPE, typename TUPLE, size_t search_from_idx = 0>
struct contains_type : if_t< // IF
                           std::is_same_v<std::tuple_element_t<search_from_idx, TUPLE>, TYPE>,
                           // THEN
                           std::true_type,
                           // ELSE
                           if_t<
                               // IF
                               (search_from_idx == std::tuple_size<TUPLE>::value - 1),
                               // THEN
                               std::false_type,
                               // ELSE
                               contains_type<TYPE, TUPLE, search_from_idx + 1>>>
{
};

template <typename TYPE, size_t search_from_idx>
struct contains_type<TYPE, std::tuple<>, search_from_idx> : std::false_type
{
};

template <typename TYPE, typename TUPLE, size_t search_from_idx = 0>
inline constexpr bool contains_type_v = contains_type<TYPE, TUPLE, search_from_idx>::value;

// contains (using vectors)

bool
contains(const std::string &search, const std::vector<std::string> &v, size_t search_from_idx = 0)
{
    if (v[search_from_idx] == search)
    {
        return true;
    }

    if (search_from_idx == v.size() - 1)
    {
        return false; // we are at the last element
    }

    return contains(search, v, search_from_idx + 1);
}

int
main()
{
    std::cout << std::boolalpha;

    std::vector<std::string> vec{"int", "bool", "float"};
    std::cout << contains("bool", vec) << std::endl;                     // true
    std::cout << contains("double", vec) << std::endl;                   // false

    std::tuple<int, bool, float> tuple1;
    std::cout << contains_type_v<bool, decltype(tuple1)> << std::endl;   // true
    std::cout << contains_type_v<double, decltype(tuple1)> << std::endl; // false

    std::tuple<> tuple2;
    std::cout << contains_type_v<double, decltype(tuple2)> << std::endl; // false
}
