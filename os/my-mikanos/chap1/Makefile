all: hello.efi

hello.o: hello.c
	clang -target x86_64-pc-win32-coff -mno-red-zone -fno-stack-protector -fshort-wchar -Wall -c hello.c

hello.efi: hello.o
	lld-link /subsystem:efi_application /entry:EfiMain /out:hello.efi hello.o 