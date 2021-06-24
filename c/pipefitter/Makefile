pipefitter : pipefitter.c
	gcc pipefitter.c -o pipefitter --pedantic

test/test_exec : test/test_exec.c
	gcc test/test_exec.c -o test/test_exec --pedantic

install : pipefitter
	cp pipefitter /srv/run_master/bin
