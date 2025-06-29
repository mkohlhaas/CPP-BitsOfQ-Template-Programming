#include "TestUtilities.h"
#include "Tuple.h"

int
main()
{
    using namespace bits_of_q;

    {
        auto tup = make_tuple(8, true, 1.5);
        std::cout << get<2>(tup) << std::endl;            // 1.5

        const Tuple const_tup{1, false, 4};
        std::cout << get<2>(const_tup) << std::endl;      // 4

        std::cout << get<2>(Tuple{2, 3, 8}) << std::endl; // 8
        get<2>(tup) = 4.2;
        std::cout << get<2>(tup) << std::endl;            // 4.2
    }

    {
        CopyCounter c;
        CopyCounter::reset();

        make_tuple(8, c, 1.5);
        CopyStats our_stats = c.stats;
        CopyCounter::reset();

        std::make_tuple(8, c, 1.5);
        CopyStats std_stats = c.stats;

        std::cout << "bits_of_q::make_tuple: " << our_stats << std::endl;
        std::cout << "std::make_tuple:       " << std_stats << std::endl;

        assert(our_stats == std_stats);
    }

    {
        auto tup = make_tuple(8, true, 1.5);

        CopyCounter c;
        CopyCounter::reset();

        get<1>(tup);
        CopyStats our_stats = CopyCounter::stats;

        auto std_tup = std::make_tuple(8, c, 1.5);
        CopyCounter::reset();

        get<1>(std_tup);
        CopyStats std_stats = CopyCounter::stats;

        std::cout << "bits_of_q::get:        " << our_stats << std::endl;
        std::cout << "std::get:              " << std_stats << std::endl;

        assert(our_stats == std_stats);
    }
}
