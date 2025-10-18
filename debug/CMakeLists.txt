# 创建调试工具库
add_library(user_debug STATIC)

#elog源文件的地址：
set(elog_src
        ${CMAKE_CURRENT_SOURCE_DIR}/elog/src)
#RTT源文件的地址：
set(RTT_src
        ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER_RTT/src)
# 添加调试源文件
target_sources(user_debug PRIVATE
        debug.c
        ${elog_src}/elog.c
        ${elog_src}/elog_buf.c
        ${elog_src}/elog_port.c
        ${elog_src}/elog_async.c
        ${elog_src}/elog_utils.c
        ${RTT_src}/SEGGER_RTT_printf.c
        ${RTT_src}/SEGGER_RTT.c
)
# 设置头文件地址
target_include_directories(user_debug PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/elog/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER_RTT/inc
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(user_debug PRIVATE
        stm32cubemx
        STM32_Drivers
)
