# Trabalho de Sistemas Concorrentes

Este repositório contém a implementação das etapas do trabalho de Sistemas Concorrentes.

## Estrutura do Projeto

```
Trabalho_Sistemas_Concorrentes/
├── etapa1.c                    # Implementação da Etapa 1 (Unix/Linux)
├── etapa1_windows.c            # Implementação da Etapa 1 (Windows)
├── etapa1_windows.exe          # Executável da Etapa 1 para Windows
├── etapa2.c                    # Implementação da Etapa 2 (Unix/Linux)
├── etapa2_windows.c            # Implementação da Etapa 2 (Windows)
├── etapa2_windows.exe          # Executável da Etapa 2 para Windows
├── compile_and_run.bat         # Script para compilar e executar Etapa 1 (Windows)
├── compile_and_run_etapa2.bat  # Script para compilar e executar Etapa 2 (Windows)
├── Makefile                    # Arquivo para compilação (Unix/Linux)
└── README.md                   # Este arquivo
```

## Etapa 1: Criação de Processos, Fork/Wait, Geração de Números Aleatórios

### Funcionalidades Implementadas

- **Processo Pai:**
  - Cria 3 processos filhos usando `fork()`
  - Armazena os PIDs dos filhos em um array
  - Aguarda o término de cada filho usando `wait()`
  - Exibe mensagens quando cada filho termina
  - Calcula e exibe o tempo total de processamento

- **Processos Filhos:**
  - Cada filho executa sua rotina 100 vezes
  - A cada iteração, exibe informações (número do filho, PID, passo atual)
  - Dorme por um tempo aleatório entre 5 e 100 ms
  - Calcula tempo médio de execução e desvio padrão
  - Exibe estatísticas antes de terminar

### Versões Disponíveis

#### Versão Unix/Linux (`etapa1.c`)
- Utiliza `fork()` para criar processos filhos reais
- Usa `wait()` para sincronização entre processos
- Requer sistema Unix/Linux para compilação e execução

#### Versão Windows (`etapa1_windows.c`)
- Utiliza threads para simular processos filhos
- Usa `WaitForMultipleObjects()` para sincronização
- Compatível com Windows e compiladores MinGW/GCC

### Compilação e Execução

#### No Windows (versão atual):
```bash
# Opção 1: Usar o script batch (mais fácil)
compile_and_run.bat

# Opção 2: Compilação manual
gcc etapa1_windows.c -o etapa1_windows.exe -lm
.\etapa1_windows.exe
```

#### No Unix/Linux (usando Makefile):
```bash
# Compilar a Etapa 1
make etapa1

# Compilar e executar a Etapa 1
make run1

# Limpar arquivos compilados
make clean
```

#### No Unix/Linux (compilação manual):
```bash
gcc etapa1.c -o etapa1 -lm
./etapa1
```

### Características Técnicas

- **Intercalamento Aleatório:** Cada processo filho usa um seed único (baseado no tempo + PID) para garantir tempos de sleep verdadeiramente aleatórios
- **Cronometragem Precisa:** Utiliza `gettimeofday()` para medições em milissegundos
- **Estatísticas:** Calcula tempo médio e desvio padrão das execuções
- **Sincronização:** O pai aguarda todos os filhos terminarem usando `wait()`

### Exemplo de Saída

```
=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA 1 ===
Processo pai (PID: 1234) iniciando...
Criando 3 processos filhos...

Processo pai criou filho 0 com PID: 1235
Processo pai criou filho 1 com PID: 1236
Processo pai criou filho 2 com PID: 1237

Todos os filhos foram criados. Aguardando término...

Filho 0 (PID: 1235) iniciado
Filho 1 (PID: 1236) iniciado
Filho 2 (PID: 1237) iniciado
Filho 0 (PID: 1235) - Passo 1/100 - Dormindo por 45 ms
...
```

## Etapa 2: Memória Compartilhada, Variáveis Compartilhadas

### Funcionalidades Implementadas

- **Processo Pai:**
  - Cria área de memória compartilhada (shmem no Unix/Linux)
  - Inicializa duas variáveis inteiras: primeira=0, segunda=300
  - Exibe valores finais das variáveis compartilhadas após término dos filhos
  - Libera recursos de memória compartilhada

- **Processos Filhos:**
  - Exibem valores das variáveis compartilhadas a cada iteração
  - Executam nova rotina a cada passo:
    1. Leem a primeira variável compartilhada
    2. Copiam para variável local e incrementam
    3. Dormem tempo aleatório
    4. Copiam de volta para a primeira variável
    5. Decrementam a segunda variável diretamente
    6. Dormem mais um tempo aleatório

### Compilação e Execução da Etapa 2

#### No Windows:
```bash
# Opção 1: Usar o script batch
compile_and_run_etapa2.bat

# Opção 2: Compilação manual
gcc etapa2_windows.c -o etapa2_windows.exe -lm
.\etapa2_windows.exe
```

#### No Unix/Linux:
```bash
# Usando Makefile
make etapa2
make run2

# Compilação manual
gcc etapa2.c -o etapa2 -lm
./etapa2
```

### Observações Importantes

- **Condições de Corrida**: O programa demonstra condições de corrida (race conditions) propositalmente, pois não há sincronização entre os processos
- **Valores Esperados**: Com 3 filhos × 100 iterações = 300 incrementos/decrementos, mas devido às condições de corrida, os valores finais podem variar
- **Memória Compartilhada**: Unix/Linux usa `shmget/shmat`, Windows simula com variáveis globais entre threads

## Próximas Etapas

As etapas 3, 4, 5 e 6 serão implementadas em arquivos separados conforme o progresso do trabalho.