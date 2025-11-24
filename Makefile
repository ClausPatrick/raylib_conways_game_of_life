# Compiler and Linker
CC = g++
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lm -ldl

# Raylib directories (adjust as necessary)
RAYLIB_INCLUDE = ./raylib/include
RAYLIB_LIB = ./raylib/lib

# Source, Object, and Log directories
SRCDIR = src
OBJDIR = obj
LOGDIR = logs

# Source files and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Executable and log files
EXECUTABLE = gameoflife
LOGFILES = $(wildcard $(LOGDIR)/*.txt)

# Include and library flags
INCLUDES = -I$(RAYLIB_INCLUDE)
LIBDIRS = -L$(RAYLIB_LIB)
LIBS = -lraylib

# Targets
.PHONY: all clean debug

# Default target
all: $(EXECUTABLE)

# Link the object files into the final executable
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) $(LDFLAGS) -o $(EXECUTABLE)

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Debug build with debugging information
debug: CFLAGS += -g -O0 -DDEBUG
debug: $(EXECUTABLE)

# Clean up object files, the executable, and log files
clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)
	rm -rf $(LOGFILES)

