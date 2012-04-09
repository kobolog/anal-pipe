#ifndef __SORTED_HEAP_HPP_INCLUDED__
#define __SORTED_HEAP_HPP_INCLUDED__

#include <queue>

#include "common.hpp"

// priority_queue-based sorted heap and auxiliary machinery to
// convert key-sorted aggregation maps to a value-sorted container.
// ----------------------------------------------------------------

namespace linkedin {

typedef std::pair<std::string, uint64_t> heap_element;

struct heap_compare {
    bool operator()(const heap_element& a, const heap_element& b) {
        return a.second < b.second;
    }
};

typedef std::priority_queue<
    heap_element,
    std::vector<heap_element>,
    heap_compare
> sorted_heap;

template<class Container>
struct push_iterator:
    std::iterator<std::output_iterator_tag, void, void, void, void>
{
    typedef Container container_type;

public:
    explicit push_iterator(Container& container):
        m_container(container)
    { }

    push_iterator& operator = (typename Container::const_reference value) {
        m_container.push(value);
        return *this;
    }

    push_iterator& operator * () {
        return *this;
    }

    push_iterator& operator ++ () {
        return *this;
    }

    push_iterator operator ++ (int) {
        return *this;
    }

private:
    Container& m_container;
};

template<class Container>
push_iterator<Container> pusher(Container& container) {
    return push_iterator<Container>(container);
}

}

#endif
