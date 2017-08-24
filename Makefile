C_FILES := test.c my_printf.c
OBJECTS := $(C_FILES:.cxx=.o)

C_FLAGS := -std=c11 -Wall -O2
LINK_FLAGS := -O2

test: $(OBJECTS)
	@$(CC) $^ -o $@ $(LINK_FLAGS)

%.o: %.cxx
	@$(CC $(C_FLAGS) -c $< -o $@

clean:
	@rm -f test *.o
