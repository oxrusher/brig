// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_ISO_TYPE_HPP
#define BRIG_DATABASE_DETAIL_GET_ISO_TYPE_HPP

#include <brig/database/global.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline column_type get_iso_type(const std::string& dbms_type_name_lcase, int scale)
{
  using namespace std;

  if ((dbms_type_name_lcase.find("int") != string::npos && dbms_type_name_lcase.find("interval") == string::npos)
   || dbms_type_name_lcase.find("bool") == 0) return Integer;
  else if (dbms_type_name_lcase.find("date") != string::npos
        || dbms_type_name_lcase.find("time") != string::npos
        || dbms_type_name_lcase.find("char") != string::npos
        || dbms_type_name_lcase.find("clob") != string::npos
        || dbms_type_name_lcase.find("text") != string::npos) return String;
  else if (dbms_type_name_lcase.find("real") != string::npos
        || dbms_type_name_lcase.find("float") != string::npos
        || dbms_type_name_lcase.find("double") != string::npos) return Double;
  else if (dbms_type_name_lcase.find("dec") == 0
        || dbms_type_name_lcase.find("num") == 0) return scale == 0? Integer: Double;
  else if (dbms_type_name_lcase.find("binary") != string::npos
        || dbms_type_name_lcase.find("blob") != string::npos) return Blob;
  else return VoidColumn;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_ISO_TYPE_HPP
