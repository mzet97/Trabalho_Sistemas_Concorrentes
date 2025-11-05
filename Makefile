CC := gcc
CFLAGS := -Wall -Wextra -std=c99
LDFLAGS := -lm

TARGETS := etapa1 etapa2 etapa_signal etapa3
SRCS := etapa1.c etapa2.c etapa_signal.c etapa3.c

.DEFAULT_GOAL := all

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Alvos (mantidos por clareza)
etapa1: etapa1.c
etapa2: etapa2.c
etapa_signal: etapa_signal.c
etapa3: etapa3.c

# Executar
run1: etapa1
	./etapa1

run2: etapa2
	./etapa2

run_signal: etapa_signal
	./etapa_signal

run3: etapa3
	./etapa3

# Limpar
clean:
	rm -f $(TARGETS)

.PHONY: clean run1 run2 run_signal run3 all
