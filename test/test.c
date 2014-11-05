#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
int main(int argc, char **argv)
{
	int result;
	struct sched_param param;
	param.sched_priority = 0;
	result = syscall(156, 1, 6, &param);	
	if(result < 0)
		perror("error");
	return 0;
}
