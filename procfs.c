#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/parser.h>

#define LEN 1024
#define LSM_NS_SELINUX          0x00000001
#define LSM_NS_APPARMOR         0x00000002
#define LSM_NS_TOMOYO           0x00000004

const enum {
        Lsmns_selinux,
        Lsmns_apparmor,
        Lsmns_tomoyo,
        Lsmns_err
};

const match_table_t tokens = {
        {Lsmns_selinux, "selinux"},
        {Lsmns_apparmor, "apparmor"},
        {Lsmns_tomoyo, "tomoyo"},
        {Lsmns_err, NULL}
};

struct proc_dir_entry *proc_lsm;
char lsm_buff[LEN];

static int parse_lsmns_procfs(void)
{
        int types = 0;
        substring_t args[MAX_OPT_ARGS];
        char* p;
        int token;
	int len = strlen(lsm_buff);
	if(len == 0)
		return types;
	char* tmp = kmalloc(len, GFP_ATOMIC);
	strlcpy(tmp, lsm_buff, len);
	char* str = &tmp[0];

	while((p = strsep(&str, ",")) != NULL){
                printk(KERN_INFO "%s", p);
                if (!*p)
                        continue;
                token = match_token(p, tokens, args);
                printk(KERN_INFO "%d", token);
		switch (token) {
                case Lsmns_selinux:
                        types |= LSM_NS_SELINUX;
                        break;
                case Lsmns_apparmor:
                        types |= LSM_NS_APPARMOR;
                        break;
                case Lsmns_tomoyo:
                        types |= LSM_NS_TOMOYO;
                        break;
		}
        }

	while(types){
		if(types & 1)
			printk(KERN_INFO "1");
		else
			printk(KERN_INFO "0");
		types >>= 1;
	}
	printk(KERN_INFO "\n");
        return types;
}

static ssize_t lsmns_read(struct file* fp, char __user *user_buff,
               size_t count, loff_t *position)
{
        parse_lsmns_procfs();
        printk(KERN_INFO "read_called\n");
        return simple_read_from_buffer(user_buff, count, position, lsm_buff, LEN);
}

static ssize_t lsmns_write(struct file* fp, const char __user *user_buff,
                size_t count, loff_t *position)
{
        printk(KERN_INFO "write called\n");
        if(count > LEN)
                return -EINVAL;
	memset(lsm_buff, 0, LEN);
        return simple_write_to_buffer(lsm_buff, LEN, position, user_buff, count);
}

struct file_operations proc_fops = {
        .read = lsmns_read,
        .write = lsmns_write,
};

int proc_init(void)
{
        proc_lsm = proc_create_data("lsmns", 0666, NULL, &proc_fops, "test");
        printk(KERN_INFO "proc_init success\n");
        return 0;
}

void proc_cleanup(void)
{
        remove_proc_entry("lsmns", NULL);
}

module_init(proc_init);
module_exit(proc_cleanup);
