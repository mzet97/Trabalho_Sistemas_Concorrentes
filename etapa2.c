#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#define NUM_FILHOS 3
#define NUM_ITERACOES 100
#define MIN_SLEEP_MS 5
#define MAX_SLEEP_MS 100
#define SHM_KEY 1234

// Estrutura para a memória compartilhada
typedef struct {
    int primeira_variavel;  // Inicializada com 0
    int segunda_variavel;   // Inicializada com 300
} MemoriaCompartilhada;

// Função para obter tempo atual em milissegundos
long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (long long)(tv.tv_usec) / 1000;
}

// Função para dormir por um número específico de milissegundos
void sleep_ms(int ms) {
    usleep(ms * 1000);
}

// Função para calcular desvio padrão
double calcular_desvio_padrao(double tempos[], int n, double media) {
    double soma = 0.0;
    for (int i = 0; i < n; i++) {
        soma += pow(tempos[i] - media, 2);
    }
    return sqrt(soma / n);
}

// Função executada pelos processos filhos
void processo_filho(int numero_filho, MemoriaCompartilhada *mem_compartilhada) {
    pid_t meu_pid = getpid();
    double tempos_execucao[NUM_ITERACOES];
    double soma_tempos = 0.0;
    
    // Inicializar gerador de números aleatórios com seed único para cada filho
    srand(time(NULL) + meu_pid);
    
    printf("Filho %d (PID: %d) iniciado\n", numero_filho, meu_pid);
    
    for (int i = 0; i < NUM_ITERACOES; i++) {
        long long inicio = get_time_ms();
        
        // Exibir informações incluindo variáveis compartilhadas
        printf("Filho %d (PID: %d) - Passo %d/%d - Var1: %d, Var2: %d\n", 
               numero_filho, meu_pid, i + 1, NUM_ITERACOES, 
               mem_compartilhada->primeira_variavel, mem_compartilhada->segunda_variavel);
        
        // Nova rotina da Etapa 2:
        // 1. Lê a primeira variável compartilhada
        int variavel_local = mem_compartilhada->primeira_variavel;
        
        // 2. Incrementa a variável local
        variavel_local++;
        
        // 3. Dorme um tempo aleatório
        int tempo_sleep1 = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (PID: %d) - Dormindo %d ms antes de escrever\n", 
               numero_filho, meu_pid, tempo_sleep1);
        sleep_ms(tempo_sleep1);
        
        // 4. Copia "de volta" para a primeira variável compartilhada
        mem_compartilhada->primeira_variavel = variavel_local;
        
        // 5. Decrementa a segunda variável compartilhada (diretamente)
        mem_compartilhada->segunda_variavel--;
        
        // 6. Dorme mais um tempo aleatório
        int tempo_sleep2 = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (PID: %d) - Dormindo %d ms após operações\n", 
               numero_filho, meu_pid, tempo_sleep2);
        sleep_ms(tempo_sleep2);
        
        long long fim = get_time_ms();
        double tempo_execucao = (double)(fim - inicio);
        tempos_execucao[i] = tempo_execucao;
        soma_tempos += tempo_execucao;
    }
    
    // Calcular estatísticas
    double tempo_medio = soma_tempos / NUM_ITERACOES;
    double desvio_padrao = calcular_desvio_padrao(tempos_execucao, NUM_ITERACOES, tempo_medio);
    
    printf("=== ESTATÍSTICAS DO FILHO %d (PID: %d) ===\n", numero_filho, meu_pid);
    printf("Tempo médio de execução por iteração: %.2f ms\n", tempo_medio);
    printf("Desvio padrão: %.2f ms\n", desvio_padrao);
    printf("Valores finais das variáveis compartilhadas - Var1: %d, Var2: %d\n", 
           mem_compartilhada->primeira_variavel, mem_compartilhada->segunda_variavel);
    printf("Filho %d (PID: %d) terminando...\n", numero_filho, meu_pid);
    
    exit(0);
}

int main() {
    pid_t pids_filhos[NUM_FILHOS];
    long long inicio_total = get_time_ms();
    int shmid;
    MemoriaCompartilhada *mem_compartilhada;
    
    printf("=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA 2 ===\n");
    printf("Processo pai (PID: %d) iniciando...\n", getpid());
    
    // Criar área de memória compartilhada
    shmid = shmget(SHM_KEY, sizeof(MemoriaCompartilhada), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Erro ao criar memória compartilhada");
        exit(1);
    }
    
    // Anexar a memória compartilhada ao processo
    mem_compartilhada = (MemoriaCompartilhada *)shmat(shmid, NULL, 0);
    if (mem_compartilhada == (MemoriaCompartilhada *)-1) {
        perror("Erro ao anexar memória compartilhada");
        exit(1);
    }
    
    // Inicializar variáveis compartilhadas
    mem_compartilhada->primeira_variavel = 0;
    mem_compartilhada->segunda_variavel = 300;
    
    printf("Memória compartilhada criada e inicializada:\n");
    printf("Primeira variável: %d\n", mem_compartilhada->primeira_variavel);
    printf("Segunda variável: %d\n", mem_compartilhada->segunda_variavel);
    printf("Criando %d processos filhos...\n\n", NUM_FILHOS);
    
    // Criar os processos filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Código do processo filho
            processo_filho(i, mem_compartilhada);
        } else if (pid > 0) {
            // Código do processo pai
            pids_filhos[i] = pid;
            printf("Processo pai criou filho %d com PID: %d\n", i, pid);
        } else {
            // Erro no fork
            perror("Erro ao criar processo filho");
            exit(1);
        }
    }
    
    printf("\nTodos os filhos foram criados. Aguardando término...\n\n");
    
    // Aguardar o término de cada filho
    for (int i = 0; i < NUM_FILHOS; i++) {
        int status;
        pid_t pid_terminado = wait(&status);
        
        // Encontrar qual filho terminou
        int numero_filho = -1;
        for (int j = 0; j < NUM_FILHOS; j++) {
            if (pids_filhos[j] == pid_terminado) {
                numero_filho = j;
                break;
            }
        }
        
        printf("*** Filho %d, PID %d, terminou ***\n", numero_filho, pid_terminado);
    }
    
    long long fim_total = get_time_ms();
    long long duracao_total = fim_total - inicio_total;
    
    printf("\n=== PROCESSAMENTO CONCLUÍDO ===\n");
    printf("Todos os filhos terminaram. Processamento teve a duração de %lld ms\n", duracao_total);
    printf("\n=== VALORES FINAIS DAS VARIÁVEIS COMPARTILHADAS ===\n");
    printf("Primeira variável (incrementada pelos filhos): %d\n", mem_compartilhada->primeira_variavel);
    printf("Segunda variável (decrementada pelos filhos): %d\n", mem_compartilhada->segunda_variavel);
    
    // Liberar recursos de memória compartilhada
    if (shmdt(mem_compartilhada) == -1) {
        perror("Erro ao desanexar memória compartilhada");
    }
    
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erro ao remover memória compartilhada");
    }
    
    printf("Recursos de memória compartilhada liberados.\n");
    
    return 0;
}