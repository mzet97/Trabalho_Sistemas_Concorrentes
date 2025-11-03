CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
TARGET_ETAPA1 = etapa1
TARGET_ETAPA2 = etapa2
TARGET_SIGNAL = etapa_signal
TARGET_ETAPA3 = etapa3
SOURCE_ETAPA1 = etapa1.c
SOURCE_ETAPA2 = etapa2.c
SOURCE_SIGNAL = etapa_signal.c
SOURCE_ETAPA3 = etapa3.c

# Compilar Etapa 1
etapa1: $(SOURCE_ETAPA1)
	$(CC) $(SOURCE_ETAPA1) -o $(TARGET_ETAPA1) $(CFLAGS)

# Compilar Etapa 2
etapa2: $(SOURCE_ETAPA2)
	$(CC) $(SOURCE_ETAPA2) -o $(TARGET_ETAPA2) $(CFLAGS)

# Compilar Etapa Signal
etapa_signal: $(SOURCE_SIGNAL)
	$(CC) $(SOURCE_SIGNAL) -o $(TARGET_SIGNAL) $(CFLAGS)

# Compilar Etapa 3 (Semáforos)
etapa3: $(SOURCE_ETAPA3)
	$(CC) $(SOURCE_ETAPA3) -o $(TARGET_ETAPA3) $(CFLAGS)

# Executar Etapa 1
run1: etapa1
	./$(TARGET_ETAPA1)

# Executar Etapa 2
run2: etapa2
	./$(TARGET_ETAPA2)

# Executar Etapa Signal
run_signal: etapa_signal
	./$(TARGET_SIGNAL)

# Executar Etapa 3
run3: etapa3
	./$(TARGET_ETAPA3)

# Limpar arquivos compilados
clean:
	rm -f $(TARGET_ETAPA1) $(TARGET_ETAPA2) $(TARGET_SIGNAL) $(TARGET_ETAPA3)

# Compilar todas as etapas
all: etapa1 etapa2 etapa_signal etapa3

.PHONY: clean run1 run2 run_signal run3 all
.PHONY: run_signal etapa_signal
