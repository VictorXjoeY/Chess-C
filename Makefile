# Victor Forbes - 9293394

ifdef case
	in = < T$(T)TestCases/$(case).in
endif

all:
	gcc -o main src/*.c -I./includes
debug:
	gcc -o main src/*.c -I./includes -g -Wall
run:
	./main $(in)
fullrun:
	valgrind -v --track-origins=yes --leak-check=full ./main $(in)
compare:
	@./main $(in) > $(case).out
	@diff $(case).out TestCases/$(case).out
	@rm $(case).out