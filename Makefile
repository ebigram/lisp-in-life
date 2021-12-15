LISP_OPT=./out/lisp_opt.qftasm

all:
	$(MAKE) $(LISP_OPT)

run:
	$(MAKE) run_gcc
	$(MAKE) run_qft

$(LISP_OPT):
	cd elvm && $(MAKE)
	./src/build_optlisp.sh

run_qft:
	./src/runlisp.sh $(LISP_OPT)

./lisp:
	gcc src/lisp.c -Isrc -o lisp

run_gcc: ./lisp
	./src/runlisp_gcc.sh
