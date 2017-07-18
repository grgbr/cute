#include "core.h"
#include <cute/cute.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define POSIX_DEFAULT_TIMEOUT (5U)

#define POSIX_PIPE_READ_FD    (0U)
#define POSIX_PIPE_WRITE_FD   (1U)
#define POSIX_PIPE_FDS_NR     (2U)

static size_t            posix_buffer_size;
static char             *posix_buffer;
static int               posix_result_pipe[POSIX_PIPE_FDS_NR];
static int               posix_console_pipe[POSIX_PIPE_FDS_NR];
static unsigned int      posix_default_timeout = POSIX_DEFAULT_TIMEOUT;
static sig_atomic_t      posix_watchdog_expired;
static struct sigaction  posix_sigact;
static struct sigaction  posix_default_sigact;

static void
posix_handle_signal(int signo)
{
	if (signo == SIGALRM)
		posix_watchdog_expired = 1;
}

static int
posix_write_result_pipe(const char *data, size_t size)
{
	ssize_t ret;

	while (true) {
		ret = write(posix_result_pipe[POSIX_PIPE_WRITE_FD], data, size);
		if (ret <= 0)
			break;

		size -= ret;
		if (!size)
			break;

		data += ret;
	}

	if (!size)
		return 0;

	if (ret <= 0)
		return ret ? ret : -EPIPE;

	return 0;
}

static ssize_t
posix_clear_pipe(int pipe[POSIX_PIPE_FDS_NR])
{
	char    data;
	int     err;
	ssize_t bytes = 0;

	while (true) {
		err = read(pipe[POSIX_PIPE_READ_FD], &data, 1U);
		if (err <= 0)
			break;

		bytes++;
	}

	return (err < 0) ? err : bytes;
}

static ssize_t
posix_read_pipe(int pipe[POSIX_PIPE_FDS_NR], char *buffer)
{
	ssize_t  ret = 0;
	ssize_t  left = (ssize_t)posix_buffer_size;
	char    *data = buffer;

	while (true) {
		ret = read(pipe[POSIX_PIPE_READ_FD], data, (size_t)left);
		if (ret <= 0)
			break;

		left -= ret;
		if (!left)
			break;

		data += ret;
	}

	if (!ret)
		/*
		 * Child closed its pipe writing end, meaning end of stream:
		 * return number of bytes read so far.
		 */
		return posix_buffer_size - left;

	if (ret < 0)
		/* Error occured: return errno code. */
		return -errno;

	ret = posix_clear_pipe(pipe);
	if (ret < 0)
		return ret;

	if (!ret)
		return posix_buffer_size;

	return -ENOBUFS;
}

static int
posix_build_freeze_result(struct cute_test *test)
{
	struct cute_result *res = &test->result;

	if (asprintf(&res->reason,
	             "timed out after %u seconds", test->timeout) <= 0) {
		res->reason = NULL;
		return -ENOMEM;
	}

	res->state = CUTE_ERROR_STATE;

	return 0;
}

static int
posix_build_abort_result(struct cute_test *test)
{
	struct cute_result *res = &test->result;

	if (asprintf(&res->reason, "aborted") <= 0) {
		res->reason = NULL;
		return -ENOMEM;
	}

	res->state = CUTE_ERROR_STATE;

	return 0;
}

static int
posix_build_segv_result(struct cute_test *test)
{
	struct cute_result *res = &test->result;

	if (asprintf(&res->reason, "segfaulted") <= 0) {
		res->reason = NULL;
		return -ENOMEM;
	}

	res->state = CUTE_ERROR_STATE;

	return 0;
}

static int
posix_build_signo_result(struct cute_test *test, int signo)
{
	struct cute_result *res = &test->result;

	if (asprintf(&res->reason,
	             "caught unhandled \"%s\" signal", strsignal(signo)) <= 0) {
		res->reason = NULL;
		return -ENOMEM;
	}

	res->state = CUTE_ERROR_STATE;

	return 0;
}

static int
posix_build_unknown_result(struct cute_test *test)
{
	struct cute_result *res = &test->result;

	if (asprintf(&res->reason, "unknown error") <= 0) {
		res->reason = NULL;
		return -ENOMEM;
	}

	res->state = CUTE_ERROR_STATE;

	return 0;
}

static int
posix_build_failure_result(struct cute_test *test)
{
	size_t              left = (ssize_t)posix_buffer_size;
	char               *data = posix_buffer;
	size_t              len;
	struct cute_result *res = &test->result;

	data[left - 1] = '\0';

	len = strnlen(data, 5);
	if (!len || (len >= 5))
		return -ERANGE;

	res->line = strdup(data);
	if (!res->line)
		return -ENOMEM;

	len++;
	data += len;
	left -= len;

	len = strnlen(data, left - 1);
	if (!len || (len >= (left - 1)))
		return -ERANGE;

	res->file = strdup(data);
	if (!res->file)
		return -ENOMEM;

	len++;
	data += len;
	left -= len;

	if ((left <= 2) || !strlen(data))
		return -ERANGE;

	res->reason = strdup(data);
	if (!res->reason)
		return -ENOMEM;

	res->state = CUTE_FAILURE_STATE;

	return 0;
}

static int
posix_fill_test_result(struct cute_test *test, int error, int child_status)
{
	struct cute_result *res = &test->result;
	int                 ret;

	ret = posix_read_pipe(posix_console_pipe, posix_buffer);
	if (ret < 0)
		return ret;

	if (ret > 0) {
		res->console = strndup(posix_buffer, posix_buffer_size - 1);
		if (!res->console)
			return -ENOMEM;
	}

	if ((error > 0) || posix_watchdog_expired)
		return posix_build_freeze_result(test);

	if (WIFEXITED(child_status)) {
		if (WEXITSTATUS(child_status) == EXIT_SUCCESS) {
			res->state = CUTE_SUCCESS_STATE;
			return 0;
		}

		return posix_build_failure_result(test);
	}

	if (WIFSIGNALED(child_status)) {
		int sig = WTERMSIG(child_status);

		if (sig == SIGABRT)
			return posix_build_abort_result(test);
		if (sig == SIGSEGV)
			return posix_build_segv_result(test);
		else
			return posix_build_signo_result(test, sig);
	}

	return posix_build_unknown_result(test);
}

static void __noreturn
posix_exec_test_child(const struct cute_test *test)
{
	int                      status = EXIT_FAILURE;
	const struct cute_suite *suite = (struct cute_suite *)
	                                 test->object.parent;

	sigaction(SIGALRM, &posix_default_sigact, NULL);

	if (!test->run)
		goto exit;

	if (close(posix_result_pipe[POSIX_PIPE_READ_FD]))
		goto exit;

	if (close(posix_console_pipe[POSIX_PIPE_READ_FD]))
		goto exit;

	if (dup2(posix_console_pipe[POSIX_PIPE_WRITE_FD], STDERR_FILENO) !=
	    STDERR_FILENO)
		goto exit;

	if (close(posix_console_pipe[POSIX_PIPE_WRITE_FD]))
		goto exit;

	if (suite && suite->setup)
		suite->setup();

	test->run();

	if (suite && suite->teardown)
		suite->teardown();

	status = EXIT_SUCCESS;

exit:
	exit(status);
}

static int
posix_wait_child(pid_t pid, int *child_status)
{
	int err;

	while (true) {
		err = wait(child_status);
		if (err > 1) {
			err = 0;
			/* Child terminated. */
			break;
		}

		if ((err < 0) && (errno != EINTR)) {
			/* Child has already been terminated. */
			assert(errno == ECHILD);
			err = -ECHILD;
			break;
		}

		/*
		 * We were interrupted while waiting for child termination
		 * either by the watchdog, the child termination SIGCHLD signal
		 * itself or another terminal related signal.
		 * In all cases we want to complete child termination.
		 */
		kill(pid, SIGKILL);
	}

	/* Immediatly disable watchdog as we won't need it anymore. */
	if (!posix_watchdog_expired)
		alarm(0);

	return err;
}

static int
posix_wait_test_child(pid_t pid, int *status)
{
	ssize_t ret;

	ret = posix_read_pipe(posix_result_pipe, posix_buffer);

	if ((ret == -EINTR) && posix_watchdog_expired) {
		kill(pid, SIGKILL);

		/*
		 * Watchdog expired before child has closed its pipe writing
		 * end, meaning it is probably frozen (infinite loop ?).
		 */
		ret = posix_wait_child(pid, NULL);
		if (ret)
			return ret;

		return 1;
	}

	if ((ret < 0) && (ret != -EINTR)) {
		/*
		 * Something bad happened while reading pipe: return fatal error
		 * since the whole program activity is in undefined state.
		 * Disable watchdog as well since it hasn't expired yet and we
		 * don't need it anymore.
		 */
		posix_wait_child(pid, NULL);

		return ret;
	}

	/*
	 * Wait for the child process to vanish either on its own or thanks to
	 * the watchdog which is still running (or another terminating signal).
	 */
	ret = posix_wait_child(pid, status);
	if (ret < 0)
		return ret;

	return 0;
}

static int
posix_spawn_test(struct cute_test *test)
{
	pid_t pid;
	int   err;
	int   stat;

	if (pipe(posix_result_pipe))
		return -errno;

	if (pipe(posix_console_pipe)) {
		err = -errno;
		goto result;
	}

	pid = fork();
	if (pid < 0) {
		err = -errno;
		goto console;
	}

	if (!pid) {
		/* testing child process side: does not return */
		posix_exec_test_child(test);
		assert(0);
	}

	posix_watchdog_expired = 0;

	if (!test->timeout)
		test->timeout = posix_default_timeout;

	alarm(test->timeout);

	close(posix_result_pipe[POSIX_PIPE_WRITE_FD]);
	close(posix_console_pipe[POSIX_PIPE_WRITE_FD]);

	err = posix_wait_test_child(pid, &stat);
	if (err >= 0)
		err = posix_fill_test_result(test, err, stat);

	close(posix_result_pipe[POSIX_PIPE_READ_FD]);
	close(posix_console_pipe[POSIX_PIPE_READ_FD]);

	if (err)
		return err;

	return 0;

console:
	close(posix_console_pipe[POSIX_PIPE_READ_FD]);
	close(posix_console_pipe[POSIX_PIPE_WRITE_FD]);

result:
	close(posix_result_pipe[POSIX_PIPE_READ_FD]);
	close(posix_result_pipe[POSIX_PIPE_WRITE_FD]);

	return err;
}

static int
posix_init_run(unsigned int default_timeout)
{
	posix_buffer_size = (size_t)sysconf(_SC_PAGESIZE);

	posix_buffer = malloc(posix_buffer_size);
	if (!posix_buffer)
		return -ENOMEM;

	if (default_timeout > 0)
		posix_default_timeout = default_timeout;

	posix_sigact.sa_handler = posix_handle_signal;
	sigemptyset(&posix_sigact.sa_mask);
	sigaction(SIGALRM, &posix_sigact, &posix_default_sigact);

	return 0;
}

static void
posix_fini_run(void)
{
	sigaction(SIGALRM, &posix_default_sigact, NULL);

	free(posix_buffer);
}

void __noreturn
posix_expect_failed(const char *line, const char *file, const char *reason)
{
	if (!posix_write_result_pipe(line, strlen(line) + 1))
		if (!posix_write_result_pipe(file, strlen(file) + 1))
			posix_write_result_pipe(reason, strlen(reason) + 1);

	exit(EXIT_FAILURE);
}

static const struct core_run posix_run = {
	.expect_failed = posix_expect_failed,
	.spawn_test    = posix_spawn_test,
	.fini_run      = posix_fini_run,
};

int
cute_setup_posix_run(unsigned int default_timeout)
{
	int err;

	err = posix_init_run(default_timeout);
	if (err)
		return err;

	core_current_run = &posix_run;

	return 0;
}
