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
