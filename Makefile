C_FILES := $(wildcard *.c)
OBJECTS := $(C_FILES:.cxx=.o)

C_FLAGS := -std=c11 -Wall -O2
LINK_FLAGS := -O2

test: $(OBJECTS)
	@$(CC) $^ -o $@ $(LINK_FLAGS)

%.o: %.cxx
	@$(CC $(C_FLAGS) -c $< -o $@

clean:
	@rm -f test *.o
