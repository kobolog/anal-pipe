#ifndef __LINE_ITERATOR_HPP_INCLUDED__
#define __LINE_ITERATOR_HPP_INCLUDED__

#include <istream>

#include "common.hpp"

// std::istream iterator yielding one line at a time.
// --------------------------------------------------

namespace linkedin {

struct line_iterator:
    std::iterator<std::input_iterator_tag, std::string>
{
public:
    line_iterator():
        m_stream()
    { }

    line_iterator(boost::shared_ptr<std::istream> stream):
        m_stream(stream)
    {
        next();
    }

    line_iterator(const line_iterator& other):
        m_stream(other.m_stream),
        m_value(other.m_value)
    { }

    line_iterator& operator = (const line_iterator& other) {
        m_stream = other.m_stream;
        m_value = other.m_value;
        return *this;
    }

    bool operator == (const line_iterator& other) const {
        return m_stream == other.m_stream;
    }

    bool operator != (const line_iterator& other) const {
        return !operator==(other);
    }

    line_iterator& operator ++ () {
        next();
        return *this;
    }

    line_iterator operator ++ (int) {
        line_iterator it(*this);
        operator++();
        return it;
    }

    const std::string& operator * () const {
        return m_value;
    }

private:
    void next() {
        if(m_stream) {
            std::getline(*m_stream, m_value);

            if(m_stream->eof()) {
                m_stream.reset();
            }
        }
    }

    boost::shared_ptr<std::istream> m_stream;
    std::string m_value;
};

}

#endif
