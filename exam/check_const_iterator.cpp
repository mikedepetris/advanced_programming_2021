#include "stack_pool.hpp"
#include <iostream>
#include <utility>

// using namespace stackpool;

template<typename T>
void print_vector(const std::vector<T> &v, const std::string &s);

void iterator_normal_pool_integer_test(stack_pool<int, std::size_t> &pool, std::size_t head) {
    for (auto i = pool.begin(head); i != pool.end(head); ++i) {
        *i = 33; // yes we can!
        std::cout << *i << std::endl;
    }
}

void iterator_const_pool_integer_test(const stack_pool<int, std::size_t> &pool, std::size_t head) {
    for (auto i = pool.begin(head); i != pool.end(head); ++i) {
        //*i = 33; // Cannot assign to return value because function 'operator*' returns a const value
        //            compile error: assignment of read-only location
        std::cout << *i << std::endl;
    }
}

void iterator_normal_pool_vector_int_test(stack_pool<std::vector<int>, std::size_t> &pool, std::size_t head) {
    // stack_pool_iterator<      stack_pool<value_type, stack_type>,       std::vector<int>, unsigned long>
    // stack_pool_iterator<const stack_pool<value_type, stack_type>, const std::vector<int>, unsigned long>
    auto normal_iterator = pool.begin(head);
    auto const_iterator = pool.cbegin(head);
    std::cout << "normal_iterator == const_iterator: "
              << (*normal_iterator == *const_iterator ? "True" : "False") << std::endl;
    for (auto i = pool.begin(head); i != pool.end(head); ++i) {
        std::vector<int> v{99, 88, 77};
        *i = v; // yes we can!
        print_vector(*i, "vector=");
    }
    for (auto i = pool.cbegin(head); i != pool.cend(head); ++i) {
        std::vector<int> v{99, 88, 77};
        // *i = v; // error! using cbegin cend
        print_vector(*i, "vector=");
    }
}

void iterator_const_pool_vector_int_test(const stack_pool<std::vector<int>, std::size_t> &pool, std::size_t head) {
    for (auto i = pool.begin(head); i != pool.end(head); ++i) {
        //for (auto i = pool.cbegin(head); i != pool.cend(head); ++i) {
        std::vector<int> v{999, 888, 777};
        //*i = v; //No viable overloaded '=' candidate function not viable: 'this' argument has type 'stack_pool_iterator<const stack_pool<std::vector<int>>, const std::vector<int>, unsigned long>::value_type' (aka 'const std::vector<int>'), but method is not marked const
        print_vector(*i, "vector=");
    }
}

template<typename T>
void print_vector(const std::vector<T> &v, const std::string &s) {
    std::cout << s;
    for (const auto &x: v)
        std::cout << x << " ";
    std::cout << std::endl;
}

int main() {
    stack_pool<std::vector<int>, std::size_t> pool{7};
    auto l1 = pool.new_stack();
    auto l2 = pool.new_stack();
    std::vector<int> v{1, 2, 3, 4, 5};
    for (int i = 1; i <= 10; ++i) {
        ++v[0];
        l1 = pool.push(v, l1);
//        std::cout << "i=" << i << ": v pushed to l1" << std::endl;
        ++v[1];
        l2 = pool.push(v, l2);
//        std::cout << "i=" << i << ": v pushed to l2" << std::endl;
        if (i % 2) {
//            std::cout << "pool capacity before l1 pop=" << pool.capacity() << std::endl;
            l1 = pool.pop(l1);
            // std::cout << "l1 popped" << "\n";
//            std::cout << "pool capacity after l1 pop=" << pool.capacity() << std::endl;
        }
    }
    pool.print_pool_vector();
    pool.print_freenodes_vector();
    pool.print_stack_vector(l1);
    pool.print_stack_vector(l2);

    iterator_normal_pool_vector_int_test(pool, l1);
    std::cout << "pool capacity after const_pool_vector_int_test l1=" << pool.capacity() << std::endl;
    pool.print_pool_vector();
    std::cout << "l1=" << l1 << std::endl;
    std::cout << "l2=" << l2 << std::endl;
    pool.print_stack_vector(l1);
    pool.print_stack_vector(l2);
    pool.print_freenodes_vector();
    auto x = pool.value(l1);
    print_vector(x, "vector value of l1=");

    iterator_const_pool_vector_int_test(pool, l2);
    std::cout << "pool capacity after const_pool_vector_int_test l2=" << pool.capacity() << std::endl;
    pool.print_pool_vector();
    std::cout << "l1=" << l1 << std::endl;
    std::cout << "l2=" << l2 << std::endl;
    pool.print_stack_vector(l1);
    pool.print_stack_vector(l2);
    pool.print_freenodes_vector();
    auto y = pool.value(l2);
    print_vector(y, "vector value of l2=");

    print_vector(pool.value(l1), "vector value of l1=");
    pool.value(l1).at(1) = 333;
    print_vector(pool.value(l1), "vector value of l1=");

    const std::vector<int> vec{77, 88, 99};
    pool.value(l1) = vec;
    print_vector(pool.value(l1), "vector value of l1=");

    {
        stack_pool<int, std::size_t> pool{5};
        auto l1 = pool.new_stack();
        l1 = pool.push(1, l1);
        l1 = pool.push(2, l1);
        l1 = pool.push(3, l1);
        pool.print_pool_int();
        pool.print_stack_int(l1);
        iterator_normal_pool_integer_test(pool, l1);
        pool.print_pool_int();
        pool.print_stack_int(l1);
        pool.print_freenodes_int();
    }
    return 0;
}
