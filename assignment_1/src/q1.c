#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

/**
 * A function to provide better information in case of failure.
 */
pid_t sane_fork()
{
	pid_t cpid = fork();

	if (cpid < 0) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	return cpid;
}

/**
 * Prints pid of the process with its "role".
 */
int print_pid(const char *role, int print_parent)
{
	int res;
	res = printf("PID of %-12s: %ld", role, (long) getpid());
	if (print_parent)
		res = printf(" (%ld)", (long) getppid());
	res = printf("\n");
	return res;
}

/**
 * Execute child process
 */
void execute_child()
{
	print_pid("child", TRUE);
	pid_t cpid = sane_fork();

	usleep(100);
	if (!cpid)
		print_pid("grand child", TRUE);
	else {
		wait(NULL);
		cpid = sane_fork();

		if (!cpid)
			print_pid("grand child", TRUE);
		else
			wait(NULL);
	}
}

/**
 * Driver Program.
 */
int main()
{
	pid_t cpid;
	cpid = sane_fork();

	if (!cpid) {
		execute_child();
	} else {
		cpid = sane_fork();

		if (!cpid) {
			execute_child();
		} else {
			wait(NULL);
			wait(NULL);
			print_pid("parent", FALSE);
		}
	}

	return 0;
}
