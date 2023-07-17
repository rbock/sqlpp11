#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace model
{
  namespace TabDepartment_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct Name
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct Division
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "division";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T division;
            T& operator()() { return division; }
            const T& operator()() const { return division; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
  } // namespace TabDepartment_

  struct TabDepartment: sqlpp::table_t<TabDepartment,
               TabDepartment_::Id,
               TabDepartment_::Name,
               TabDepartment_::Division>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tab_department";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tabDepartment;
        T& operator()() { return tabDepartment; }
        const T& operator()() const { return tabDepartment; }
      };
    };
  };
} // namespace model
