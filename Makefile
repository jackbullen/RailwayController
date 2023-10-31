CC = gcc

CFLAGS = -Wall -Werror -g -std=c99 -pthread
LFLAGS = -pthread

TARGET = railway_simulator

OBJDIR = obj
SRCDIR = src
INCDIR = include

SRCS    := $(wildcard $(SRCDIR)/*.c)
INCS    := $(wildcard $(INCDIR)/*.h)
OBJS    := $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCS) | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR)/*.o $(TARGET)

.PHONY: clean