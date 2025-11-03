#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define NUM_FILHOS 3
#define NUM_ITERACOES 100
#define MIN_SLEEP_MS 5
#define MAX_SLEEP_MS 100

static pid_t pids_filhos[NUM_FILHOS];
static volatile sig_atomic_t sinais_recebidos = 0;

long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000LL + (long long)tv.tv_usec / 1000LL;
}

void sleep_ms(int ms) {
    usleep(ms * 1000);
}

double calcular_desvio_padrao(double tempos[], int n, double media) {
    double soma = 0.0;
    for (int i = 0; i < n; i++) {
        double d = tempos[i] - media;
        soma += d * d;
    }
    return sqrt(soma / n);
}

static void handle_sigusr1(int signo, siginfo_t *info, void *ctx) {
    (void)signo; (void)ctx;
    pid_t remetente = info ? info->si_pid : -1;
    int numero_filho = -1;
    for (int i = 0; i < NUM_FILHOS; i++) {
        if (pids_filhos[i] == remetente) {
            numero_filho = i;
            break;
        }
    }
    sinais_recebidos++;
    // Aviso imediato do pai ao receber o sinal
    printf("[PAI] Recebi sinal de término do filho %d (PID: %d). Total sinais: %d\n",
           numero_filho, remetente, sinais_recebidos);
    fflush(stdout);
}

void rotina_filho(int numero_filho) {
    pid_t meu_pid = getpid();
    srand((unsigned int)(time(NULL) ^ meu_pid));

    double tempos_execucao[NUM_ITERACOES];
    double soma_tempos = 0.0;

    printf("Filho %d (PID: %d) iniciado\n", numero_filho, meu_pid);

    for (int i = 0; i < NUM_ITERACOES; i++) {
        long long inicio = get_time_ms();

        int tempo_sleep = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (PID: %d) - Rodada %d/%d - Dormindo %d ms\n",
               numero_filho, meu_pid, i + 1, NUM_ITERACOES, tempo_sleep);
        fflush(stdout);

        sleep_ms(tempo_sleep);

        long long fim = get_time_ms();
        double tempo_execucao = (double)(fim - inicio);
        tempos_execucao[i] = tempo_execucao;
        soma_tempos += tempo_execucao;
    }

    double media = soma_tempos / NUM_ITERACOES;
    double desvio = calcular_desvio_padrao(tempos_execucao, NUM_ITERACOES, media);

    printf("=== ESTATÍSTICAS DO FILHO %d (PID: %d) ===\n", numero_filho, meu_pid);
    printf("Tempo médio por iteração: %.2f ms\n", media);
    printf("Desvio padrão: %.2f ms\n", desvio);
    fflush(stdout);

    // Notifica o pai via signal
    pid_t pai = getppid();
    kill(pai, SIGUSR1);
    printf("Filho %d (PID: %d) enviou sinal de término ao pai (PID: %d).\n",
           numero_filho, meu_pid, pai);
    fflush(stdout);

    exit(0);
}

int main(void) {
    long long inicio_total = get_time_ms();

    // Instala handler de SIGUSR1 com informações do remetente
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sa.sa_sigaction = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Erro ao instalar handler de SIGUSR1");
        return 1;
    }

    printf("=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA SIGNAL ===\n");
    printf("Processo pai (PID: %d) iniciando...\n", getpid());
    fflush(stdout);

    // Cria processos filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            rotina_filho(i);
        } else if (pid > 0) {
            pids_filhos[i] = pid;
            printf("[PAI] Criado filho %d com PID: %d\n", i, pid);
            fflush(stdout);
        } else {
            perror("Erro ao criar processo filho");
            return 1;
        }
    }

    printf("\n[PAI] Todos os filhos foram criados. Aguardando término...\n\n");
    fflush(stdout);

    // Aguarda término de cada filho e reporta
    for (int i = 0; i < NUM_FILHOS; i++) {
        int status;
        pid_t pid_terminado = wait(&status);
        int numero_filho = -1;
        for (int j = 0; j < NUM_FILHOS; j++) {
            if (pids_filhos[j] == pid_terminado) {
                numero_filho = j;
                break;
            }
        }
        printf("Filho %d, PID %d, terminou (status=%d).\n", numero_filho, pid_terminado, status);
        fflush(stdout);
    }

    long long fim_total = get_time_ms();
    long long duracao_total = fim_total - inicio_total;
    printf("\n=== PROCESSAMENTO CONCLUÍDO ===\n");
    printf("Todos os filhos terminaram. Processamento teve a duração de %lld ms\n", duracao_total);
    printf("Sinais de término recebidos: %d\n", sinais_recebidos);
    fflush(stdout);

    return 0;
}

