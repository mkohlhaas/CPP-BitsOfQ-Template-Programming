#ifndef BOQ_NAIVE_LAZY_METAPROGRAMMING_H
#define BOQ_NAIVE_LAZY_METAPROGRAMMING_H

#include "TypeList.h"

namespace bits_of_q::naive_lazy
{
    // naive, direct implementation
    // using lazy instantiation to prevent O(2^n) type instantiations

    ////////////////////////////////// REMOVE_IF //////////////////////////////////

    template <template <typename> class PREDICATE, typename LIST, typename NEW_LIST = type_list<>>
    struct remove_if;

    template <template <typename> class PREDICATE, typename T0, typename... T1toN, typename NEW_LIST>
    struct remove_if<PREDICATE, type_list<T0, T1toN...>, NEW_LIST>
    {
        using type = typename if_<PREDICATE<T0>::value, remove_if<PREDICATE, type_list<T1toN...>, NEW_LIST>,
                                  remove_if<PREDICATE, type_list<T1toN...>, push_back_t<NEW_LIST, T0>>>::type::type;
    };

    template <template <typename> class PREDICATE, typename NEW_LIST>
    struct remove_if<PREDICATE, type_list<>, NEW_LIST>
    {
        using type = NEW_LIST;
    };

    template <template <typename> class PREDICATE, typename LIST>
    using remove_if_t = typename remove_if<PREDICATE, LIST>::type;

} // namespace bits_of_q::naive_lazy

#endif
