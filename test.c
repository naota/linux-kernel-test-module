#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h> 
#include <linux/uaccess.h>

#define TEST_STRING_BUF_SIZE 256

static char test_string[TEST_STRING_BUF_SIZE] = "";

static int test_proc_show(struct seq_file *f, void *arg)
{
	seq_printf(f, "%s", test_string);
	return 0;
}

#define TEST_PROC_NAME "test_mod_file"

static ssize_t test_proc_write(struct file *file, const char __user *buffer,
			       size_t count, loff_t *pos)
{
	int n;

	n = (count < TEST_STRING_BUF_SIZE - 1) ?
	  count : TEST_STRING_BUF_SIZE - 1;
	if(copy_from_user(test_string, buffer, n))
		return -EFAULT;
	test_string[n] = '\0';

	return count;
}

static int test_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, test_proc_show, NULL);
}

static const struct file_operations test_proc_fops = {
	.open = test_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = test_proc_write,
};

static int test_init (void)
{
	proc_create(TEST_PROC_NAME, S_IRUSR|S_IWUSR, NULL, &test_proc_fops);
	return 0;
}

static void test_exit(void)
{
	remove_proc_entry(TEST_PROC_NAME, NULL);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiro Aota");
MODULE_DESCRIPTION("A Code Testing Module");
