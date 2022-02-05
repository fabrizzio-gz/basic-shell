#include "wrappers.h"

#ifndef MAXJOBS
#define MAXJOBS  16
#endif
#define MAXCMD   16

void reap_all_children();
static pid_t parse_pid(char *arg);
static void print_terminated_job(pid_t pid);

extern pid_t jobs[];
extern pid_t fg_job;

/* local globals */
static char job_status[MAXJOBS] = {0}; /* 0: Running, 1: Stopped */
static char *status[2] = {"Running", "Stopped"};
/* Save only MAXCMD chars of job command */
static char job_cmd[MAXJOBS][MAXCMD];

void save_job(pid_t pid) {
  for (int i=0; i < MAXJOBS; i++)
    if (jobs[i] == 0) {
      jobs[i] = pid;
      return;
    }

  fprintf(stderr, "Reached job limit: %d. Can't save job\n", MAXJOBS);
  kill(pid, SIGINT);
  Waitpid(pid, NULL, 0);
  reap_all_children();
  exit(1);
}

int get_jid(pid_t pid) {
  for (int i=0; i < MAXJOBS; i++)
    if (jobs[i] == pid)
      // JID is jobs array index + 1
      return i+1;

  reap_all_children();
  char s[64];
  sprintf(s, "get_jid: Unkown PID %d", pid);
  unix_error(s);
  return -1;
}

void release_job(pid_t pid) {
  jobs[get_jid(pid) - 1] = 0;
}

void print_jobs() {
  for (int i=0; i < MAXJOBS; i++)
    if (jobs[i] != 0)
      printf("[%d] %d %s\t\t%s\n", i+1, jobs[i], status[(int) job_status[i]], job_cmd[i]);
}

void save_job_cmd(pid_t pid, char *argv[], int bg) {
  int job_i = get_jid(pid) - 1;

  int i = 0;
  int arg = 0;
  int j = 0;
  while (i < MAXCMD - 1 && argv[arg] != NULL) {
    job_cmd[job_i][i++] = argv[arg][j++];
    if (argv[arg][j] == '\0') {
      arg++;
      j=0;
      if (i < MAXCMD - 1)
        job_cmd[job_i][i++] = ' ';
    }
  }

  if ((i < MAXCMD - 2) && bg) {
    job_cmd[job_i][i++] = ' ';
    job_cmd[job_i][i++] = '&';
  }
    
  job_cmd[job_i][i] = '\0';
}

void resume_fg_job(char **argv) {
  pid_t pid;
  if ((pid = parse_pid(argv[1])) > 0) {
    fg_job = pid;
    Kill(-pid, SIGCONT);
    job_status[get_jid(pid)-1] = 0; /* Status: running */
    if (waitpid(pid, NULL, 0) < 0)
      unix_error("waitfg: waitpid error");
    return;
  }

  printf("%s: No such %s\n", argv[1], *argv[1] == '%' ? "job" : "process");
}

void resume_bg_job(char **argv) {
  pid_t pid;
  if ((pid = parse_pid(argv[1])) > 0) {
    Kill(-pid, SIGCONT);
    int jid = get_jid(pid);
    job_status[jid-1] = 0; /* Status: running */
    printf("[%d] %d\t\t\t%s\n", jid, pid, job_cmd[jid-1]);
    return;
  }
  
  printf("%s: No such %s\n", argv[1], *argv[1] == '%' ? "job" : "process");
}

void terminate_fg() {
  Kill(-fg_job, SIGINT);

  int status;
  Waitpid(fg_job, &status, 0);
  if (WIFSIGNALED(status)) {
    char s[50];
    sprintf(s, "Job [%d] %d terminated by signal", get_jid(fg_job), fg_job);
    psignal(WTERMSIG(status), s);
  }
    
  release_job(fg_job);
}

void stop_fg() {
  Kill(-fg_job, SIGTSTP);

  int status;
  Waitpid(fg_job, &status, WUNTRACED);
  if (WIFSTOPPED(status)) {
    char s[50];
    sprintf(s, "Job [%d] %d stopped by signal", get_jid(fg_job), fg_job);
    psignal(WSTOPSIG(status), s);
  }

  /* Set job status to stopped */
  job_status[get_jid(fg_job) -1] = 1;
}

void reap_terminated_children() {
  int status;
  pid_t terminated_pid;
  while ((terminated_pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (WIFSIGNALED(status)) {
      char s[50];
      sprintf(s, "Job %d terminated by signal", terminated_pid);
      psignal(WTERMSIG(status), s);
    } else
      print_terminated_job(terminated_pid);
    release_job(terminated_pid);
  }
}

static void reap_nonterminated_children() {
  for (int i=0; i < MAXJOBS; i++)
    if (jobs[i] != 0) {
      if (job_status[i] == 1)  /* status: stopped */
        Kill(-jobs[i], SIGCONT); 
      Kill(-jobs[i], SIGTERM);
      Waitpid(jobs[i], NULL, 0);
      jobs[i] = 0;
    }
}

void reap_all_children() {
  reap_terminated_children();
  reap_nonterminated_children();
}

static pid_t parse_pid(char *arg) {
  int jid;
  if (*arg == '%') {
    jid = atoi((arg+1));
    if (jid >= MAXJOBS)
      return -1;
    /* check jid is set to a valid process */ 
    return jobs[jid -1] > 0 ? jobs[jid-1] : -1;
  }

  int pid;
  pid = atoi(arg);
  if (pid <= 0)
    return -1;
  for (int i=0; i < MAXJOBS; i++)
    if (jobs[i] == pid)
      return pid;
  /* pid is not part of jobs */
  return -1;
}

static void print_terminated_job(pid_t pid) {
  int job_i = get_jid(pid) - 1;
  printf("[%d] %d Done\t\t\t%s\n", job_i + 1, pid, job_cmd[job_i]);
}
