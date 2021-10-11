#include "BlobSample.h"
#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/sqlpp11.h>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <functional>
#include <iostream>
#include <random>

namespace sql = sqlpp::sqlite3;
const auto blob = BlobSample{};

/*
 * max default blob/text is 1,000,000,000
 * But 999,999,993 is the biggest one SQLITE will accept
 *  without throwing SQLITE_TOOBIG
 * But it takes too long (over 5s) to generate and check
 */
constexpr size_t blob_size = 1000 * 1000ul;
constexpr size_t blob_small_size = 999;

void verify_blob(sql::connection& db, const std::vector<uint8_t>& data, uint64_t id)
{
  auto result = db(select(blob.data).from(blob).where(blob.id == id));
  const auto& result_row = result.front();
  std::cerr << "Insert size: " << data.size() << std::endl;
  std::cerr << "Select size: " << result_row.data.len << std::endl;
  if (data.size() != result_row.data.len)
  {
    std::cerr << "Size mismatch" << std::endl;

    throw std::runtime_error("Size mismatch " + std::to_string(data.size()) +
                             " != " + std::to_string(result_row.data.len));
  }
  std::cerr << "Verifying content" << std::endl;
  std::vector<uint8_t> result_blob(result_row.data.blob, result_row.data.blob + result_row.data.len);
  if (data != result_blob)
  {
    std::cout << "Content mismatch ([row] original -> received)" << std::endl;

    for (size_t i = 0; i < data.size(); i++)
    {
      if (data[i] != result_row.data.blob[i])
      {
        std::cerr << "[" << i << "] " << static_cast<int>(data.at(i)) << " -> " << static_cast<int>(result_blob.at(i))
                  << std::endl;
      }
    }
    throw std::runtime_error("Content mismatch");
  }
}

int Blob(int, char*[])
{
  sql::connection_config config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = true;

  sql::connection db(config);
  db.execute(R"(CREATE TABLE blob_sample (
             id INTEGER PRIMARY KEY,
             data blob
           ))");
  std::cerr << "Generating data " << blob_size << std::endl;
  std::vector<uint8_t> data(blob_size);
  std::uniform_int_distribution<unsigned short> distribution(0, 255);
  std::mt19937 engine;
  auto generator = std::bind(distribution, engine);
  std::generate_n(data.begin(), blob_size, generator);

  std::vector<uint8_t> data_smaller(blob_small_size);
  std::generate_n(data_smaller.begin(), blob_small_size, generator);

  // If we use the bigger blob it will trigger SQLITE_TOOBIG for the query
  auto id = db(insert_into(blob).set(blob.data = data_smaller));

  auto prepared_insert = db.prepare(insert_into(blob).set(blob.data = parameter(blob.data)));
  prepared_insert.params.data = data;
  auto prep_id = db(prepared_insert);
  prepared_insert.params.data.set_null();
  auto null_id = db(prepared_insert);

  verify_blob(db, data_smaller, id);
  verify_blob(db, data, prep_id);
  {
    auto result = db(select(blob.data).from(blob).where(blob.id == null_id));
    const auto& result_row = result.front();
    std::cerr << "Null blob is_null:\t" << std::boolalpha << result_row.data.is_null() << std::endl;
    std::cerr << "Null blob len == 0:\t" << std::boolalpha << (result_row.data.len == 0) << std::endl;
    std::cerr << "Null blob blob == nullptr:\t" << std::boolalpha << (result_row.data.blob == nullptr) << std::endl;
    if (!result_row.data.is_null() || result_row.data.len != 0 || result_row.data.blob != nullptr)
    {
      throw std::runtime_error("Null blob has incorrect values");
    }
  }
  return 0;
}
