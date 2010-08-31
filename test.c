#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/idr.h>

static DEFINE_SPINLOCK(test_lock);
static DEFINE_IDR(test_idr);

static int test_new_entry(char *str)
{
	int id, ret;

	do {
		if (!idr_pre_get(&test_idr, GFP_KERNEL))
			return -ENOMEM;

		spin_lock(&test_lock);
		ret = idr_get_new(&test_idr, str, &id);
		spin_unlock(&test_lock);
	} while (ret == -EAGAIN);

	if (ret)
		return -ENOMEM;

	return id;
}

static int test_remove_entry(int id)
{
	char *ptr;

	ptr = idr_find(&test_idr, id);
	if (ptr == NULL)
		return -EINVAL;

	spin_lock(&test_lock);
	idr_remove(&test_idr, id);
	spin_unlock(&test_lock);

	kfree(ptr);

	return 0;
}

static int test_replace_entry(int id, char *str)
{
	void *res;

	spin_lock(&test_lock);
	res = idr_replace(&test_idr, (void *)str, id);
	spin_unlock(&test_lock);

	return IS_ERR(res) ? PTR_ERR(res) : 0;
}

static int test_free_idr_entry(int id, void *p, void *data)
{
	kfree(p);
	return 0;
}


#define TEST_PROC_NAME "test_mod_file"

static int test_print_idr_entry(int id, void *p, void *data)
{
	struct seq_file *f = (struct seq_file *) data;
	char *str = (char *) p;

	seq_printf(f, "%05d\t%s\n", id, str);
	return 0;
}

static int test_proc_show(struct seq_file *f, void *arg)
{
	seq_printf(f, "ID   \tData\n");

	idr_for_each(&test_idr, test_print_idr_entry, (void *) f);

	return 0;
}

#define TEST_PROC_BUF_SIZE 512

static ssize_t test_proc_write(struct file *file, const char __user *buffer,
			       size_t count, loff_t *pos)
{
	int n, res, id;
	char buf[TEST_PROC_BUF_SIZE];
	char *str;

	n = (count < TEST_PROC_BUF_SIZE - 1) ?
	  count : TEST_PROC_BUF_SIZE - 1;
	if (copy_from_user(buf, buffer, n))
		return -EFAULT;
	buf[n] = '\0';

	str = kmalloc(TEST_PROC_BUF_SIZE, GFP_KERNEL);
	if (str == NULL)
		return -ENOMEM;

	res = 0;
	if (sscanf(buf, "new %s", str)) {
		res = test_new_entry(str);
	} else if (sscanf(buf, "rm %d", &id)) {
		kfree(str);
		res = test_remove_entry(id);
	} else if (sscanf(buf, "rep %d %s", &id, str)) {
		res = test_replace_entry(id, str);
	} else {
		kfree(str);
	}

	if (res < 0)
		return res;

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

static int test_init(void)
{
	proc_create(TEST_PROC_NAME, S_IRUSR|S_IWUSR, NULL, &test_proc_fops);

	/* idr_init(&test_idr); */

	return 0;
}

static void test_exit(void)
{
	remove_proc_entry(TEST_PROC_NAME, NULL);

	idr_for_each(&test_idr, test_free_idr_entry, NULL);
	idr_remove_all(&test_idr);
	/* idr_destroy(&test_idr); */
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiro Aota");
MODULE_DESCRIPTION("A Code Testing Module");
