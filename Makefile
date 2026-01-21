CC := gcc
CFLAGS := -Wall -Wextra -std=gnu11 -g -pthread
DEPFLAGS := -MMD -MP
CFLAGS += $(DEPFLAGS)
LDFLAGS := -pthread

SRCDIR := source
OBJDIR := objects
SRCS := $(addprefix $(SRCDIR)/,DMA.c interrupciones.c logger.c main.c memoriaPrincipal.c memoriaSecundaria.c procesador_aux.c procesador.c reloj.c cargadorDePrograma.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
TARGET := sosim

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@echo "Ejecutando $(TARGET)..."
	@./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)


debug: CFLAGS += -DDEBUG -g
debug: clean all


-include $(OBJS:.o=.d)
