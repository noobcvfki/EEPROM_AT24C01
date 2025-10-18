#include "debug.h"

int RETARGET_WriteChar(char c);

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
	
#else

#define PUTCHAR_PROTOTYPE int fputc(int ch,FILE *f)

#endif //

PUTCHAR_PROTOTYPE
{
	RETARGET_WriteChar(ch);
	
	return ch;
}

// 将单个字符通过 SEGGER RTT 发送到 Channel 0
int RETARGET_WriteChar(char c) {
    SEGGER_RTT_printf(0, &c);  // 0 表示 RTT 的 Channel 0
    return 1;  // 表示成功写入一个字符
}

void debug_init(void)
{
	//**********初始化RTT和elog**********//
	SEGGER_RTT_Init();
	elog_init();

	//**********设置输出格式**********//
	//启用输出字体的颜色
	elog_set_text_color_enabled(true);
	
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	


	//启动
	elog_start();
}

void test_elog(void){
	log_a("TAG","this is assert");
	log_e("TAG","this is error");
	log_w("TAG","this is warning");
	log_i("TAG","this is info");
	log_d("TAG","this is debug");
	log_v("TAG","this is verbose");
}



