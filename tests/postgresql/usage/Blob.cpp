/*
 * Copyright (c) 2019-2019, Jaroslav Bisikirski
 * Copyright (c) 2021-2021, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

#include <functional>
#include <iostream>
#include <random>

#include "BlobSample.h"
#include "make_test_connection.h"

namespace sql = sqlpp::postgresql;
const auto blob = model::BlobSample{};

// This would be a great fuzzing target :-)
constexpr size_t blob_size = 1000 * 1000ul;
constexpr size_t blob_small_size = 999;

void verify_blob(sql::connection& db, const std::vector<uint8_t>& data, uint64_t id)
{
  auto result = db(select(blob.data).from(blob).where(blob.id == id));
  const auto& result_row = result.front();
  std::cerr << "id: " << id << std::endl;
  std::cerr << "Insert size: " << data.size() << std::endl;
  std::cerr << "Select size: " << result_row.data.len << std::endl;
  if (data.size() != result_row.data.len)
  {
    std::cerr << "Size mismatch" << std::endl;

    throw std::runtime_error("Size mismatch " + std::to_string(data.size()) +
                             " != " + std::to_string(result_row.data.len));
  }
  std::cerr << "Verifying content" << std::endl;
  const auto result_blob = result_row.data.value();
  if (data != result_blob)
  {
    std::cout << "Content mismatch ([row] original -> received)" << std::endl;

    for (size_t i = 0; i < data.size(); i++)
    {
      if (data[i] != result_row.data.value()[i])
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
  sql::connection db = sql::make_test_connection();

  db.execute(R"(DROP TABLE IF EXISTS blob_sample;)");
  db.execute(R"(CREATE TABLE blob_sample (
             id bigserial PRIMARY KEY,
             data bytea
           ))");
  std::cerr << "Generating data " << blob_size << std::endl;
  std::vector<uint8_t> data(blob_size);
  std::uniform_int_distribution<unsigned short> distribution(0, 255);
  std::mt19937 engine;
  auto generator = std::bind(distribution, engine);
  std::generate_n(data.begin(), blob_size, generator);

  std::vector<uint8_t> data_smaller(blob_small_size);
  std::generate_n(data_smaller.begin(), blob_small_size, generator);

  db(insert_into(blob).set(blob.data = data_smaller));
  const auto id = db.last_insert_id("blob_sample", "id");
  verify_blob(db, data_smaller, id);

  auto prepared_insert = db.prepare(insert_into(blob).set(blob.data = parameter(blob.data)));
  prepared_insert.params.data = data;
  db(prepared_insert);
  const auto prep_id = db.last_insert_id("blob_sample", "id");
  prepared_insert.params.data.set_null();
  db(prepared_insert);
  const auto null_id = db.last_insert_id("blob_sample", "id");

  verify_blob(db, data, prep_id);
  {
    auto result = db(select(blob.data).from(blob).where(blob.id == null_id));
    const auto& result_row = result.front();
    std::cerr << "Null blob is_null:\t" << std::boolalpha << result_row.data.is_null() << std::endl;
    std::cerr << "Null blob len == 0:\t" << std::boolalpha << (result_row.data.len == 0) << std::endl;
    std::cerr << "Null blob blob == nullptr:\t" << std::boolalpha << (result_row.data.is_null()) << std::endl;
    if (!result_row.data.is_null() || result_row.data.len != 0)
    {
      throw std::runtime_error("Null blob has incorrect values");
    }
  }

  return 0;
}
