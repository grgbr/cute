#include "iodir.h"
#include "util.h"
#include <poll.h>
#include <stdlib.h>
#include <string.h>
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

	atom = malloc(sizeof(*atom));
	atom->next = NULL;
	atom->busy = 0;

	return atom;
}

static void
cute_iodir_free_atom(struct cute_iodir_atom * atom)
{
	cute_assert_intern(atom);
	cute_assert_intern(atom->busy <= sizeof(atom->data));

	free(atom);
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
static int                       cute_iodir_stdout_orig;
static struct cute_iodir_block * cute_iodir_stdout_block;

static int                       cute_iodir_stderr_pipe[2];
static int                       cute_iodir_stderr_orig;
static struct cute_iodir_block * cute_iodir_stderr_block;

static pthread_mutex_t           cute_iodir_lck;
static pthread_cond_t            cute_iodir_cond;
static int                       cute_iodir_notifier;
static pthread_t                 cute_iodir_thr;
static enum cute_iodir_state     cute_iodir_stat;

static void
cute_iodir_notify(void)
{
	uint64_t val = 1;

	if (write(cute_iodir_notifier, &val, sizeof(val)) == sizeof(val))
		return;

	cute_assert_intern(errno != EINVAL);
	cute_assert_intern(errno != EBADF);
	cute_assert_intern(errno != EDESTADDRREQ);
	cute_assert_intern(errno != EFAULT);
	cute_assert_intern(errno != EFBIG);
	cute_assert_intern(errno != EINTR);
	cute_assert_intern(errno != EPERM);
	cute_assert_intern(errno != EPIPE);
}

static int
cute_iodir_handle_notify(struct pollfd * poll)
{
	short evts = poll->revents;

	poll->revents = 0;

	if (evts & (POLLIN | POLLHUP)) {
		uint64_t val;

		if (read(cute_iodir_notifier, &val, sizeof(val)) == sizeof(val))
			return 0;

		cute_assert_intern(errno != EINVAL);
		cute_assert_intern(errno != EBADF);
		cute_assert_intern(errno != EFAULT);
		cute_assert_intern(errno != EISDIR);

		return -errno;
	}
	else if (!evts)
		return 0;
	else if (evts & POLLNVAL)
		return -EBADF;
	else if (evts & POLLERR)
		return -EIO;
	else
		return -EPERM;
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
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(stderr_block);
	cute_assert_intern(stderr_block->head);
	cute_assert_intern(stderr_block->tail);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);

	int err;

	fflush(stdout);
	err = cute_dup2(cute_iodir_stdout_pipe[1], STDOUT_FILENO);
	if (err < 0)
		goto err;

	fflush(stderr);
	err = cute_dup2(cute_iodir_stderr_pipe[1], STDERR_FILENO);
	if (err < 0)
		goto restore;

	cute_lock(&cute_iodir_lck);

	cute_assert_intern(!cute_iodir_stdout_block);
	cute_assert_intern(!cute_iodir_stderr_block);
	cute_assert_intern(cute_iodir_stat == CUTE_IODIR_IDLE_STAT);

	cute_iodir_stdout_block = stdout_block;
	cute_iodir_stderr_block = stderr_block;
	cute_iodir_stat = CUTE_IODIR_REDIR_STAT;

	cute_unlock(&cute_iodir_lck);
	cute_cond_signal(&cute_iodir_cond);

	return;

restore:
	while (cute_dup2(cute_iodir_stdout_orig, STDOUT_FILENO) == -EINTR)
		;
err:
	cute_error("cannot redirect standard I/Os: %s (%d)\n",
	           strerror(-err),
	           -err);
	exit(EXIT_FAILURE);
}

void
cute_iodir_restore(void)
{
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);

	int err;
	int stat = 0;

	fflush(stdout);
	do {
		err = cute_dup2(cute_iodir_stdout_orig, STDOUT_FILENO);
	} while (err == -EINTR);
	if (err < 0)
		stat = -err;

	fflush(stderr);
	do {
		err = cute_dup2(cute_iodir_stderr_orig, STDERR_FILENO);
	} while (err == -EINTR);
	if ((err < 0) && !stat)
		stat = -err;

	if (stat)  {
		cute_error("cannot restore standard I/Os redirection: "
		           "%s (%d)\n",
		           strerror(stat),
		           stat);
		exit(EXIT_FAILURE);
	}

	cute_lock(&cute_iodir_lck);

	if (cute_iodir_stat == CUTE_IODIR_HALT_STAT) {
		cute_unlock(&cute_iodir_lck);
		cute_error("cannot restore standard I/Os redirection: "
		           "I/Os thread has halted unexpectedly.\n");
		exit(EXIT_FAILURE);
	}

	cute_assert_intern(cute_iodir_stat == CUTE_IODIR_REDIR_STAT);
	cute_assert_intern(cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_block);

	cute_iodir_stat = CUTE_IODIR_FLUSH_STAT;
	cute_iodir_notify();

	while (cute_iodir_stat != CUTE_IODIR_IDLE_STAT)
		cute_cond_wait(&cute_iodir_cond, &cute_iodir_lck);

	cute_iodir_stdout_block = NULL;
	cute_iodir_stderr_block = NULL;

	cute_unlock(&cute_iodir_lck);
}

static enum cute_iodir_state
cute_iodir_wait_query(struct cute_iodir_block ** out,
                      struct cute_iodir_block ** err)
{
	cute_assert_intern(out);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(err);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);

	enum cute_iodir_state stat;

	cute_lock(&cute_iodir_lck);

	if (cute_iodir_stat == CUTE_IODIR_IDLE_STAT) {
		do {
			cute_cond_wait(&cute_iodir_cond, &cute_iodir_lck);
		} while (cute_iodir_stat == CUTE_IODIR_IDLE_STAT);
	}

	switch (cute_iodir_stat) {
	case CUTE_IODIR_REDIR_STAT:
	case CUTE_IODIR_FLUSH_STAT:
		cute_assert_intern(cute_iodir_stdout_block);
		cute_assert_intern(cute_iodir_stderr_block);
		*out = cute_iodir_stdout_block;
		*err = cute_iodir_stderr_block;
		break;

	case CUTE_IODIR_HALT_STAT:
		break;

	case CUTE_IODIR_IDLE_STAT:
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

	if (sz > 0) {
		cute_iodir_atom_push_data(atom, (size_t)sz);
		return 0;
	}
	else if (!sz)
		return -ENODATA;

	cute_assert_intern(errno != EFAULT);
	cute_assert_intern(errno != EINTR);
	cute_assert_intern(errno != EINVAL);
	cute_assert_intern(errno != EISDIR);

	return -errno;
}

static int
cute_iodir_end_io(struct pollfd * poll, int ret)
{
	cute_assert_intern(poll);
	cute_assert_intern(poll->fd >= 0);
	cute_assert_intern(poll->events);

	switch (ret) {
	case 0:
	case -EAGAIN:
		return 0;

	default:
		poll->fd = -1;
		return ret;
	}
}

static int
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
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);
	cute_assert_intern(cute_iodir_stderr_block);

	if (poll->fd >= 0) {
		int   ret;
		short evts = poll->revents;

		poll->revents = 0;

		if (evts & (POLLIN | POLLHUP)) {
			do {
				ret = cute_iodir_read(poll->fd, block);
			} while (!ret);
		}
		else if (evts & POLLNVAL)
			ret = -EBADF;
		else if (evts & POLLERR)
			ret = -EIO;
		else if (!evts)
			ret = -EAGAIN;
		else
			ret = -EPERM;

		return cute_iodir_end_io(poll, ret);
	}
	else
		return 0;
}

static int
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
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);
	cute_assert_intern(cute_iodir_stderr_block);

	if (poll->fd >= 0) {
		int ret;

		do {
			ret = cute_iodir_read(poll->fd, block);
		} while (!ret);

		return cute_iodir_end_io(poll, ret);
	}
	else
		return 0;
}

static int
cute_iodir_poll(struct pollfd * fds, unsigned int nr)
{
	int ret;

	ret = poll(fds, nr, -1);
	if (ret < 0) {
		cute_assert_intern(errno != EFAULT);
		cute_assert_intern(errno != EINVAL);
		ret = -errno;
	}

	return ret;
}

static void *
cute_iodir_capture(void * arg __cute_unused)
{
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);

	static int    ret = 0;
	struct pollfd fds[] = {
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

	while ((fds[0].fd >= 0) || (fds[1].fd >= 0)) {
		struct cute_iodir_block * oblk;
		struct cute_iodir_block * eblk;
		enum cute_iodir_state     stat;
		int                       err;

		stat = cute_iodir_wait_query(&oblk, &eblk);
		if (stat == CUTE_IODIR_REDIR_STAT) {
			err = cute_iodir_poll(fds,
			                      sizeof(fds) / sizeof(fds[0]));
			if (!err || (err == -EINTR))
				continue;
			if (err < 0)
				break;

			err = cute_iodir_process(&fds[0], oblk);
			if (!ret && err)
				ret = -err;
			err = cute_iodir_process(&fds[1], eblk);
			if (!ret && err)
				ret = -err;
		}
		else if (stat == CUTE_IODIR_FLUSH_STAT) {
			err = cute_iodir_flush(&fds[0], oblk);
			if (!ret && err)
				ret = -err;
			err = cute_iodir_flush(&fds[1], eblk);
			if (!ret && err)
				ret = -err;

			cute_iodir_wakeup(CUTE_IODIR_IDLE_STAT);
		}
		else if (stat == CUTE_IODIR_HALT_STAT) {
			break;
		}
		else {
			cute_assert_intern(0);
		}

		err = cute_iodir_handle_notify(&fds[2]);
		if (err) {
			if (!ret)
				ret = -err;
			break;
		}
	}

	cute_iodir_wakeup(CUTE_IODIR_HALT_STAT);

	pthread_exit(&ret);
}

static int
cute_iodir_open_pipe(FILE * stdio, int pipe_fds[2], int * orig)
{
	cute_assert_intern(stdio);
	cute_assert_intern(pipe_fds);
	cute_assert_intern(orig);

	int ret;

	ret = cute_pipe(pipe_fds);
	if (ret)
		return ret;

	ret = cute_setup_nonblock(pipe_fds[0]);
	if (ret)
		goto close_pipe;

	fflush(stdio);

	ret = fileno(stdio);
	cute_assert_intern(ret >= 0);

	ret = cute_dup(ret);
	if (ret < 0)
		goto close_pipe;

	*orig = ret;

	return 0;

close_pipe:
	close(pipe_fds[0]);
	close(pipe_fds[1]);

	return ret;
}

static void
cute_iodir_close_pipe(int pipe_fds[2], int orig)
{
	cute_assert_intern(pipe_fds[0] >= 0);
	cute_assert_intern(pipe_fds[1] >= 0);
	cute_assert_intern(orig >= 0);

	close(pipe_fds[0]);
	close(pipe_fds[1]);
	close(orig);
}

int
cute_iodir_init(void)
{
	int      err;
	sigset_t set;

	err = cute_iodir_open_pipe(stdout,
	                           cute_iodir_stdout_pipe,
	                           &cute_iodir_stdout_orig);
	if (err)
		return err;

	err = cute_iodir_open_pipe(stderr,
	                           cute_iodir_stderr_pipe,
	                           &cute_iodir_stderr_orig);
	if (err)
		goto close_out;

	if (cute_iodir_open_notify())
		goto close_err;

	err = cute_lock_init(&cute_iodir_lck);
	if (err)
		goto close_notif;

	err = cute_cond_init(&cute_iodir_cond);
	if (err)
		goto fini_lck;

	cute_iodir_stat = CUTE_IODIR_IDLE_STAT;
	cute_iodir_stdout_block = NULL;

	err = sigfillset(&set);
	cute_assert_intern(!err);
	err = cute_thr_create(&cute_iodir_thr, &set, cute_iodir_capture, NULL);
	if (err)
		goto fini_cond;

	return 0;

fini_cond:
	cute_cond_fini(&cute_iodir_cond);
fini_lck:
	cute_lock_fini(&cute_iodir_lck);
close_notif:
	cute_iodir_close_notify();
close_err:
	cute_iodir_close_pipe(cute_iodir_stderr_pipe, cute_iodir_stderr_orig);
close_out:
	cute_iodir_close_pipe(cute_iodir_stdout_pipe, cute_iodir_stdout_orig);

	return err;
}

int
cute_iodir_fini(void)
{
	cute_assert_intern(cute_iodir_stat == CUTE_IODIR_IDLE_STAT);
	cute_assert_intern(cute_iodir_stdout_orig >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stdout_pipe[1] >= 0);
	cute_assert_intern(!cute_iodir_stdout_block);
	cute_assert_intern(cute_iodir_stderr_orig >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[0] >= 0);
	cute_assert_intern(cute_iodir_stderr_pipe[1] >= 0);
	cute_assert_intern(!cute_iodir_stderr_block);

	int * ret;

	cute_iodir_wakeup(CUTE_IODIR_HALT_STAT);
	cute_thr_join(cute_iodir_thr, (void **)&ret);

	cute_cond_fini(&cute_iodir_cond);
	cute_lock_fini(&cute_iodir_lck);

	cute_iodir_close_notify();

	cute_iodir_close_pipe(cute_iodir_stderr_pipe, cute_iodir_stderr_orig);
	cute_iodir_close_pipe(cute_iodir_stdout_pipe, cute_iodir_stdout_orig);

	return *ret;
}
