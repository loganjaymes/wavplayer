play:
	gcc -std=c99 -g -o play playwav.c -lasound
create:
	gcc -std=c99 -o create createwav.c -lm
