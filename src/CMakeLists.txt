add_library(mycompiler_lib
    lexer.cpp
    parser.cpp
)

target_link_libraries(mycompiler_lib
    fmt::fmt
)

target_include_directories(mycompiler_lib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(sek
    main.cpp
)

target_link_libraries(sek
    PRIVATE mycompiler_lib
    fmt::fmt
)
