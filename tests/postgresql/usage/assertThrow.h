#pragma once

#define assert_throw(code, exception)                \
  {                                                  \
    bool exceptionThrown = false;                    \
    try                                              \
    {                                                \
      code;                                          \
    }                                                \
    catch (const exception&)                         \
    {                                                \
      exceptionThrown = true;                        \
    }                                                \
    if (not exceptionThrown)                         \
      throw std::runtime_error("missing exception"); \
  }
