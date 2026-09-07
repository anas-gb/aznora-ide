add_executable(Aznora
    main.cpp
)

target_link_libraries(Aznora
    PRIVATE
        aznora_core
)

target_compile_options(Aznora PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)