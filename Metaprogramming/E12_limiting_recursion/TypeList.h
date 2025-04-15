#ifndef BOQ_TYPE_LIST_H
#define BOQ_TYPE_LIST_H

namespace bits_of_q
{

    template <typename...>
    struct type_list
    {
    };

    /// helper functions from MetaProgramming.h (copied here so we don't have to compile the whole metaprogramming
    /// library in our benchmarks) ///
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

    //

    template <typename LIST, typename T>
    struct push_back;

    template <template <typename...> class LIST, typename... T0toN, typename T>
    struct push_back<LIST<T0toN...>, T>
    {
        using type = LIST<T0toN..., T>;
    };

    template <typename LIST, typename T>
    using push_back_t = typename push_back<LIST, T>::type;

} // namespace bits_of_q

#endif
