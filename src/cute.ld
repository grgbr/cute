SECTIONS
{
	cute_tests :
	{
		__start_cute_tests = .;
		KEEP(*(cute_tests))     /* SORT_NONE may be needed ? */
		__stop_cute_tests = .;
	}

	cute_suites :
	{
		__start_cute_suites = .;
		KEEP(*(cute_suites))     /* SORT_NONE may be needed ? */
		__stop_cute_suites = .;
	}
}
INSERT AFTER .data;
