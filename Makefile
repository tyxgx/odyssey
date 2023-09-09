CC = clang++

CFLAGS = -g -Wall -Wextra -std=c++20 -Wpedantic -Wuninitialized -Wmissing-include-dirs -Wshadow -O2 -Wundef -fsanitize=address

TARGET = ody

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET).out src/*.cpp

clean:
	$(RM) -f .DS_Store
	$(RM) -rf *.dSYM/ 
veryclean:
	$(RM) -f *.out
	$(RM) -f .DS_Store
	$(RM) -rf *.dSYM/ 
	
