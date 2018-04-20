/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_ITERATOR_HH
#define UTIL_ITERATOR_HH

#include <vector>
#include <iterator>

template<class Iterator>
class JoinIterator
{
private:
  std::vector<std::pair<Iterator, Iterator>> iterators_;
  size_t current_;

public:
  typedef std::forward_iterator_tag iterator_category;
  typedef typename std::iterator_traits<Iterator>::value_type value_type;
  typedef typename std::iterator_traits<Iterator>::pointer pointer;
  typedef typename std::iterator_traits<Iterator>::reference reference;

  JoinIterator( Iterator t1_begin, Iterator t1_end,
                Iterator t2_begin, Iterator t2_end, const size_t current = 0 )
    : iterators_( { { t1_begin, t1_end }, { t2_begin, t2_end } } ),
      current_( current )
  {}

  JoinIterator & operator++()
  {
    if ( current_ == iterators_.size() ) {
      return *this;
    }

    if ( iterators_.at( current_ ).first != iterators_.at( current_ ).second ) {
      iterators_.at( current_ ).first++;
    }
    else {
      current_++;
    }

    return *this;
  }

  reference operator*()
  {
    auto & c = iterators_[ current_ ];

    if ( c.first != c.second ) {
      return *c.first;
    }

    return *iterators_[ current_ + 1 ].first;
  }

  pointer operator->()
  {
    auto & c = iterators_[ current_ ];

    if ( c.first != c.second ) {
      return &( *c.first );
    }

    return &( *iterators_[ current_ + 1 ].first );
  }

  bool operator==( const JoinIterator & other )
  {
    if ( current_ != other.current_ or
         iterators_.size() != other.iterators_.size() ) {
      return false;
    }

    for ( size_t i = 0; i < iterators_.size(); i++ ) {
      if ( iterators_[ i ].first != other.iterators_[ i ].first or
           iterators_[ i ].second != other.iterators_[ i ].second ) {
        return false;
      }
    }

    return true;
  }

  bool operator!=( const JoinIterator & other ) { return not operator==( other ); }
};

template<class Iterator>
class JoinContainer
{
private:
  JoinIterator<Iterator> begin_;
  JoinIterator<Iterator> end_;

public:
  JoinContainer( Iterator c1_begin, Iterator c1_end,
                 Iterator c2_begin, Iterator c2_end )
    : begin_( c1_begin, c1_end, c2_begin, c2_end ),
      end_( c1_end, c1_end, c2_end, c2_end, 1 )
  {}

  JoinIterator<Iterator> begin() { return begin_; }
  JoinIterator<Iterator> end() { return end_; }
};

template<class Container>
JoinContainer<typename Container::iterator> join_containers( Container & c1, Container & c2 )
{
  return { c1.begin(), c1.end(), c2.begin(), c2.end() };
}

template<class Container>
JoinContainer<typename Container::const_iterator> join_containers( const Container & c1, const Container & c2 )
{
  JoinContainer<typename Container::const_iterator> x( c1.cbegin(), c1.cend(), c2.cbegin(), c2.cend() );
  return x;
}


#endif /* UTIL_ITERATOR_HH */
