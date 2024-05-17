
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/mutex.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

#include <linux/sched.h>
#include <asm/uaccess.h>

#define FPGA_SEGMENTS 3
#define NO_SEGMENTS_AVAILABLE -1
#define NO_SEGMENTS_RESERVED  -2

#define FPGA_FIRMWARE_PATH "/sys/class/fpga_manager/fpga0/firmware"
#define FPGA_FLAGS_PATH "/sys/class/fpga_manager/fpga0/flags"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seth Kiefer");

/*
static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int myintArray[2] = { -1, -1 };
static int arr_argc = 0;
*/

static int segments[FPGA_SEGMENTS] = {0};
static DEFINE_MUTEX(segments_mutex);

/* 
 * module_param(foo, int, 0000)
 * The first param is the parameters name
 * The second param is it's data type
 * The final argument is the permissions bits, 
 * for exposing parameters in sysfs (if non-zero) at a later stage.
 */

/* 
module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");

module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");

module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");

module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");
*/
/*
 * module_param_array(name, type, num, perm);
 * The first param is the parameter's (in this case the array's) name
 * The second param is the data type of the elements of the array
 * The third argument is a pointer to the variable that will store the number 
 * of elements of the array initialized by the user at module loading time
 * The fourth argument is the permission bits
 */
/*
module_param_array(myintArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintArray, "An array of integers");
*/

int request_fpga_segment_handler(void)
{
	int seg;
	bool done = false;

	//Find and reserve first available FPGA segment
	mutex_lock(&segments_mutex);

	for(seg = 0; seg < FPGA_SEGMENTS && !done; seg++){
		if(segments[seg] == 0){

			//reserve FPGA for current process
			segments[seg] = current->pid;
			done = true;
		}
	}

	mutex_unlock(&segments_mutex);

	if(!done) seg = NO_SEGMENTS_AVAILABLE;

	if(seg >= 0){
		/*do any additional setup*/
	}

	return seg;
}

int program_fpga_segment_handler(char* bitstream_file_name)
{
	//echo 0 > 
	return 0;
}

int release_fpga_segment_handler(void)
{
	int seg;
	bool done = false;

	//Find and reserved FPGA segment
	mutex_lock(&segments_mutex);
	
	for(seg = 0; seg < FPGA_SEGMENTS && !done; seg++){
		if(segments[seg] == current->pid){

			done = true;
			
			/*clear FPGA?*/

			//mark FPGA segment as available
			segments[seg] = 0;
		}
	}

	

	mutex_unlock(&segments_mutex);

	if(!done) seg = NO_SEGMENTS_RESERVED;
	else seg = 1;

	if(seg >= 0){
		/*do any additional setup*/
	}

	return seg;
}

SYSCALL_DEFINE0(request_fpga_segment)
{
	return request_fpga_segment_handler();
}

SYSCALL_DEFINE1(program_fpga_segment, char *, filename)
{
	return program_fpga_segment_handler(filename);
}

SYSCALL_DEFINE0(program_fpga_segment)
{
	return program_fpga_segment_handler();
}

/*
static int __init request_fpga_segment_handler_init(void)
{
	
	mutex_init(&segments_mutex);
	return 0;
}

static void __exit request_fpga_segment_handler_exit(void)
{
	printk(KERN_INFO "Goodbye, world 5\n");
}

module_init(request_fpga_segment_handler_init);
module_exit(request_fpga_segment_handler_exit);

*/