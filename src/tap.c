#include "cons.h"
#include <errno.h>

int
cute_report_setup_tap(const struct cute_config * config __cute_unused)
{
	cute_error("TAP support not implemented yet !\n");
	return -ENOSYS;
}
