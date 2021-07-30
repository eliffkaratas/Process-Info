#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

static char *param;
static char *param1;
module_param(param,charp,0660);
module_param(param1,charp,0660);

int simple_init(void){

	printk(KERN_INFO "Right Usage:\n-all prints some information (process id and itsargument/s) about all processes\n-p takes process id and prints the details of it\n-k takes process id\n");
	return 0;
}

int all_init (void){
	struct task_struct *task;
	int ppid;
	char *all = "-all";
	if(strcmp(param,all)==0){
		printk(KERN_INFO "Process id and the argument list for the processes:\n");
		
		for_each_process(task)
	     	{
			uid_t uid = __kuid_val(task_uid(task));	
			rcu_read_lock();
			ppid = task_tgid_vnr(rcu_dereference(task->real_parent));
			rcu_read_unlock();
			printk(KERN_INFO "Ppid=%x, Uid=%x, \t Pıd=%d, \t Name=%s, \t State=%ld\n",ppid, uid, task->pid, task->comm, task->state);
		}	

	}else{
		printk(KERN_INFO "You must enter -all argument.");
	}              	
	return 0;
}

int cmdline_init(void){

	char *proc = "/proc/";
	char *cmdline = "/cmdline";
	char line[100];

	struct file *f;
	char buf[120];
	mm_segment_t fs;
	int i = 0;
	int count = 0;

	char *p = "-p";
	if(strcmp(param,p)==0){
		for(i=0; i<100; i++)
			line[i]=0;

		int length=strlen(proc); // creating //proc/pid/cmdline file path.
		for(i=0; i<length; i++){
			line[count++]=proc[i];
		}

		length=strlen(param1);
		for(i=0; i<length; i++){
			line[count++]=param1[i];
		}

		length=strlen(cmdline);
		for(i=0; i<length; i++){
			line[count++]=cmdline[i];
		}

		for(i=0;i<120;i++)
        		buf[i] = 0;
    		f = filp_open(line, O_RDONLY, 0);
    		if(f == NULL)
        		printk(KERN_ALERT "File open error.\n");
    		else{
			fs = get_fs();
			set_fs(get_ds());
			// Read the file
			f->f_op->read(f, buf, 120, &f->f_pos);
			set_fs(fs);
			printk(KERN_INFO "Process Information: \n");
			printk(KERN_INFO "Pid=%s \t Command line=%s\n",param1,buf);
    		}
    		filp_close(f,NULL);
	}
	else{
		printk(KERN_INFO "You must enter -p argument.");
	}
    	return 0;
}

void simple_exit(void) {
printk(KERN_INFO "Removing Module\n");
}

module_init(cmdline_init); // any func. that choosen
module_exit(simple_exit); 
