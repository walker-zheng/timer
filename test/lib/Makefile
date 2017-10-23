UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
TARGET = libhello.so
else
TARGET = hello.dll
endif

$(TARGET): hello.o
	gcc  -o $@  $^  -shared
# '-o $@'    => output file => $@ = libhello.so or hello.dll
# '   $^'    => no options => Link all depended files => $^ = hello.o
# '-shared'  => generate shared library

%.o: %.c
	gcc  -o $@  -c $<  -fPIC
# '-o $@' => output file => $@ = the target file (main.o)
# '-c $<' => compile the first depended file (main.cpp)
# '-fPIC' => Position-Independent Code (required for shared lib)

clean:
	rm -f *.o *.so *.dll *.exe

