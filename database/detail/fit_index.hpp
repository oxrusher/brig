// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_INDEX_HPP
#define BRIG_DATABASE_DETAIL_FIT_INDEX_HPP

#include <brig/database/global.hpp>
#include <brig/database/index_definition.hpp>
//#include <brig/string_cast.hpp>
//#include <brig/unicode/lower_case.hpp>
//#include <brig/unicode/transform.hpp>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline index_definition fit_index(const index_definition& idx, DBMS sys)
{
  //using namespace brig::unicode;

  index_definition idx;
  idx.type = idx_from->type;
  if (Primary != idx.type)
    switch (sys_to)
    {
    case VoidSystem:
    throw std::runtime_error("DBMS error");
    case CUBRID:
    case MS_SQL:
    case MySQL:
    idx.id.name = tbl_to.id.name + "_idx_" + string_cast<char>(++counter);
    break;
    case DB2:
    case Informix:
    case Ingres:
    case Oracle:
    case Postgres:
    idx.id.schema = tbl_to.id.schema;
    idx.id.name = tbl_to.id.name + "_idx_" + string_cast<char>(++counter);
    break;
    case SQLite:
    if (Spatial != idx.type) idx.id.name = tbl_to.id.name + "_idx_" + string_cast<char>(++counter);
    break;
    }

  for (auto col_from(std::begin(idx_from->columns)); col_from != std::end(idx_from->columns); ++col_from)
  {
    idx
  }

}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_INDEX_HPP
