#include <signal.h>

void sigchld_handler(int signal) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void setup_signal_listener() {
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		exit(1);
	}
}