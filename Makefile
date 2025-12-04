CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
TARGET_ETAPA1 = etapa1
TARGET_ETAPA2 = etapa2
TARGET_SIGNAL = etapa_signal
TARGET_ETAPA3 = etapa3
TARGET_ETAPA4 = etapa4
TARGET_ETAPA4_FORCED = etapa4_forced
TARGET_ETAPA5 = etapa5
SOURCE_ETAPA1 = etapa1.c
SOURCE_ETAPA2 = etapa2.c
SOURCE_SIGNAL = etapa_signal.c
SOURCE_ETAPA3 = etapa3.c
SOURCE_ETAPA4 = etapa4.c
SOURCE_ETAPA4_FORCED = etapa4_forced.c
SOURCE_ETAPA5 = etapa5.c

TARGETS := etapa1 etapa2 etapa_signal etapa3
SRCS := etapa1.c etapa2.c etapa_signal.c etapa3.c

.DEFAULT_GOAL := all

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Compilar Etapa 4 (Starvation)
etapa4: $(SOURCE_ETAPA4)
	$(CC) $(SOURCE_ETAPA4) -o $(TARGET_ETAPA4) $(CFLAGS)

# Compilar Etapa 4 (Starvation Forçada)
etapa4_forced: $(SOURCE_ETAPA4_FORCED)
	$(CC) $(SOURCE_ETAPA4_FORCED) -o $(TARGET_ETAPA4_FORCED) $(CFLAGS)

# Compilar Etapa 5 (Solução Starvation)
etapa5: $(SOURCE_ETAPA5)
	$(CC) $(SOURCE_ETAPA5) -o $(TARGET_ETAPA5) $(CFLAGS)

# Executar Etapa 1
run1: etapa1
	./etapa1

run2: etapa2
	./etapa2

run_signal: etapa_signal
	./etapa_signal

run3: etapa3
	./etapa3

# Executar Etapa 4
run4: etapa4
	./$(TARGET_ETAPA4)

# Executar Etapa 4 Forçada
run4_forced: etapa4_forced
	./$(TARGET_ETAPA4_FORCED)

# Executar Etapa 5
run5: etapa5
	./$(TARGET_ETAPA5)

# Limpar arquivos compilados
clean:
	rm -f $(TARGET_ETAPA1) $(TARGET_ETAPA2) $(TARGET_SIGNAL) $(TARGET_ETAPA3) $(TARGET_ETAPA4) $(TARGET_ETAPA4_FORCED) $(TARGET_ETAPA5)

# Compilar todas as etapas
all: etapa1 etapa2 etapa_signal etapa3 etapa4 etapa4_forced etapa5

.PHONY: clean run1 run2 run_signal run3 run4 run4_forced run5 all
.PHONY: run_signal etapa_signal
