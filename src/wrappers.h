#ifndef __WRAPPERS_H__
#define __WRAPPERS_H__

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

typedef void handler_t(int);

void unix_error(char *msg);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);
void Sigdelset(sigset_t *set, int signum);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
void Write(int fd, const void *buf, size_t count);
char *Fgets(char *ptr, int n, FILE *stream);
handler_t *Signal(int signum, handler_t *handler);
void Setpgid(pid_t pid, pid_t pgid);
void Kill(pid_t pid, int signum);
pid_t Fork(void);
pid_t Waitpid(pid_t pid, int *iptr, int options);

#endif
