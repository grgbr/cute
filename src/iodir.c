#include "iodir.h"
#include "common.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <poll.h>
#include <sys/eventfd.h>

/******************************************************************************
 * Standard I/O text block handling
 ******************************************************************************/

struct cute_iodir_atom {
	struct cute_iodir_atom * next;
	size_t                   busy;
	char                     data[1024];
};

static void
cute_iodir_atom_push_data(struct cute_iodir_atom * atom, size_t size)
{
	cute_assert_intern(atom);
	cute_assert_intern(size);
	cute_assert_intern((atom->busy + size) <= sizeof(atom->data));

	atom->busy += size;
}

static inline size_t
cute_iodir_atom_busy_size(const struct cute_iodir_atom * atom)
{
	cute_assert_intern(atom);
	cute_assert_intern(atom->busy <= sizeof(atom->data));

	return atom->busy;
}

static const char *
cute_iodir_atom_busy_data(const struct cute_iodir_atom * atom)
{
	cute_assert_intern(atom);
	cute_assert_intern(atom->busy <= sizeof(atom->data));

	return atom->data;
}

static size_t
cute_iodir_atom_free_size(const struct cute_iodir_atom * atom)
{
	cute_assert_intern(atom);
	cute_assert_intern(atom->busy <= sizeof(atom->data));

	return sizeof(atom->data) - atom->busy;
}

static char *
cute_iodir_atom_free_data(struct cute_iodir_atom * atom)
{
	cute_assert_intern(atom);
	cute_assert_intern(atom->busy <= sizeof(atom->data));

	return &atom->data[atom->busy];
}

static struct cute_iodir_atom *
cute_iodir_alloc_atom(void)
{
	struct cute_iodir_atom * atom;

	atom = cute_malloc(sizeof(*atom));
	atom->next = NULL;
	atom->busy = 0;

	return atom;
}

static void
cute_iodir_free_atom(struct cute_iodir_atom * atom)
{
	cute_assert_intern(atom);
	cute_assert_intern(atom->busy <= sizeof(atom->data));

	cute_free(atom);
}

bool
cute_iodir_is_block_busy(const struct cute_iodir_block * block)
{
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);

	return !!cute_iodir_atom_busy_size(block->head);
}

void
cute_iodir_print_block(FILE *                          stdio,
                       int                             depth,
                       const struct cute_iodir_block * block)
{
	cute_assert_intern(stdio);
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);

	const struct cute_iodir_atom * atom = block->head;

	do {
		int          sz;
		const char * line;

		sz = (int)cute_iodir_atom_busy_size(atom);
		if (!sz)
			break;

		line = cute_iodir_atom_busy_data(atom);
		do {
			const char * eol;
			int          len;

			eol = memchr(line, '\n', (size_t)sz);
			if (!eol)
				eol = &line[sz];
			len = (int)(eol - line);

			fprintf(stdio, "%*.s%.*s\n", depth, "", len, line);

			line = eol + 1;
			sz -= len + 1;
		} while (sz > 0);

		atom = atom->next;
	} while (atom);
}

static struct cute_iodir_atom *
cute_iodir_block_atom(struct cute_iodir_block * block)
{
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);

	struct cute_iodir_atom * tail = block->tail;

	cute_assert_intern(tail->busy <= sizeof(tail->data));
	if (tail->busy == sizeof(tail->data)) {
		struct cute_iodir_atom * atom;

		atom = cute_iodir_alloc_atom();

		tail->next = atom;
		block->tail = atom;
	}

	return block->tail;
}

void
cute_iodir_init_block(struct cute_iodir_block * block)
{
	cute_assert_intern(block);

	struct cute_iodir_atom * head;

	head = cute_iodir_alloc_atom();

	block->head = head;
	block->tail = head;
}

void
cute_iodir_fini_block(struct cute_iodir_block * block)
{
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);

	struct cute_iodir_atom * atom = block->head;

	do {
		struct cute_iodir_atom * next = atom->next;

		cute_iodir_free_atom(atom);
		atom = next;
	} while (atom);
}

/******************************************************************************
 * Standard I/Os collection / redirection handling
 ******************************************************************************/

enum cute_iodir_state {
	CUTE_IODIR_IDLE_STAT,
	CUTE_IODIR_REDIR_STAT,
	CUTE_IODIR_FLUSH_STAT,
	CUTE_IODIR_HALT_STAT
};

static int                       cute_iodir_stdout_pipe[2];
static struct cute_iodir_block * cute_iodir_stdout_block;

static int                       cute_iodir_stderr_pipe[2];
static struct cute_iodir_block * cute_iodir_stderr_block;

static pthread_mutex_t           cute_iodir_lck;
static pthread_cond_t            cute_iodir_cond;
static int                       cute_iodir_notifier;
static pthread_t                 cute_iodir_thr;
static enum cute_iodir_state     cute_iodir_stat;

FILE *                           cute_iodir_stdout;
FILE *                           cute_iodir_stderr;

static FILE *
cute_iodir_dup_stdio(FILE * stdio)
{
	int fd;

	fd = fileno(stdio);
	cute_assert_intern(fd >= 0);

	fflush(stdio);

	fd = cute_dup(fd);
	if (fd < 0) {
		errno = -fd;
		return NULL;
	}

	return fdopen(fd, "w");
}

static void
cute_iodir_notify(void)
{
	ssize_t  sz;
	uint64_t val = 1;

	sz = write(cute_iodir_notifier, &val, sizeof(val));
	if (sz != sizeof(val)) {
		cute_assert_intern(sz < 0);
		cute_assert_intern(errno == EAGAIN);
	}
}

static void
cute_iodir_clear_notify(int fd)
{
	cute_assert_intern(fd >= 0);

	uint64_t val;
	ssize_t  sz;

	sz = read(fd, &val, sizeof(val));
	if (sz != sizeof(val)) {
		cute_assert_intern(sz < 0);
		cute_assert_intern(errno == EAGAIN);
	}
}

static void
cute_iodir_handle_notify(const struct pollfd * poll)
{
	short evts = poll->revents;

	cute_assert_intern(!(evts & ~(POLLIN | POLLERR)));

	if (evts & (POLLIN | POLLERR))
		cute_iodir_clear_notify(poll->fd);
}

static int
cute_iodir_open_notify(void)
{
	cute_iodir_notifier = eventfd(0, EFD_NONBLOCK);
	if (cute_iodir_notifier >= 0)
		return 0;

	cute_assert_intern(errno != EINVAL);

	return -errno;
}

static void
cute_iodir_close_notify(void)
{
	if (!close(cute_iodir_notifier))
		return;

	cute_assert_intern(errno != EBADF);
}

void
cute_iodir_redirect(struct cute_iodir_block * stdout_block,
                    struct cute_iodir_block * stderr_block)
{
	cute_assert_intern(stdout_block);
	cute_assert_intern(stdout_block->head);
	cute_assert_intern(stdout_block->tail);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(stderr_block);
	cute_assert_intern(stderr_block->head);
	cute_assert_intern(stderr_block->tail);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr);

	cute_lock(&cute_iodir_lck);

	cute_assert_intern(cute_iodir_stat == CUTE_IODIR_IDLE_STAT);
	cute_assert_intern(!cute_iodir_stdout_block);
	cute_assert_intern(!cute_iodir_stderr_block);

	cute_iodir_stdout_block = stdout_block;
	cute_iodir_stderr_block = stderr_block;
	cute_iodir_stat = CUTE_IODIR_REDIR_STAT;
	cute_iodir_notify();

	cute_unlock(&cute_iodir_lck);
}

void
cute_iodir_restore(void)
{
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr);

	fflush(NULL);
	sync();

	cute_lock(&cute_iodir_lck);

	cute_assert_intern((cute_iodir_stat == CUTE_IODIR_REDIR_STAT) ||
	                   (cute_iodir_stat == CUTE_IODIR_HALT_STAT));
	cute_assert_intern(cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_block);

	cute_iodir_stat = CUTE_IODIR_FLUSH_STAT;
	cute_iodir_notify();

	while ((cute_iodir_stat != CUTE_IODIR_IDLE_STAT) &&
	       (cute_iodir_stat != CUTE_IODIR_HALT_STAT))
		cute_cond_wait(&cute_iodir_cond, &cute_iodir_lck);

	if (cute_iodir_stat == CUTE_IODIR_HALT_STAT) {
		int * err;

		cute_unlock(&cute_iodir_lck);
		cute_thr_join(cute_iodir_thr, (void **)&err);
		cute_assert_intern(*err);

		cute_error("I/O thread has halted unexpectedly: %s (%d)\n",
		           strerror(*err),
		           *err);

		exit(EXIT_FAILURE);
	}

	cute_iodir_stdout_block = NULL;
	cute_iodir_stderr_block = NULL;

	cute_unlock(&cute_iodir_lck);
}

static enum cute_iodir_state
cute_iodir_scan_query(struct cute_iodir_block ** out,
                      struct cute_iodir_block ** err)
{
	cute_assert_intern(out);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(err);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr);

	enum cute_iodir_state stat;

	cute_lock(&cute_iodir_lck);

	switch (cute_iodir_stat) {
	case CUTE_IODIR_REDIR_STAT:
	case CUTE_IODIR_FLUSH_STAT:
		cute_assert_intern(cute_iodir_stdout_block);
		cute_assert_intern(cute_iodir_stderr_block);
		*out = cute_iodir_stdout_block;
		*err = cute_iodir_stderr_block;
		break;

	case CUTE_IODIR_IDLE_STAT:
	case CUTE_IODIR_HALT_STAT:
		*out = NULL;
		*err = NULL;
		break;

	default:
		cute_assert_intern(0);
	}

	stat = cute_iodir_stat;

	cute_unlock(&cute_iodir_lck);

	return stat;
}

static void
cute_iodir_wakeup(enum cute_iodir_state stat)
{
	cute_lock(&cute_iodir_lck);
	cute_iodir_stat = stat;
	cute_cond_signal(&cute_iodir_cond);
	cute_unlock(&cute_iodir_lck);
}

static int
cute_iodir_read(int fd, struct cute_iodir_block * block)
{
	cute_assert_intern(fd >= 0);
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);

	struct cute_iodir_atom * atom;
	ssize_t                  sz;

	atom = cute_iodir_block_atom(block);

	sz = read(fd,
	          cute_iodir_atom_free_data(atom),
	          cute_iodir_atom_free_size(atom));
	cute_assert_intern(sz);

	if (sz > 0) {
		cute_iodir_atom_push_data(atom, (size_t)sz);
		return 0;
	}

	cute_assert_intern(errno == EAGAIN);

	return -errno;
}

static void
cute_iodir_process(struct pollfd * poll, struct cute_iodir_block * block)
{
	cute_assert_intern(poll);
	cute_assert_intern(poll->fd >= 0);
	cute_assert_intern(poll->events);
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr);
	cute_assert_intern(cute_iodir_stderr_block);

	cute_assert_intern(!(poll->revents & ~POLLIN));

	if (poll->revents & POLLIN) {
		int ret;

		do {
			ret = cute_iodir_read(poll->fd, block);
		} while (!ret);
	}
}

static void
cute_iodir_flush(struct pollfd * poll, struct cute_iodir_block * block)
{
	cute_assert_intern(poll);
	cute_assert_intern(poll->fd >= 0);
	cute_assert_intern(poll->events);
	cute_assert_intern(block);
	cute_assert_intern(block->head);
	cute_assert_intern(block->tail);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr);
	cute_assert_intern(cute_iodir_stderr_block);

	int ret;

	do {
		ret = cute_iodir_read(poll->fd, block);
	} while (!ret);
}

#define CUTE_IODIR_POLL_NR (3U)

static int
cute_iodir_poll(struct pollfd fds[CUTE_IODIR_POLL_NR])
{
	int ret;

	ret = poll(fds, CUTE_IODIR_POLL_NR, -1);
	if (ret < 0) {
		cute_assert_intern(errno == ENOMEM);
		ret = -errno;
	}

	return ret;
}

static int
cute_iodir_handle_idle(struct pollfd fds[CUTE_IODIR_POLL_NR])
{
	int err;

	err = cute_iodir_poll(fds);
	if (err < 0)
		return err;

	cute_iodir_handle_notify(&fds[2]);

	return 0;
}

static int
cute_iodir_handle_redir(struct pollfd             fds[CUTE_IODIR_POLL_NR],
                        struct cute_iodir_block * stdout_block,
                        struct cute_iodir_block * stderr_block)

{
	int err;

	cute_iodir_process(&fds[0], stdout_block);

	cute_iodir_process(&fds[1], stderr_block);

	err = cute_iodir_poll(fds);
	if (err < 0)
		return err;

	cute_iodir_handle_notify(&fds[2]);

	return 0;
}

static void
cute_iodir_handle_flush(struct pollfd             fds[CUTE_IODIR_POLL_NR],
                        struct cute_iodir_block * stdout_block,
                        struct cute_iodir_block * stderr_block)
{
	cute_iodir_flush(&fds[0], stdout_block);

	cute_iodir_flush(&fds[1], stderr_block);

	cute_iodir_clear_notify(fds[2].fd);

	cute_iodir_wakeup(CUTE_IODIR_IDLE_STAT);
}

static void *
cute_iodir_capture(void * arg __cute_unused)
{
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr);

	static int    ret;
	struct pollfd fds[CUTE_IODIR_POLL_NR] = {
		[0] = {
			.fd      = cute_iodir_stdout_pipe[0],
			.events  = POLLIN | POLLERR | POLLHUP | POLLNVAL,
			.revents = 0
		},
		[1] = {
			.fd      = cute_iodir_stderr_pipe[0],
			.events  = POLLIN | POLLERR | POLLHUP | POLLNVAL,
			.revents = 0
		},
		[2] = {
			.fd      = cute_iodir_notifier,
			.events  = POLLIN | POLLERR | POLLHUP | POLLNVAL,
			.revents = 0
		}
	};

	do {
		struct cute_iodir_block * oblk;
		struct cute_iodir_block * eblk;

		switch (cute_iodir_scan_query(&oblk, &eblk)) {
		case CUTE_IODIR_IDLE_STAT:
			ret = cute_iodir_handle_idle(fds);
			break;

		case CUTE_IODIR_REDIR_STAT:
			ret = cute_iodir_handle_redir(fds, oblk, eblk);
			break;

		case CUTE_IODIR_FLUSH_STAT:
			cute_iodir_handle_flush(fds, oblk, eblk);
			break;

		case CUTE_IODIR_HALT_STAT :
			ret = 0;
			goto exit;

		default:
			cute_assert_intern(0);
		}
	} while (!ret);

	cute_iodir_wakeup(CUTE_IODIR_HALT_STAT);

exit:
	pthread_exit(&ret);
}

static int
cute_iodir_open_pipe(int pipe_fds[2], FILE * stdio)
{
	cute_assert_intern(pipe_fds);
	cute_assert_intern(stdio);

	int ret;

	ret = cute_pipe(pipe_fds);
	if (ret)
		return ret;

	cute_setup_nonblock(pipe_fds[0]);

	ret = fileno(stdio);
	cute_assert_intern(ret >= 0);

	fflush(stdio);

	ret = cute_dup2(pipe_fds[1], ret);
	if (ret < 0)
		return ret;

	return 0;
}

static void
cute_iodir_close_pipe(int pipe_fds[2], FILE * stdio, int stdfd)
{
	cute_assert_intern(pipe_fds);
	cute_assert_intern(pipe_fds[0] >= 0);
	cute_assert_intern(pipe_fds[1] >= 0);
	cute_assert_intern(stdio);
	cute_assert_intern(stdfd >= 0);

	int fd;
	int err;

	fd = fileno(stdio);
	cute_assert_intern(fd >= 0);

	do {
		err = cute_dup2(fd, stdfd);
	} while (err == -EINTR);

	close(pipe_fds[0]);
	close(pipe_fds[1]);
}

int
cute_iodir_init(void)
{
	const char * msg;
	int          err;
	sigset_t     set;

	cute_iodir_stdout = cute_iodir_dup_stdio(stdout);
	if (!cute_iodir_stdout) {
		err = -errno;
		msg = "cannot duplicate stdout";
		goto err;
	}

	cute_iodir_stderr = cute_iodir_dup_stdio(stderr);
	if (!cute_iodir_stderr) {
		err = -errno;
		msg = "cannot duplicate stderr";
		goto close_stdout;
	}

	err = cute_iodir_open_pipe(cute_iodir_stdout_pipe, stdout);
	if (err) {
		msg = "cannot redirect stdout";
		goto close_stderr;
	}

	err = cute_iodir_open_pipe(cute_iodir_stderr_pipe, stderr);
	if (err) {
		msg = "cannot redirect stderr";
		goto close_out_pipe;
	}

	err = cute_iodir_open_notify();
	if (err) {
		msg = "cannot open notification channel";
		goto close_err_pipe;
	}

	err = cute_lock_init(&cute_iodir_lck);
	if (err) {
		msg = "cannot setup locking";
		goto close_notify;
	}

	err = cute_cond_init(&cute_iodir_cond);
	if (err) {
		msg = "cannot setup condition variable";
		goto fini_lock;
	}

	cute_iodir_stat = CUTE_IODIR_IDLE_STAT;
	cute_iodir_stdout_block = NULL;
	cute_iodir_stderr_block = NULL;

	err = sigfillset(&set);
	cute_assert_intern(!err);
	err = cute_thr_create(&cute_iodir_thr, &set, cute_iodir_capture, NULL);
	if (err) {
		msg = "cannot spawn thread";
		goto fini_cond;
	}

	return 0;

fini_cond:
	cute_cond_fini(&cute_iodir_cond);
fini_lock:
	cute_lock_fini(&cute_iodir_lck);
close_notify:
	cute_iodir_close_notify();
close_err_pipe:
	cute_iodir_close_pipe(cute_iodir_stderr_pipe,
	                      cute_iodir_stderr,
	                      STDERR_FILENO);
close_out_pipe:
	cute_iodir_close_pipe(cute_iodir_stdout_pipe,
	                      cute_iodir_stdout,
	                      STDOUT_FILENO);
close_stderr:
	fclose(cute_iodir_stderr);
close_stdout:
	fclose(cute_iodir_stdout);
err:
	cute_error("cannot setup standard I/Os: %s: %s (%d)\n",
	           msg,
	           strerror(-err),
	           -err);

	return err;
}

void
cute_iodir_fini(void)
{
	cute_assert_intern(cute_iodir_stat == CUTE_IODIR_IDLE_STAT);
	cute_assert_intern(cute_iodir_stdout);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(!cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(!cute_iodir_stderr_block);

	cute_lock(&cute_iodir_lck);
	cute_iodir_stat = CUTE_IODIR_HALT_STAT;
	cute_iodir_notify();
	cute_unlock(&cute_iodir_lck);

	cute_thr_join(cute_iodir_thr, NULL);

	cute_cond_fini(&cute_iodir_cond);
	cute_lock_fini(&cute_iodir_lck);

	cute_iodir_close_notify();

	cute_iodir_close_pipe(cute_iodir_stderr_pipe,
	                      cute_iodir_stderr,
	                      STDERR_FILENO);
	cute_iodir_close_pipe(cute_iodir_stdout_pipe,
	                      cute_iodir_stdout,
	                      STDOUT_FILENO);

	fclose(cute_iodir_stdout);
	fclose(cute_iodir_stderr);
}
