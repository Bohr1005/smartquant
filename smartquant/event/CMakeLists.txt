aux_source_directory(./src event)

add_library(
    event  
    STATIC
    ${event}
    )