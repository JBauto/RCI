CC 				= gcc
CFLAGS 			= -Wall -g
RMFLAGS			= -f
EXECUTABLES		= ddt testTejo
S_DIR 			= source
O_DIR 			= obj

all: ddt testTejo

# ddt
ddt: ddt.o interface.o network.o
	$(CC) $(CFLAGS) $(addprefix $(O_DIR)/,$^) -o $@

ddt.o: $(S_DIR)/ddt.c $(S_DIR)/interface.h $(S_DIR)/network.h
	$(CC) $(CFLAGS) -c $< -o $(O_DIR)/$@

interface.o: $(S_DIR)/interface.c
	$(CC) $(CFLAGS) -c $< -o $(O_DIR)/$@

network.o: $(S_DIR)/network.c
	$(CC) $(CFLAGS) -c $< -o $(O_DIR)/$@

#testTejo
testTejo: testTejo.o
	$(CC) $(CFLAGS) $(O_DIR)/$^ -o $@

testTejo.o: $(S_DIR)/testTejo.c
	$(CC) $(CFLAGS) -c $< -o $(O_DIR)/$@

#clean
clean:
	clear
	rm $(RMFLAGS) $(O_DIR)/*.o
	rm $(RMFLAGS) $(EXECUTABLES)


# $@ reffers to named before the :
# $^ reffers to all named after the :
# $< reffers to first after the :
# $(variable) : declared at the top of the file
