# NORMAL COMPILE
STANDARD = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -g3
# MEMORY COMPILE
MEMORY = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -fsanitize=undefined -fsanitize=address -g3
#OPTIMISE COMPILE
OPTIMISE = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -O3
LDLIBS =  -lm
CC = gcc
#CC = clang

all: stan mem opt extension_s extension_m extension_o

stan: bookcase.c neillsimplescreen.c
	$(CC) bookcase.c neillsimplescreen.c -o bookcase_s $(STANDARD) $(LDLIBS)

mem: bookcase.c neillsimplescreen.c
	$(CC) bookcase.c neillsimplescreen.c -o bookcase_m $(MEMORY) $(LDLIBS)

opt: bookcase.c neillsimplescreen.c
	$(CC) bookcase.c neillsimplescreen.c -o bookcase_o $(OPTIMISE) $(LDLIBS)

extension_s: extension.c neillsimplescreen.c
	$(CC) extension.c neillsimplescreen.c -o extension_s $(STANDARD) $(LDLIBS)

extension_m: extension.c neillsimplescreen.c
	$(CC) extension.c neillsimplescreen.c -o extension_m $(MEMORY) $(LDLIBS)

extension_o: extension.c neillsimplescreen.c
	$(CC) extension.c neillsimplescreen.c -o extension_o $(OPTIMISE) $(LDLIBS)



clean:
	rm -f bookcase_s bookcase_m bookcase_o extension_s extension_m extension_o
