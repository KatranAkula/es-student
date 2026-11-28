cmake_minimum_required(VERSION 3.13)

# Инициализация Pico SDK (путь должен быть задан в PICO_SDK_PATH)
include(pico_sdk_import.cmake)

project(231_firmware_template)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

#Ключ предупреждений — до add_subdirectory, чтобы получили и библиотеки
add_compile_options(-Wall)

#libs лежит вне папки проекта: второй аргумент задаёт каталог сборки
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

#железо прибора: единственное место проекта с SDK
add_library(platform STATIC
    platform/platform.c
)
#путь к заголовкам — папка проекта; platform подключает pico_stdlib как PRIVATE, через этот путь logic и main.c находят и firmware.h, и platform/platform.h
target_include_directories(platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(platform PRIVATE pico_stdlib)

#логика прибора: задачи и команды, без SDK
add_library(logic STATIC
    logic/api/api.c
    logic/api/api-callbacks.c
    logic/led-task/led-task.c
    logic/button-task/button-task.c
    logic/pi-task/pi-task.c
)
# путь к заголовкам — папка logic
# stdio_text_protocol — PUBLIC, остальные библиотеки — PRIVATE
target_include_directories(logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logic)
target_link_libraries(logic
    PUBLIC stdio_text_protocol
    PRIVATE platform systime profiling device logging
)

# main.c только запускает модули и держит суперцикл
add_executable(${PROJECT_NAME}
    main.c
)
# SDK main.c не подключает — П7; свои библиотеки для журнала и секундомеров — отдельно
target_link_libraries(${PROJECT_NAME}
    logic
    profiling
    logging
    device
    systime
    pico_stdlib  
)

# Выключатель профилирования: 0 — вызовов секундомеров в образе нет
target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=1

)

# Вывод printf в USB (а не в UART)
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)


# Создание файлов с именем проекта .uf2, .elf, .map, .dis
pico_add_extra_outputs(${PROJECT_NAME})
