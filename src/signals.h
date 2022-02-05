void sigint_sigtstp_handler(int sig);
void sigusr1_handler(int sig);
handler_t *add_signal_handler(int signum, handler_t *handler, int blocked_signum);
