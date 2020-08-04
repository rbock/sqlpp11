add_executable(Test)
target_link_libraries(Test PRIVATE sqlpp11::sqlpp11)

target_sources(Test 
    PRIVATE 
        main.cpp 
)