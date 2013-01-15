// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_ISO_TYPE_HPP
#define BRIG_DATABASE_DETAIL_GET_ISO_TYPE_HPP

#include <brig/global.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline column_type get_iso_type(const std::string& type_lcase, int scale)
{
  using namespace std;

  if ((type_lcase.find("int") != string::npos && type_lcase.find("interval") == string::npos)
    || type_lcase.find("bool") == 0) return Integer;
  else if (type_lcase.find("date") != string::npos
        || type_lcase.find("time") != string::npos
        || type_lcase.find("char") != string::npos
        || type_lcase.find("clob") != string::npos
        || type_lcase.find("text") != string::npos) return String;
  else if (type_lcase.find("real") != string::npos
        || type_lcase.find("float") != string::npos
        || type_lcase.find("double") != string::npos) return Double;
  else if (type_lcase.find("dec") == 0
        || type_lcase.find("num") == 0) return scale == 0? Integer: Double;
  else if (type_lcase.find("binary") != string::npos
        || type_lcase.find("blob") != string::npos) return Blob;
  else return VoidColumn;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_ISO_TYPE_HPP
