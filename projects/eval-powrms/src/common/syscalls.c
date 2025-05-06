
#include <_ansi.h>
#include <sys/reent.h>

// Existing functions (you probably already have)


__attribute__((used)) int _getpid_r(struct _reent *r)
{
	(void)r;
	return 1;
}
__attribute__((used)) int _getpid(void)
{
	return 1;
}

__attribute__((used)) int _kill_r(struct _reent *r, int pid, int sig)
{
	(void)r;
	(void)pid;
	(void)sig;
	return -1;   // Indicate error (no process support)
}

__attribute__((used)) int _kill(int pid, int sig)
{
	(void)pid;
	(void)sig;
	return -1;   // Indicate error (no process support)
}