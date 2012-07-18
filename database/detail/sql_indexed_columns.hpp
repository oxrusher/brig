// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_INDEXED_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_SQL_INDEXED_COLUMNS_HPP

#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_indexed_columns(DBMS sys, const identifier& tbl)
{
  switch (sys)
  {
  default: throw std::runtime_error("SQL error");
  case CUBRID: return "SELECT '', i.index_name, i.is_primary_key, i.is_unique, 0, k.key_attr_name, k.asc_desc FROM _db_class c, _db_index i, _db_index_key k WHERE c.owner.name = '" + tbl.schema + "' AND c.class_name = '" + tbl.name + "' AND i.class_of = c AND k.index_of = i ORDER BY i.is_primary_key DESC, i.index_name, k.key_order";
  case DB2: return "SELECT RTRIM(i.INDSCHEMA), i.INDNAME, (CASE i.UNIQUERULE WHEN 'P' THEN 1 ELSE 0 END) pri, (CASE i.UNIQUERULE WHEN 'D' THEN 0 ELSE 1 END) unq, (CASE RTRIM(i.IESCHEMA) || '.' || i.IENAME WHEN 'DB2GSE.SPATIAL_INDEX' THEN 1 ELSE 0 END) sp, c.COLNAME, (CASE c.COLORDER WHEN 'D' THEN 1 ELSE 0 END) dsc FROM SYSCAT.INDEXES i, SYSCAT.INDEXCOLUSE c WHERE i.INDSCHEMA = c.INDSCHEMA AND i.INDNAME = c.INDNAME AND i.TABSCHEMA = '" + tbl.schema + "' AND i.TABNAME = '" + tbl.name + "' ORDER BY pri DESC, i.INDSCHEMA, i.INDNAME, c.COLSEQ";
  case Informix:
    {
    auto loc = std::locale::classic();
    std::ostringstream stream; stream.imbue(loc);
    for (int i(0); i < 16; ++i)
    {
      if (i > 0) stream << " UNION ALL ";
      stream << "SELECT RTRIM(i.owner), i.idxname, (CASE cs.constrtype WHEN 'P' THEN 1 ELSE 0 END) pri, (CASE i.idxtype WHEN 'U' THEN 1 ELSE 0 END) unq, 0 sp, c.colname, i.dsc, " << i << " keyno FROM (SELECT idxname, owner, tabid, idxtype, ABS(ikeyextractcolno(indexkeys, " << i << ")) colno, (CASE WHEN ikeyextractcolno(indexkeys, " << i << ") < 0 THEN 1 ELSE 0 END) dsc FROM sysindices) i JOIN (SELECT tabid FROM systables WHERE owner ='" + tbl.schema + "' AND tabname = '" + tbl.name + "') t ON i.tabid = t.tabid JOIN (SELECT tabid, colno, colname FROM syscolumns) c ON (i.tabid = c.tabid AND i.colno = c.colno) LEFT JOIN (SELECT tabid, idxname, constrtype FROM sysconstraints) cs ON (i.tabid = cs.tabid AND i.idxname = cs.idxname)";
    }
    stream << " UNION ALL ";
    stream << "SELECT RTRIM(i.owner), i.idxname, 0 pri, 0 unq, 1 sp, c.colname, 0 dsc, 0 keyno FROM (SELECT idxname, owner, tabid, CAST(SUBSTRING(CAST(indexkeys AS lvarchar) FROM 1 FOR 2) AS SMALLINT) colno, amid FROM sysindices) i, (SELECT tabid FROM systables WHERE owner = 'informix' AND tabname = 't') t, (SELECT am_id FROM sysams WHERE am_name = 'rtree') m, syscolumns c WHERE i.tabid = t.tabid AND i.amid = m.am_id AND i.tabid = c.tabid AND i.colno = c.colno";
    stream << " ORDER BY 3 DESC, 1, 2, 7";
    return stream.str();
    }
  // index name is unique only within the table
  case MS_SQL: return "SELECT '', i.name, i.is_primary_key, i.is_unique, (CASE i.type WHEN 4 THEN 1 ELSE 0 END) sp, COL_NAME(c.object_id, c.column_id) col, c.is_descending_key FROM sys.indexes i, sys.index_columns c WHERE i.object_id = OBJECT_ID('\"" + tbl.schema + "\".\"" + tbl.name + "\"') AND i.object_id = c.object_id AND i.index_id = c.index_id ORDER BY i.is_primary_key DESC, i.name, c.key_ordinal";
  // index name is unique only within the table; ASC or DESC are permitted for future extensions - currently (5.6) they are ignored
  case MySQL: return "SELECT '', INDEX_NAME, (INDEX_NAME = 'PRIMARY') pri, NOT NON_UNIQUE, INDEX_TYPE = 'SPATIAL', COLUMN_NAME, COLLATION = 'D' FROM INFORMATION_SCHEMA.STATISTICS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY pri DESC, INDEX_NAME, SEQ_IN_INDEX";
  case Oracle: return "SELECT i.OWNER, i.INDEX_NAME, (CASE cnt.CONSTRAINT_TYPE WHEN 'P' THEN 1 ELSE 0 END) pri, (CASE i.UNIQUENESS WHEN 'NONUNIQUE' THEN 0 ELSE 1 END) unq, (CASE i.ITYP_OWNER || '.' || i.ITYP_NAME WHEN 'MDSYS.SPATIAL_INDEX' THEN 1 ELSE 0 END) sp, c.COLUMN_NAME, (CASE c.DESCEND WHEN 'DESC' THEN 1 ELSE 0 END) dsc FROM (SELECT * FROM ALL_INDEXES WHERE TABLE_OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "') i LEFT JOIN (SELECT * FROM ALL_CONSTRAINTS WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "') cnt ON i.INDEX_NAME = cnt.INDEX_NAME JOIN ALL_IND_COLUMNS c ON i.OWNER = c.INDEX_OWNER AND i.INDEX_NAME = c.INDEX_NAME ORDER BY pri DESC, i.OWNER, i.INDEX_NAME, c.COLUMN_POSITION";
  case Postgres: return "SELECT scm, name, pri, unq, m.amname = 'gist', a.attname, opt & 1 FROM (SELECT y.*, y.keys[gs] AS key, y.opts[gs] AS opt FROM (SELECT x.*, generate_series(x.lb, x.ub) AS gs FROM (SELECT i.indisprimary AS pri, i.indisunique AS unq, i.indkey AS keys, i.indoption opts, array_lower(i.indkey, 1) AS lb, array_upper(i.indkey, 1) AS ub, o.relname AS name, o.relam AS mth, t.oid tbl, s.nspname AS scm FROM pg_catalog.pg_index i, pg_catalog.pg_class o, pg_catalog.pg_class t, pg_catalog.pg_namespace s WHERE i.indexrelid = o.oid AND i.indrelid = t.oid AND t.relnamespace = s.oid AND s.nspname = '" + tbl.schema + "' AND t.relname = '" + tbl.name + "') AS x) AS y) AS z, pg_catalog.pg_am m, pg_catalog.pg_attribute a WHERE m.oid = mth AND a.attrelid = tbl AND a.attnum = key ORDER BY pri DESC, scm, name, gs";
  }
  return "";
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_INDEXED_COLUMNS_HPP
