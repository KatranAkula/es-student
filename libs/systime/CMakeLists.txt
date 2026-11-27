# Библиотека systime: сборка из одного файла реализации
add_library(systime 
    src/systime.c
)

# Заголовок открыт всем: #include "systime/systime.h"
target_include_directories(systime PUBLIC
    include
)

# SDK подключаем только для себя (PRIVATE)
target_link_libraries(systime 
    pico_stdlib
)
