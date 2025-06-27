#include <iostream>
#include <list>
#include <string>

// if_ ✓

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

// type_list ✓

template <typename...>
struct type_list
{
};

// empty ✓ (not needed any more - contains_type changed)

template <typename TL>
struct empty : std::false_type
{
};

template <>
struct empty<type_list<>> : std::true_type
{
};

template <typename TL>
constexpr bool empty_v = empty<TL>::value;

static_assert(empty_v<type_list<>>);          // empty
static_assert(not empty_v<type_list<float>>); // not empty

// front ✓

template <typename TL>
struct front;

template <typename T, typename... Ts>
struct front<type_list<T, Ts...>>
{
    using type = T;
};

template <typename TL>
using front_t = front<TL>::type; // ::type as type alias

static_assert(std::is_same_v<front_t<type_list<int, bool>>, int>);

// pop_front ✓

template <typename TL>
struct pop_front;

template <typename T, typename... Ts>
struct pop_front<type_list<T, Ts...>>
{
    using type = type_list<Ts...>;
};

template <typename TL>
using pop_front_t = typename pop_front<TL>::type;

static_assert(std::is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);
static_assert(std::is_same_v<front_t<pop_front_t<type_list<int, bool, float>>>, bool>);
static_assert(std::is_same_v<front_t<pop_front_t<pop_front_t<type_list<int, bool, float>>>>, float>);

// contains_type ✓

template <typename SEARCH, typename TL>
struct contains_type : if_t<std::is_same_v<SEARCH, front_t<TL>>,    // IF
                            std::true_type,                         // THEN
                            contains_type<SEARCH, pop_front_t<TL>>> // ELSE
{
};

// specialization for empty type_list
template <typename SEARCH>
struct contains_type<SEARCH, type_list<>> : std::false_type
{
};

template <typename SEARCH, typename TL>
constexpr bool contains_type_v = contains_type<SEARCH, TL>::value;

// contains ✓

bool
contains(const std::string &search, std::list<std::string> l)
{
    if (l.empty())
    {
        return false;
    }

    if (search == l.front())
    {
        return true;
    }

    l.pop_front();
    return contains(search, l);
}

int
main()
{
    std::cout << std::boolalpha;

    using list_strings = std::list<std::string>;
    list_strings list{"int", "bool", "double"};
    std::cout << contains("bool", list_strings{}) << std::endl;        // false
    std::cout << contains("bool", list) << std::endl;                  // true
    std::cout << contains("float", list) << std::endl;                 // false

    type_list<int, bool, double> types;
    std::cout << contains_type_v<bool, type_list<>> << std::endl;      // false
    std::cout << contains_type_v<bool, decltype(types)> << std::endl;  // true
    std::cout << contains_type_v<float, decltype(types)> << std::endl; // false
}
