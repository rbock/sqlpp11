#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
Copyright (c) 2023, Vesselin Atanasov
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace sqlpp
{
  namespace detail
  {
    // This class is modeled after boost::circular_buffer
    template <typename T>
    class circular_buffer
    {
    public:
      circular_buffer(std::size_t capacity);
      std::size_t capacity() const;
      void set_capacity(std::size_t capacity);
      std::size_t size() const;
      bool empty() const;
      bool full() const;
      T& front();
      void pop_front();
      void push_back(T&& t);

    private:
      std::vector<T> _data;
      std::size_t _capacity;
      std::size_t _size;
      std::size_t _back;
      std::size_t _front;

      void increment(std::size_t& pos);
    };

    template <typename T>
    circular_buffer<T>::circular_buffer(std::size_t capacity)
        : _data(capacity), _capacity{capacity}, _size{0}, _back{0}, _front{0}
    {
    }

    template <typename T>
    std::size_t circular_buffer<T>::capacity() const
    {
      return _capacity;
    }

    template <typename T>
    void circular_buffer<T>::set_capacity(std::size_t new_capacity)
    {
      if (new_capacity == _capacity)
      {
        return;
      }
      // Reduce the number of cases by rotating `front()` to `_data.begin()`.
      std::rotate(_data.begin(), _data.begin() + static_cast<ptrdiff_t>(_front), _data.end());

      _front = 0;
      _back = (new_capacity > _size) ? _size : 0;
      _data.resize(new_capacity);
      _capacity = new_capacity;
      _size = std::min(_size, new_capacity);
    }

    template <typename T>
    std::size_t circular_buffer<T>::size() const
    {
      return _size;
    }

    template <typename T>
    bool circular_buffer<T>::empty() const
    {
      return _size == 0;
    }

    template <typename T>
    bool circular_buffer<T>::full() const
    {
      return _size == _capacity;
    }

    template <typename T>
    T& circular_buffer<T>::front()
    {
      if (empty())
      {
        throw std::runtime_error{"circular_buffer::front() called on empty buffer"};
      }
      return _data[_front];
    }

    template <typename T>
    void circular_buffer<T>::pop_front()
    {
      if (empty())
      {
        throw std::runtime_error{"circular_buffer::pop_front() called on empty buffer"};
      }
      _data[_front] = {};
      increment(_front);
      --_size;
    }

    template <typename T>
    void circular_buffer<T>::push_back(T&& t)
    {
      if (full())
      {
        throw std::runtime_error{"circular_buffer::push_back() called on full buffer"};
      }
      _data[_back] = std::move(t);
      increment(_back);
      ++_size;
    }

    template <typename T>
    void circular_buffer<T>::increment(std::size_t& pos)
    {
      pos = (pos + 1) % _capacity;
    }
  }  // namespace detail
}  // namespace sqlpp
