CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
TARGET_ETAPA1 = etapa1
TARGET_ETAPA2 = etapa2
SOURCE_ETAPA1 = etapa1.c
SOURCE_ETAPA2 = etapa2.c

# Compilar Etapa 1
etapa1: $(SOURCE_ETAPA1)
	$(CC) $(SOURCE_ETAPA1) -o $(TARGET_ETAPA1) $(CFLAGS)

# Compilar Etapa 2
etapa2: $(SOURCE_ETAPA2)
	$(CC) $(SOURCE_ETAPA2) -o $(TARGET_ETAPA2) $(CFLAGS)

# Executar Etapa 1
run1: etapa1
	./$(TARGET_ETAPA1)

# Executar Etapa 2
run2: etapa2
	./$(TARGET_ETAPA2)

# Limpar arquivos compilados
clean:
	rm -f $(TARGET_ETAPA1) $(TARGET_ETAPA2)

# Compilar todas as etapas
all: etapa1 etapa2

.PHONY: clean run1 run2 all