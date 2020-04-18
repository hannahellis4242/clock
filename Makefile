all : makefile.avr makefile.tests
	make -f makefile.avr
	make -f makefile.tests

clean : makefile.avr makefile.tests
	make -f makefile.avr clean
	make -f makefile.tests clean

upload : makefile.avr
	make -f makefile.avr upload

size : makefile.avr
	make -f makefile.avr size

hex_size : makefile.avr
		make -f makefile.avr hex_size

test : makefile.tests
	make -f makefile.tests

run_tests : makefile.tests
	make -f makefile.tests run

test_clean : makefile.tests
	make -f makefile.tests clean
