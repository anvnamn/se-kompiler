add_library(mycompiler_lib
    lexer.cpp
    parser.cpp
    semantic_analyzer.cpp
    codegen.cpp
)

target_link_libraries(mycompiler_lib
    fmt::fmt
)

target_include_directories(mycompiler_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(mycompiler_lib
    SYSTEM PUBLIC
        ${fmt_SOURCE_DIR}/include
        ${cxxopts_SOURCE_DIR}/include
)

add_executable(sek
    main.cpp
)

target_link_libraries(sek
    PRIVATE mycompiler_lib
    fmt::fmt
    cxxopts::cxxopts
)
