

#include <config.h>
#include <va_arg.h>
#include <printh.h>

#ifdef TESTING
void test_hypervisor()
{
	printh("\r\n\r\nAuto-testing hypervisor\r\n"); 
	test_va_arg();
}
#else
void test_hypervisor() { }
#endif
