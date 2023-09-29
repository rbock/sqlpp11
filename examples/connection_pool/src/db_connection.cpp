#include <memory>
#include <db_connection.h>

db_connection::pq_conn& db_connection::fetch()
{
  if (m_conn_ptr == nullptr)
  {
    m_conn_ptr = sqlpp::compat::make_unique<pq_conn>(m_pool.get());
  }
  return *m_conn_ptr;
}

db_connection::db_connection(sqlpp::postgresql::connection_pool& pool) : m_pool{pool}, m_conn_ptr{nullptr}
{
}
