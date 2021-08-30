aux_source_directory(./ctp ctp)
aux_source_directory(./stock_level2 l2)
aux_source_directory(./rem rem)
aux_source_directory(${PROJECT_SOURCE_DIR}/smartquant/ common)

add_library(
    gateway
    STATIC
    ${ctp}
    ${l2}
    ${rem}
    ${common}
)

target_link_libraries(
    gateway
    zmq.a
    pthread
)
