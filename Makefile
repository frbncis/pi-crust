build:
	mkdir -p out
	gcc -L/usr/X11/lib -lX11 -o out/kvm.out xreadkeys.c 

.PHONY: clean
