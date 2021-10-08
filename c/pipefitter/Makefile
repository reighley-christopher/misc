pipefitter : pipefitter.c
	gcc pipefitter.c -o pipefitter --pedantic

faninout : faninout.c bytebuffer.c bytebuffer.h yajl_module.c
	gcc faninout.c bytebuffer.c yajl_module.c -lyajl -lpthread -o faninout --pedantic

bytebuffer_tests : bytebuffer.c bytebuffer_tests.c bytebuffer.h
	gcc bytebuffer_tests.c bytebuffer.c -o bytebuffer_tests --pedantic

test/test_exec : test/test_exec.c
	gcc test/test_exec.c -o test/test_exec --pedantic

install : pipefitter
	cp pipefitter /srv/run_master/bin
