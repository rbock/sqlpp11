#pragma once

#include <sqlpp11/postgresql/postgresql.h>

class db_connection
{
private:
  using pq_conn = sqlpp::postgresql::pooled_connection;

  sqlpp::postgresql::connection_pool& m_pool;
  // For C++17 or newer just use std::optional<pq_conn> m_conn;
  std::unique_ptr<pq_conn> m_conn_ptr;

  pq_conn& fetch();

public:
  db_connection(sqlpp::postgresql::connection_pool& pool);
  db_connection(const db_connection&) = delete;
  db_connection(db_connection&&) = delete;

  db_connection& operator=(const db_connection&) = delete;
  db_connection& operator=(db_connection&&) = delete;

  // Delegate any methods of sqlpp::postgresql::connection that you may need

  template <typename T>
  auto operator()(const T& t) -> decltype(fetch()(t))
  {
    return fetch()(t);
  }
};
