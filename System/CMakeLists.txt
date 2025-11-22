
add_library(unit STATIC)

target_sources(unit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/unit.c
)

target_include_directories(unit PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(unit PRIVATE
        elog
        STM32_Drivers
        softlib
        bsp_at24c02
)
