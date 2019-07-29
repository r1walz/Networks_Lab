#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/** Maximum buffer size */
#define BUFSIZE 100

/**
 * Function to give better information about the crash and exit with
 * a predefined value.
 */
void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

/**
 * Define a "sane" fork which gives more information in case of failure.
 */
pid_t sane_fork()
{
	pid_t cpid = fork();

	if (cpid < 0)
		die("fork failed");
	return cpid;
}

/**
 * Fetch name of process using it's PID.
 * It reads name from /proc/PID/status file.
 */
char *get_process_name(pid_t pid)
{
	FILE *file;
	char path[BUFSIZE];
	char proc_name[BUFSIZE];

	snprintf(path, BUFSIZE, "/proc/%d/status", pid);

	file = fopen(path, "r");
	fscanf(file, "%*s %s", proc_name);
	fclose(file);

	return strdup(proc_name);
}

/**
 * Main driver program.
 */
int main()
{
	pid_t cpid = sane_fork();

	if (!cpid) {
		char *pname;
		usleep(1000);

		pname = get_process_name(getppid());
		printf("I'm an orphan :(  (%d) My foster parent is %s (%d)\n",
			getpid(), pname, getppid());
		free(pname);

		cpid = sane_fork();
		if (!cpid)
			printf("I'm a zombie! @_@ (%d)\n", getpid());
		else
			sleep(1);
	} else
		printf("I'm the parent!   (%d)\n", getpid());

	return 0;
}
