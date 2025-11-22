cmake_minimum_required(VERSION 3.10)


add_library(softlib STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/iic_hal.c  # debug 模块自身的逻辑（若有）
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DWT_delay.c
)

# ------------------------------
# 3. 链接子模块并暴露接口
# ------------------------------
target_link_libraries(softlib PUBLIC
        STM32_Drivers
)

# （可选）暴露 debug 模块自身的头文件（若有）
target_include_directories(softlib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)