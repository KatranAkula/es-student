# Библиотека device: сборка из одного файла реализации
add_library(device 
    src/device.c
)

# Заголовок открыт всем, кто подключает библиотеку: #include "device/device.h"
target_include_directories(device PUBLIC
    include
)

# SDK — только для сборки самой device, наружу include-пути SDK не отдаёт
target_link_libraries(device PRIVATE
    systime
    pico_unique_id #pico/unique_id.h: pico_get_unique_board_id_string()
)

target_include_directories(device PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../231-firmware-template #firmware.h: libs\device → ..\..\  es-student
)
