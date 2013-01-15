// Andrew Naplavkov

#ifndef BRIG_DETAIL_NUMERIC_VISITOR_HPP
#define BRIG_DETAIL_NUMERIC_VISITOR_HPP

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <string>

namespace brig { namespace detail {

template <typename To>
struct numeric_visitor : ::boost::static_visitor<bool> {
  To& to;
  explicit numeric_visitor(To& to_) : to(to_)  {}
  bool operator()(const null_t&) const  { return false; }
  template <typename From>
  bool operator()(From) const;
  bool operator()(const std::string&) const;
  bool operator()(const blob_t&) const  { return false; }
}; // numeric_visitor

template <typename To>
  template <typename From>
bool numeric_visitor<To>::operator()(From from) const
{
  using namespace ::boost::numeric;
  if (converter<To, From>::out_of_range(from) != cInRange) return false;
  to = converter<To, From>::convert(from);
  return true;
}

template <typename To>
bool numeric_visitor<To>::operator()(const std::string& from) const
{
  try  { to = ::boost::lexical_cast<To>(from); return true; }
  catch (::boost::bad_lexical_cast&)  { return false; }
} // numeric_visitor::

} } // brig::detail

#endif // BRIG_DETAIL_NUMERIC_VISITOR_HPP
