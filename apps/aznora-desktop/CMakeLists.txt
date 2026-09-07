add_executable(Aznora
    main.cpp
)

target_link_libraries(Aznora
    PUBLIC
        aznora_core
)

target_compile_options(Aznora PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)