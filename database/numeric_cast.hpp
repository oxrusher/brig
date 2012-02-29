// Andrew Naplavkov

#ifndef BRIG_DATABASE_NUMERIC_CAST_HPP
#define BRIG_DATABASE_NUMERIC_CAST_HPP

#include <boost/variant/apply_visitor.hpp>
#include <brig/database/detail/numeric_visitor.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database {

template <typename To>
bool numeric_cast(const variant& from, To& to)  { return ::boost::apply_visitor(detail::numeric_visitor<To>(to), from); }

} } // brig::database

#endif // BRIG_DATABASE_NUMERIC_CAST_HPP
