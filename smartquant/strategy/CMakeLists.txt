add_executable(
    indexArb_v2_time
    ./indexArb_v2_time.cpp
)


target_link_libraries(
    indexArb_v2_time
    algo
    event
    gateway
    libthostmduserapi_se.so
    libthosttraderapi_se.so
)