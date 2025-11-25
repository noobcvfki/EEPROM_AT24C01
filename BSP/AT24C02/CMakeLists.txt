
add_library(bsp_at24c02 STATIC)

target_sources(bsp_at24c02 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/at24c02_driver.c
)

target_include_directories(bsp_at24c02 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(bsp_at24c02 PRIVATE
        elog
        softlib
)
