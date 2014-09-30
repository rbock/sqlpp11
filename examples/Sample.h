#ifndef TEST_SAMPLE_H
#define TEST_SAMPLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>

namespace test
{
  namespace TabPerson_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
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
      struct _name_t
      {
        static constexpr const char* _get_name() { return "name"; }
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Feature
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "feature"; }
        template<typename T>
        struct _member_t
          {
            T feature;
            T& operator()() { return feature; }
            const T& operator()() const { return feature; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct TabPerson: sqlpp::table_t<TabPerson,
               TabPerson_::Id,
               TabPerson_::Name,
               TabPerson_::Feature>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "tab_person"; }
      template<typename T>
      struct _member_t
      {
        T tabPerson;
        T& operator()() { return tabPerson; }
        const T& operator()() const { return tabPerson; }
      };
    };
  };
  namespace TabFeature_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
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
      struct _name_t
      {
        static constexpr const char* _get_name() { return "name"; }
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct Fatal
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "fatal"; }
        template<typename T>
        struct _member_t
          {
            T fatal;
            T& operator()() { return fatal; }
            const T& operator()() const { return fatal; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::require_insert>;
    };
  }

  struct TabFeature: sqlpp::table_t<TabFeature,
               TabFeature_::Id,
               TabFeature_::Name,
               TabFeature_::Fatal>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "tab_feature"; }
      template<typename T>
      struct _member_t
      {
        T tabFeature;
        T& operator()() { return tabFeature; }
        const T& operator()() const { return tabFeature; }
      };
    };
  };
}
#endif
