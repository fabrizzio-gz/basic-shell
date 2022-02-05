all:
	gcc -o shell -Wall src/main.c src/signals.c src/jobs.c src/wrappers.c
clean:
	rm shell
