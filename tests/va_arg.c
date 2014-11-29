

#include <config.h>
#include <va_arg.h>
#include <printh.h>

void va_arg_test(const char *fmt, ...)
{
        va_list arg;
        va_list tmp;
	printh("Test: va_arg_test\r\n");
        va_start(arg, fmt);

        tmp = arg;
        if (va_arg(arg, int) != 0x0a0a0a0a)
                print_str("Error with va_arg initial value\r\n");
        if (arg <= tmp)
                print_str("va_arg does not modify arg correctly\r\n");
        if (va_arg(arg, int) != (int)0xa0a0a0a0)
                print_str("Error with va_arg sequential value\r\n");

        va_end(arg);
        if (arg != 0x0)
                print_str("Error with va_end, arg still valid\r\n");
}

void test_va_arg() 
{
	printh("test_va_arg\r\n");
	va_arg_test("test format %d %d\r\n", 0x0a0a0a0a, 0xa0a0a0a0);
}
