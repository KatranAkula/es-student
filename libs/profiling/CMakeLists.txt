# Библиотека profiling: сборка из одного файла реализации
add_library(profiling 
    src/profiling.c
)

# Заголовок открыт всем: #include "profiling/profiling.h"
target_include_directories(profiling PUBLIC
    include
)

# SDK 
target_link_libraries(profiling 
    systime
)

