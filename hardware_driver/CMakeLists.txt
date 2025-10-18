# 创建硬件相关的静态库
add_library(hardware_driver STATIC)
# 添加该模块的源文件，使用相对路径，无需 "./"
target_sources(hardware_driver PRIVATE
        src/iic_hal_software.c
        src/AT24C02_driver.c
        src/eeprom_humi_handler.c
)
# 设置该模块的公共头文件路径，这样主程序链接此库后会自动包含此路径
target_include_directories(hardware_driver PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(hardware_driver
        PUBLIC stm32cubemx
        PUBLIC STM32_Drivers
        PUBLIC user_debug
)
