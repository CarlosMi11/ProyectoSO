CC := gcc
CFLAGS := -Wall -Wextra -std=gnu11 -g -pthread
DEPFLAGS := -MMD -MP
CFLAGS += $(DEPFLAGS)
LDFLAGS := -pthread

SRCDIR := source
OBJDIR := objects
# Discover all .c sources in the source directory
SRCS := $(wildcard $(SRCDIR)/*.c)
# Place object files under $(OBJDIR) preserving filenames
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
TARGET := sosim

.PHONY: all clean run debug silence

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@echo "Ejecutando $(TARGET)..."
	@./$(TARGET)

silence:
	@$(MAKE) all CFLAGS="$(filter-out -Wall -Wextra,$(CFLAGS))"
	clear

clean:
	rm -rf $(OBJDIR) $(TARGET)


debug: CFLAGS += -DDEBUG -g
debug: clean all


-include $(OBJS:.o=.d)
