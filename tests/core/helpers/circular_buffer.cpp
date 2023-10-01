/*
 * Copyright (c) 2023, Roland Bock
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

#include <sqlpp11/detail/circular_buffer.h>

#include <iostream>

namespace sqlpp
{
  namespace
  {
    template <typename Result, typename Expected>
    void assert_equal(int lineNo, const Result& result, const Expected& expected)
    {
      if (result != expected)
      {
        std::cerr << __FILE__ << " " << lineNo << '\n'
                  << "Expected: -->|" << expected << "|<--\n"
                  << "Received: -->|" << result << "|<--\n";
        throw std::runtime_error("unexpected result");
      }
    }

    template <typename Result, typename Expected>
    void assert_not_equal(int lineNo, const Result& result, const Expected& expected)
    {
      if (result == expected)
      {
        std::cerr << __FILE__ << " " << lineNo << '\n'
                  << "Expected: -->|" << expected << "|<--\n"
                  << "Received: -->|" << result << "|<--\n";
        throw std::runtime_error("unexpected equality");
      }
    }

    template <typename Result>
    inline void assert_true(int lineNo, Result result)
    {
      assert_equal(lineNo, result, true);
    }

    template <typename Result>
    inline void assert_false(int lineNo, Result result)
    {
      assert_equal(lineNo, result, false);
    }

    template <typename Callable>
    void assert_runtime_error(int lineNo, Callable callable)
    {
      try
      {
        callable();
      }
      catch (const std::runtime_error& ex)
      {
        return;
      }
      catch (...)
      {
        std::cerr << __FILE__ << " " << lineNo << '\n' << "Unexpected exception caught\n";
        throw std::runtime_error("unexpected exception");
      }
      std::cerr << __FILE__ << " " << lineNo << '\n' << "Expected exception not thrown\n";
      throw std::runtime_error("Missing exception");
    }

    template <typename Callable>
    void assert_no_except(int lineNo, Callable callable)
    {
      try
      {
        callable();
      }
      catch (...)
      {
        std::cerr << __FILE__ << " " << lineNo << '\n' << "Unexpected exception caught\n";
        throw std::runtime_error("unexpected exception");
      }
    }

    using ::sqlpp::detail::circular_buffer;

    void test_no_capacity()
    {
      auto cb = circular_buffer<int>(0);
      assert_true(__LINE__, cb.empty());
      assert_true(__LINE__, cb.full());
      assert_runtime_error(__LINE__, [&cb] { cb.front(); });
      assert_runtime_error(__LINE__, [&cb] { cb.push_back(42); });
      assert_runtime_error(__LINE__, [&cb] { cb.pop_front(); });
      cb.set_capacity(1);
    }

    void test_empty()
    {
      auto cb = circular_buffer<int>(10);
      assert_true(__LINE__, cb.empty());
      assert_false(__LINE__, cb.full());
      assert_runtime_error(__LINE__, [&cb] { cb.front(); });
      assert_runtime_error(__LINE__, [&cb] { cb.pop_front(); });
      assert_no_except(__LINE__, [&cb] { cb.push_back(42); });
      cb.set_capacity(1);
    }

    void test_push_back()
    {
      constexpr int capacity = 10;
      auto cb = circular_buffer<int>(capacity);

      // We can push as many times as we have capacity.
      for (int i = 0; i < capacity; ++i)
      {
        cb.push_back(42);
        assert_equal(__LINE__, cb.size(), std::size_t(i + 1));
      }
      assert_true(__LINE__, cb.full());

      // Cannot push more than the capacity.
      assert_runtime_error(__LINE__, [&cb] { cb.push_back(42); });
    }

    void test_pop_front()
    {
      constexpr int capacity = 10;
      auto cb = circular_buffer<int>(capacity);

      for (int i = 0; i < capacity; ++i)
      {
        cb.push_back(42);
      }

      // We can pop as many times as we pushed before.
      for (int i = 0; i < capacity; ++i)
      {
        cb.pop_front();
        assert_equal(__LINE__, cb.size(), std::size_t(capacity - i - 1));
      }
      assert_true(__LINE__, cb.empty());

      // Cannot pop from empty buffer.
      assert_runtime_error(__LINE__, [&cb] { cb.pop_front(); });
    }

    void test_front()
    {
      constexpr int capacity = 10;
      auto cb = circular_buffer<int>(capacity);

      // Pushing back does not change the front.
      for (int i = 0; i < capacity; ++i)
      {
        cb.push_back(std::move(i));
        assert_equal(__LINE__, cb.front(), 0);
      }

      // Popping from the front moves `front` to the next entry.
      for (int i = 0; i < capacity; ++i)
      {
        assert_equal(__LINE__, cb.front(), i);
        cb.pop_front();
      }

      // Cannot pop from empty buffer.
      assert_runtime_error(__LINE__, [&cb] { cb.pop_front(); });
    }

    void test_increase_capacity()
    {
      constexpr int old_capacity = 11;
      constexpr int old_size = 7;
      constexpr int new_capacity = 17;
      for (int iterations = 0; iterations < old_capacity; ++iterations)
      {
        auto cb = circular_buffer<int>(old_capacity);

        // Pre-fill with `size` items.
        int current_back = 0;
        int current_front = 0;
        for (int i = 0; i < old_size; ++i)
        {
          cb.push_back(std::move(current_back));
          ++current_back;
        }

        // Move through the buffer.
        for (int i = 0; i < iterations; ++i)
        {
          cb.push_back(std::move(current_back));
          assert_equal(__LINE__, cb.front(), current_front);
          cb.pop_front();
          ++current_back;
          ++current_front;
        }

        assert_equal(__LINE__, cb.size(), std::size_t(old_size));

        // Increasing the capacity above the current size should have no effect on the current items.
        cb.set_capacity(new_capacity);

        // Popping from the front moves `front` to the next entry. Since not items were touched, we can value continue to be in ithe same order they were pushed.
        for (int i = 0; i < new_capacity; ++i)
        {
          assert_equal(__LINE__, cb.front(), current_front);
          cb.pop_front();
          ++current_front;
          cb.push_back(std::move(current_back));
          ++current_back;
        }
      }
    }

    void test_reduce_capacity()
    {
      constexpr int old_capacity = 17;
      constexpr int old_size = 11;
      constexpr int new_capacity = 7;
      for (int iterations = 0; iterations < old_capacity; ++iterations)
      {
        auto cb = circular_buffer<int>(old_capacity);

        // Pre-fill with `size` items.
        int current_back = 0;
        int current_front = 0;
        for (int i = 0; i < old_size; ++i)
        {
          cb.push_back(std::move(current_back));
          ++current_back;
        }

        // Move through the buffer.
        for (int i = 0; i < iterations; ++i)
        {
          cb.push_back(std::move(current_back));
          assert_equal(__LINE__, cb.front(), current_front);
          cb.pop_front();
          ++current_back;
          ++current_front;
        }

        assert_equal(__LINE__, cb.size(), std::size_t(old_size));

        // Reducing the capacity below the current size implictly drops the last push_back items. The remaining items at
        // the front() should remain unchanged, though.
        cb.set_capacity(new_capacity);

        // Popping from the front moves `front` to the next entry.
        for (int i = 0; i < new_capacity; ++i)
        {
          assert_equal(__LINE__, cb.front(), current_front);
          cb.pop_front();
          ++current_front;
          cb.push_back(std::move(current_back));
          ++current_back;
        }

        // We can observe that a few push_back items were dropped.
        assert_not_equal(__LINE__, cb.front(), current_front);

      }
    }

    void test_reduce_capacity_to_size()
    {
      constexpr int old_capacity = 17;
      constexpr int old_size = 11;
      constexpr int new_capacity = old_size;
      for (int iterations = 0; iterations < old_capacity; ++iterations)
      {
        auto cb = circular_buffer<int>(old_capacity);

        // Pre-fill with `size` items.
        int current_back = 0;
        int current_front = 0;
        for (int i = 0; i < old_size; ++i)
        {
          cb.push_back(std::move(current_back));
          ++current_back;
        }

        // Move through the buffer.
        for (int i = 0; i < iterations; ++i)
        {
          cb.push_back(std::move(current_back));
          assert_equal(__LINE__, cb.front(), current_front);
          cb.pop_front();
          ++current_back;
          ++current_front;
        }

        assert_equal(__LINE__, cb.size(), std::size_t(old_size));

        // Reducing the capacity to the current size drops no items.
        cb.set_capacity(new_capacity);

        // Popping from the front moves `front` to the next entry.
        for (int i = 0; i < old_capacity; ++i)
        {
          assert_equal(__LINE__, cb.front(), current_front);
          cb.pop_front();
          ++current_front;
          cb.push_back(std::move(current_back));
          ++current_back;
        }
      }
    }

  }  // namespace
}  // namespace sqlpp11

int main(int, char*[])
{
  sqlpp::test_no_capacity();
  sqlpp::test_empty();
  sqlpp::test_push_back();
  sqlpp::test_pop_front();
  sqlpp::test_front();
  sqlpp::test_increase_capacity();
  sqlpp::test_reduce_capacity();
  sqlpp::test_reduce_capacity_to_size();

  return 0;
}
