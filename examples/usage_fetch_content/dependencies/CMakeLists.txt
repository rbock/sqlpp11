include(FetchContent)

FetchContent_Declare(sqlpp11
  GIT_REPOSITORY https://github.com/Leon0402/sqlpp11
  GIT_TAG        origin/feature/fetchContent
)

add_subdirectory(sqlpp11)