#pragma once

/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include <string>
#include <iostream>

namespace sqlpp
{
  namespace sqlite3
  {
    struct connection_config
    {
      connection_config() : path_to_database{}, flags{0}, vfs{}, debug{false},password{}
      {
      }
      connection_config(const connection_config&) = default;
      connection_config(connection_config&&) = default;
      connection_config& operator=(const connection_config&) = default;
      connection_config& operator=(connection_config&&) = default;

      connection_config(std::string path, int fl = 0, std::string vf = "", bool dbg = false,std::string password="")
          : path_to_database{std::move(path)}, flags{fl}, vfs{std::move(vf)}, debug{dbg},password{password}
      {
      }

      bool operator==(const connection_config& other) const
      {
        return (other.path_to_database == path_to_database && other.flags == flags && other.vfs == vfs &&
                other.debug == debug && other.password==password);
      }

      bool operator!=(const connection_config& other) const
      {
        return !operator==(other);
      }


      std::string path_to_database;
      int flags;
      std::string vfs;
      bool debug;
      std::string password;
    };
  }
}
