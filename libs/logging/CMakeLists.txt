# Библиотека logging: сборка из одного файла реализации
add_library(logging STATIC
    src/logging.c
)

# Заголовок открыт всем: #include "logging/logging.h"
target_include_directories(logging PUBLIC
    include
)

# SDK не подключаем совсем: logging использует только printf из stdio
