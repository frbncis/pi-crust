build:
	mkdir -p out
	gcc -o out/kvm.out xreadkeys.c scancodes.c -DXK_MISCELLANY=1 -DXK_LATIN1=1 -L/usr/include/ -lX11

.PHONY: clean
