# Makefile para proyecto1 (SO) - genera el ejecutable "so-sim"
# Uso: make          -> compila
#       make run      -> compila y ejecuta
#       make clean    -> elimina objetos y ejecutable
#       make debug    -> compila con símbolos de depuración

CC := gcc
CFLAGS := -Wall -Wextra -std=gnu11 -g -pthread
DEPFLAGS := -MMD -MP
CFLAGS += $(DEPFLAGS)
LDFLAGS := -pthread

SRCDIR := source
OBJDIR := objects
SRCS := $(addprefix $(SRCDIR)/,DMA.c interrupciones.c logger.c main.c memoriaPrincipal.c memoriaSecundaria.c procesador_aux.c procesador.c reloj.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
TARGET := so-sim

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

# Compilación para depuración (añade -DDEBUG y re-compila)
debug: CFLAGS += -DDEBUG -g
debug: clean all

# Incluir archivos de dependencias generados por -MMD
-include $(OBJS:.o=.d)
