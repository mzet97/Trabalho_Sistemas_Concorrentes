# Trabalho de Sistemas Concorrentes

Este repositório contém a implementação das etapas do trabalho de Sistemas Concorrentes.

## Requisitos
- Ambiente Unix/Linux ou WSL 2 em Windows
- `gcc` e `make` instalados
- Terminal com suporte a ferramentas POSIX

## Estrutura do Projeto

```
Trabalho_Sistemas_Concorrentes/
├── Makefile            # Compilação (Unix/Linux)
├── etapa1.c            # Etapa 1 (Unix/Linux)
├── etapa2.c            # Etapa 2 (Unix/Linux)
├── etapa3.c            # Etapa 3 — Semáforos (Unix/Linux)
├── etapa_signal.c      # Versão com sinais (Unix/Linux)
├── s1.md               # Simulações Dijkstra/Peterson (manual)
└── s2.md               # Produtor-Consumidor: semáforos e Redes de Petri
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

 

### Compilação e Execução

#### No Unix/Linux (usando Makefile):
```bash
# Compilar a Etapa 1
make etapa1

# Compilar e executar a Etapa 1
make run1

# Limpar arquivos compilados
make clean
```

#### Compilação manual:
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
- **Memória Compartilhada**: Usa `shmget/shmat` em Unix/Linux

## Etapas Adicionais

- **Etapa 3 (Semáforos)**: `etapa3.c` usa `semget/semop/semctl` para proteger atualizações de variáveis compartilhadas e garantir consistência.
- **Sinais (etapa_signal)**: `etapa_signal.c` implementa comunicação pai-filho por sinais (`SIGUSR1`) e mede tempos/estatísticas.
- **S1 (Dijkstra/Peterson)**: `s1.md` contém simulações manuais das soluções clássicas de exclusão mútua.
- **S2 (Produtor-Consumidor)**: `s2.md` apresenta cenários manuais, Redes de Petri e roteiro de simulação no JSARP.

## Etapa 3 — Semáforos (System V)
- Objetivo: garantir consistência das variáveis compartilhadas usando semáforos (`semget/semop/semctl`).
- Recursos: memória compartilhada (`shmget/shmat`) e array de semáforos (duas posições para operações distintas).
- Comandos:
  - `make etapa3` para compilar
  - `make run3` para executar
- Resultado esperado: valores finais estáveis (ex.: `Primeira variável=300`, `Segunda variável=0`).

## Etapa Signal — Comunicação por Sinais
- Objetivo: comunicação pai–filho via `SIGUSR1` com `sigaction` (`SA_SIGINFO`) e contagem de sinais recebidos.
- Comandos:
  - `make etapa_signal` para compilar
  - `make run_signal` para executar
- Observações: handler identifica PID do remetente; o pai aguarda filhos com `wait()` e contabiliza sinais.

## S1 — Dijkstra e Peterson
- Arquivo: `s1.md`
- Conteúdo:
  - Simulações manuais dos protocolos (cenários que funcionam e falham).
  - Diagramas Mermaid de fluxo e sequência para Dijkstra (semáforo) e Peterson.
- Visualização:
  - Use um preview Markdown com suporte a Mermaid ou exporte os blocos para SVG/PNG.

## S2 — Produtor-Consumidor (Semáforos e Redes de Petri)
- Arquivo: `s2.md`
- Conteúdo:
  - Simulação manual sem semáforo e com sincronização sem exclusão mútua.
  - Solução completa [`item`, `vaga`, `mutex`] com grafo de lugares/transições e árvore de alcançabilidade (N=2, 1P/1C).
  - Diagramas Mermaid (fluxos e sequência) e roteiro de uso no JSARP para comprovação.
- JSARP:
  - Modele lugares `Itm`, `Vag`, `Mtx`, `P_out`, `P_in`, `C_out`, `C_in` e transições `Put`, `Put_done`, `Get`, `Get_done`.
  - Valide `Itm+Vag=N` e exclusão mútua via `Mtx`.

## Comandos Rápidos (Makefile)
- `make all`: compila todas as etapas (`etapa1`, `etapa2`, `etapa_signal`, `etapa3`)
- `make run1`: executa a Etapa 1
- `make run2`: executa a Etapa 2
- `make run_signal`: executa `etapa_signal`
- `make run3`: executa a Etapa 3 (semáforos)
- `make clean`: remove os binários gerados

## Visualização de Diagramas (Mermaid)
- `s1.md` e `s2.md` incluem blocos Mermaid para fluxogramas e diagramas de chamadas
- Para visualizar:
  - Use um editor com suporte a Mermaid (extensões de preview Markdown)
  - Se não houver renderização nativa, exporte para SVG/PNG ou use ferramentas online de Mermaid

## JSARP (Simulações de Redes de Petri)
- Em `s2.md` há o roteiro de modelagem e validação no JSARP
- Crie lugares (`Itm`, `Vag`, `Mtx`, `P_out`, `P_in`, `C_out`, `C_in`) e transições (`Put`, `Put_done`, `Get`, `Get_done`)
- Configure marcações iniciais (ex.: `N=2`: `Vag=2`, `Itm=0`, `Mtx=1`, `P_out=1`, `C_out=1`)
- Dispare transições passo a passo para reproduzir a árvore de alcançabilidade manual
- Validações principais:
  - `Itm + Vag = N` sempre
  - Exclusão mútua com `Mtx` (nunca `P_in=1` e `C_in=1` simultâneos)
