#include "cons.h"
#include <errno.h>

int
cute_report_setup_xml(const struct cute_config * config __cute_unused)
{
	cute_error("XML support not implemented yet !\n");
	return -ENOSYS;
}
