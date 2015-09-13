/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_TRANSACTION_H
#define SQLPP_TRANSACTION_H

#include <stdexcept>

namespace sqlpp
{
  static constexpr bool quiet_auto_rollback = false;
  static constexpr bool report_auto_rollback = true;

  template <typename Db>
  class transaction_t
  {
    Db& _db;
    const bool _report_unfinished_transaction;
    bool _finished = false;

  public:
    transaction_t(Db& db, bool report_unfinished_transaction)
        : _db(db), _report_unfinished_transaction(report_unfinished_transaction)
    {
      _db.start_transaction();
    }

    transaction_t(const transaction_t&) = delete;
    transaction_t(transaction_t&&) = default;
    transaction_t& operator=(const transaction_t&) = delete;
    transaction_t& operator=(transaction_t&&) = delete;

    ~transaction_t()
    {
      if (not _finished)
      {
        try
        {
          _db.rollback_transaction(_report_unfinished_transaction);
        }
        catch (const std::exception& e)
        {
          _db.report_rollback_failure(std::string("auto rollback failed: ") + e.what());
        }
        catch (...)
        {
          _db.report_rollback_failure("auto rollback failed with unknown exception");
        }
      }
    }

    void commit()
    {
      _finished = true;
      _db.commit_transaction();
    }

    void rollback()
    {
      _finished = true;
      _db.rollback_transaction(false);
    }
  };

  template <typename Db>
  transaction_t<Db> start_transaction(Db& db, bool report_unfinished_transaction = report_auto_rollback)
  {
    return {db, report_unfinished_transaction};
  }
}

#endif
