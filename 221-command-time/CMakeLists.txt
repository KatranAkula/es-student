cmake_minimum_required(VERSION 3.13)

# Инициализация Pico SDK (путь должен быть задан в PICO_SDK_PATH)
include(pico_sdk_import.cmake)

project(221_command_time)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Исходные файлы проекта 
add_executable(${PROJECT_NAME}
    main.c
    memory/memory.c
    led/led.c
    logging/log.c
    device/device.c
    clock/clock.c
    profiling/profiling.c
)

pico_set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld) #в задании нет, но проверка не проходит!!!

# Пути поиска заголовков добавляем корень проекта и все подпапки с заголовками
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/led
    ${CMAKE_CURRENT_SOURCE_DIR}/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/device
    ${CMAKE_CURRENT_SOURCE_DIR}/clock
    ${CMAKE_CURRENT_SOURCE_DIR}/profiling
)

# Библиотеки SDK
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
)

# Вывод printf в USB (а не в UART) 
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# создание файлов с именем проекта .uf2, .elf, .map, .dis 
pico_add_extra_outputs(${PROJECT_NAME})