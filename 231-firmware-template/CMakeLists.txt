cmake_minimum_required(VERSION 3.13)

# Инициализация Pico SDK (путь должен быть задан в PICO_SDK_PATH)
include(pico_sdk_import.cmake)



project(231_firmware_template)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Ключ предупреждений — до add_subdirectory, чтобы получили и библиотеки
add_compile_options(-Wall)

# libs лежит вне папки проекта: второй аргумент задаёт каталог сборки
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

# Исходные файлы проекта — только то, что осталось в корне проекта
add_executable(${PROJECT_NAME}
    main.c
    api/api.c
    led-task/led-task.c
    button-task/button-task.c

    pi-task/pi-task.c
)



# Пути поиска заголовков: корень проекта и подпапки проекта с заголовками
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task
    ${CMAKE_CURRENT_SOURCE_DIR}/button-task
    ${CMAKE_CURRENT_SOURCE_DIR}/pi-task


)





# Библиотеки SDK и свои — по именам, папки и исходники библиотек тут больше не перечисляются
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    logging
    device
    profiling 
    systime
    stdio_text_protocol
    # //

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


