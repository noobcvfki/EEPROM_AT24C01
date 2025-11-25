add_library(BSP_MPUxxx STATIC)

target_sources(BSP_MPUxxx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MPUxxx_driver.c
)

target_include_directories(BSP_MPUxxx PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(BSP_MPUxxx PRIVATE
    elog
    softlib
)
