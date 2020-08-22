build:
	mkdir -p out
	gcc -DXK_MISCELLANY=1 -DXK_LATIN1=1 -L/usr/X11/lib -lX11 -o out/kvm.out xreadkeys.c scancodes.c

.PHONY: clean
