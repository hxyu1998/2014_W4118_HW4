#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
int main(int argc, char **argv)
{
	int result;
/*	
	struct sched_param param;
	param.sched_priority = 0;
	result = syscall(156, 1, 6, &param);
*/
	result = syscall(378, 1, 1);
	if(result < 0)
		perror("error");
	printf("total tasks: %d\n", result);
	return result;
}
