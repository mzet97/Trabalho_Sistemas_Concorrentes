#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#define NUM_FILHOS 3
#define NUM_ITERACOES 100
#define MIN_SLEEP_MS 5
#define MAX_SLEEP_MS 100

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
void processo_filho(int numero_filho) {
    pid_t meu_pid = getpid();
    double tempos_execucao[NUM_ITERACOES];
    double soma_tempos = 0.0;
    
    // Inicializar gerador de números aleatórios com seed único para cada filho
    srand(time(NULL) + meu_pid);
    
    printf("Filho %d (PID: %d) iniciado\n", numero_filho, meu_pid);
    
    for (int i = 0; i < NUM_ITERACOES; i++) {
        long long inicio = get_time_ms();
        
        // Gerar tempo de sleep aleatório entre 5 e 100 ms
        int tempo_sleep = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        
        printf("Filho %d (PID: %d) - Passo %d/%d - Dormindo por %d ms\n", 
               numero_filho, meu_pid, i + 1, NUM_ITERACOES, tempo_sleep);
        
        sleep_ms(tempo_sleep);
        
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
    printf("Filho %d (PID: %d) terminando...\n", numero_filho, meu_pid);
    
    exit(0);
}

int main() {
    pid_t pids_filhos[NUM_FILHOS];
    long long inicio_total = get_time_ms();
    
    printf("=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA 1 ===\n");
    printf("Processo pai (PID: %d) iniciando...\n", getpid());
    printf("Criando %d processos filhos...\n\n", NUM_FILHOS);
    
    // Criar os processos filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Código do processo filho
            processo_filho(i);
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
    
    return 0;
}