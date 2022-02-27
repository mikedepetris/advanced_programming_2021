#include "stack_pool.hpp"
#include "ap_error.hpp"
#include <iostream>
#include <cassert>
#include <algorithm> // max_element, min_element

/*
 * checks for memory leaks
 *
    valgrind ./check_stack_pool.x
    valgrind ./check_const_iterator.x
    valgrind ./check_swap.x
    g++ -g -fsanitize=address check_stack_pool.cpp
    g++ -g -fsanitize=address check_const_iterator.cpp
    g++ -g -fsanitize=address check_swap.cpp

    unit tests: "make check"
*/

// using namespace stackpool;

void print_line(const std::string &s) {
    std::cout << "\n" << s << std::endl;
}

int main() {
    print_line("Testing the pool");
    {
        print_line("SCENARIO(\"getting confident with the addresses\")");
        stack_pool<int, std::size_t> pool{16};
        auto l = pool.new_stack();
        std::cout << "REQUIRE( l == 0): l=" << l << std::endl;
        l = pool.push(42, l);
        std::cout << "REQUIRE( l == std::size_t(1)): l=" << l << std::endl;
        pool.print_pool_int();
        pool.print_freenodes_int();
        pool.print_stack_int(l);
    }
    {
        print_line("from readme");
        stack_pool<int> pool{22};
        auto l1 = pool.new_stack();
        l1 = pool.push(3, l1);
        l1 = pool.push(1, l1);
        l1 = pool.push(4, l1);
        l1 = pool.push(1, l1);
        l1 = pool.push(5, l1);
        l1 = pool.push(9, l1);
        l1 = pool.push(2, l1);
        l1 = pool.push(6, l1);
        l1 = pool.push(5, l1);
        l1 = pool.push(3, l1);
        l1 = pool.push(5, l1);

        auto l2 = pool.new_stack();
        l2 = pool.push(8, l2);
        l2 = pool.push(9, l2);
        l2 = pool.push(7, l2);
        l2 = pool.push(9, l2);
        l2 = pool.push(3, l2);
        l2 = pool.push(1, l2);
        l2 = pool.push(1, l2);
        l2 = pool.push(5, l2);
        l2 = pool.push(9, l2);
        l2 = pool.push(9, l2);
        l2 = pool.push(7, l2);

        auto constructed_not_assigned{pool.begin(l1)};

        auto M = std::max_element(pool.begin(l1), pool.end(l1));
        assert(*M == 9);
        std::cout << "*M == 9: " << ((*M == 9) ? "True" : "False") << std::endl;

        auto m = std::min_element(pool.begin(l2), pool.end(l2));
        assert(*m == 1);
        std::cout << "*m == 1: " << ((*m == 1) ? "True" : "False") << std::endl;

        pool.print_pool_int();
        pool.print_freenodes_int();
        pool.print_stack_int(l1);
        pool.print_stack_int(l2);
    }
    {
        print_line("test what happens when the pool is full");
        stack_pool<int> pool{10};
        auto l = pool.new_stack();
        std::cout << "pool capacity=" << pool.capacity() << std::endl;
        for (int i = 0; i < 9; ++i) // first 9 of 10 (0..8)
            l = pool.push(i + 1, l); // (1..9)
        pool.print_pool_int();
        pool.print_freenodes_int();
        pool.print_stack_int(l);
        std::cout << "pool capacity=" << pool.capacity() << std::endl;
        l = pool.push(10, l); // use the last available
        pool.print_pool_int();
        pool.print_freenodes_int();
        pool.print_stack_int(l);
        std::cout << "pool capacity=" << pool.capacity() << std::endl;
        l = pool.push(11, l); // here is full
        pool.print_pool_int();
        pool.print_freenodes_int();
        pool.print_stack_int(l);
        std::cout << "pool capacity=" << pool.capacity() << std::endl;
    }
    {
        print_line("another capacity test with vectors with pool{4}, v{1, 2, 3, 4, 5}");
        stack_pool<std::vector<int>, std::size_t> pool{4};
        auto l1 = pool.new_stack();
        auto l2 = pool.new_stack();
        std::vector<int> v{1, 2, 3, 4, 5};
        for (int i = 1; i <= 10; ++i) {
            l1 = pool.push(v, l1);
            std::cout << "i=" << i << ": v pushed to l1" << std::endl;
            l2 = pool.push(v, l2);
            std::cout << "i=" << i << ": v pushed to l2" << std::endl;
            if (i % 2) {
                std::cout << "pool capacity before l1 pop=" << pool.capacity() << std::endl;
                l1 = pool.pop(l1);
                // std::cout << "l1 popped" << "\n";
                std::cout << "pool capacity after l1 pop=" << pool.capacity() << std::endl;
            }
        }
        pool.print_pool_vector();
        pool.print_freenodes_vector();
        pool.print_stack_vector(l1);
        pool.print_stack_vector(l2);
    }
    {
        print_line("several tests with vectors");

        stack_pool<int, std::size_t> pool{};
        stack_pool<int, std::size_t> pool2{41};
        stack_pool<int, std::size_t> pool3;
        auto l1 = pool.new_stack();
        // l1 == pool.end() == std::size_t(0)
        std::cout << "new_stack l1 == pool.end() == std::size_t(0) =" << l1 << std::endl;

        l1 = pool.push(42, l1);
        // l1 == std::size_t(1)
        std::cout << "after push 42: l1 == std::size_t(1) =" << l1 << std::endl;
        pool.print_pool_int();
        pool.print_freenodes_int();

        std::cout << "pool.value(l1)=" << pool.value(l1) << std::endl;
        pool.value(l1) = 77;
        std::cout << "l1 value set to 77: pool.value(l1)=" << pool.value(l1) << std::endl;
        pool.print_pool_int();

        l1 = pool.push(43, l1);
        std::cout << "after push 43: l1 == std::size_t(2) =" << l1 << std::endl;
        pool.print_pool_int();
        pool.print_freenodes_int();

        l1 = pool.push(10, l1);
        l1 = pool.push(11, l1);
        std::cout << "after push 10 11: l1=" << l1 << std::endl;
        pool.print_pool_int();
        pool.print_freenodes_int();

        auto l2 = pool.new_stack();
        std::cout << "new_stack l2=" << l2 << std::endl;
        try {
            std::cout << "pool.value(l2)=" << pool.value(l2) << std::endl;
        } catch (const Stack_pool_exception &e) {
            std::cout << "can't get value: " << e.what() << std::endl;
            // std::cerr << e.what() << std::endl;
            // do not exit, go on testing, can't get value
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return 1;
        } catch (...) {
            std::cerr << "Unknown exception. Aborting.\n";
            return 3;
        }
        try {
            pool.value(l2) = 88;
            std::cout << "l2 value set to 88: pool.value(l2)=" << pool.value(l2) << std::endl;
        } catch (const Stack_pool_exception &e) {
            std::cout << "can't set value: " << e.what() << std::endl;
            // std::cerr << e.what() << std::endl;
            // do not exit, go on testing discarding value 88
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            // return 1; do not exit, go on testing discarding value 88
        } catch (...) {
            std::cerr << "Unknown exception. Aborting.\n";
            return 3;
        }
        pool.print_pool_int();
        pool.print_freenodes_int();

        l2 = pool.push(20, l2);
        std::cout << "after l2 push 20: l1=" << l1 << std::endl;
        std::cout << "after l2 push 20: l2=" << l2 << std::endl;
        std::cout << "pool.value(l2)=" << pool.value(l2) << std::endl;
        pool.print_pool_int();
        pool.print_freenodes_int();

        l1 = pool.pop(l1);
        std::cout << "after l1 pop: l1=" << l1 << std::endl;
        std::cout << "after l1 pop: l2=" << l2 << std::endl;
        std::cout << "pool.value(l1)=" << pool.value(l1) << std::endl;
        pool.print_pool_int();
        pool.print_stack_int(l1);
        pool.print_stack_int(l2);
        pool.print_freenodes_int();

        l2 = pool.push(21, l2);
        std::cout << "after l2 push 21: l1=" << l1 << std::endl;
        std::cout << "after l2 push 21: l2=" << l2 << std::endl;
        std::cout << "pool.value(l2)=" << pool.value(l2) << std::endl;
        pool.print_pool_int();
        pool.print_stack_int(l1);
        pool.print_stack_int(l2);
        pool.print_freenodes_int();

        l1 = pool.free_stack(l1);
        std::cout << "after l1 free_stack: l1=" << l1 << std::endl;
        std::cout << "after l1 free_stack: l2=" << l2 << std::endl;
        pool.print_pool_int();
        pool.print_stack_int(l1);
        pool.print_stack_int(l2);
        pool.print_freenodes_int();
    }
    return 0;
}
