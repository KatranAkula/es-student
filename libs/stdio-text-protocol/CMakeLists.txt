#библа stdio-text-protocol: цель с подчёркиваниями вместо дефисов
add_library(stdio_text_protocol STATIC
    src/stdio-text-protocol.c
)

#заголовок открыт для всех #include "stdio-text-protocol/stdio-text-protocol.h"
target_include_directories(stdio_text_protocol PUBLIC
    include
)

#подключаю pico_stdlib целиком, пАтАмШТА pico_stdio без него не собирается. SDK — PRIVATE
target_link_libraries(stdio_text_protocol PRIVATE
    pico_stdlib
)
