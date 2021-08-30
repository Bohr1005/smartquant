add_executable(
     example
    ./example.cpp
)


target_link_libraries(
    example
    algo
    event
    gateway
    libthostmduserapi_se.so
    libthosttraderapi_se.so
)
