#include <linux/module.h>

static int test_init (void)
{
	return 0;
}

static void test_exit(void)
{
	
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiro Aota");
MODULE_DESCRIPTION("A Code Testing Module");
