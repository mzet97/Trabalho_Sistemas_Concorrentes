#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#define NUM_FILHOS 3
#define NUM_ITERACOES 100
#define MIN_SLEEP_MS 5
#define MAX_SLEEP_MS 100
#define SHM_KEY 1234
#define SEM_KEY 5678

#define SEM_VAR1 0
#define SEM_VAR2 1

typedef struct {
    int primeira_variavel;  // Inicializada com 0
    int segunda_variavel;   // Inicializada com 300
} MemoriaCompartilhada;

// union semun não é definido pelo header, precisa declarar
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

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

int sem_p(int semid, int semnum) {
    struct sembuf op = { .sem_num = (unsigned short)semnum, .sem_op = -1, .sem_flg = 0 };
    return semop(semid, &op, 1);
}

int sem_v(int semid, int semnum) {
    struct sembuf op = { .sem_num = (unsigned short)semnum, .sem_op = +1, .sem_flg = 0 };
    return semop(semid, &op, 1);
}

int inicializa_semaforos(int semid) {
    union semun arg;
    arg.val = 1;
    if (semctl(semid, SEM_VAR1, SETVAL, arg) == -1) return -1;
    if (semctl(semid, SEM_VAR2, SETVAL, arg) == -1) return -1;
    return 0;
}

void processo_filho(int numero_filho, MemoriaCompartilhada *mem, int semid) {
    pid_t meu_pid = getpid();
    srand((unsigned int)(time(NULL) ^ meu_pid));

    double tempos_execucao[NUM_ITERACOES];
    double soma_tempos = 0.0;

    printf("Filho %d (PID: %d) iniciado\n", numero_filho, meu_pid);

    for (int i = 0; i < NUM_ITERACOES; i++) {
        long long inicio = get_time_ms();

        // Exibir informações incluindo variáveis compartilhadas (snapshot)
        printf("Filho %d (PID: %d) - Passo %d/%d - Var1: %d, Var2: %d\n",
               numero_filho, meu_pid, i + 1, NUM_ITERACOES,
               mem->primeira_variavel, mem->segunda_variavel);

        // Rotina com semáforos
        // 1..4: Lê, copia p/ local, incrementa, dorme
        if (sem_p(semid, SEM_VAR1) == -1) { perror("sem_p var1"); exit(1); }
        int variavel_local = mem->primeira_variavel;
        variavel_local++;
        int tempo_sleep1 = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (PID: %d) - Dormindo %d ms antes de escrever (Var1)\n",
               numero_filho, meu_pid, tempo_sleep1);
        sleep_ms(tempo_sleep1);
        // 5: Copia de volta p/ primeira variável
        mem->primeira_variavel = variavel_local;
        if (sem_v(semid, SEM_VAR1) == -1) { perror("sem_v var1"); exit(1); }

        // 6: Decrementa a segunda variável diretamente (protegido por semáforo próprio)
        if (sem_p(semid, SEM_VAR2) == -1) { perror("sem_p var2"); exit(1); }
        mem->segunda_variavel--;
        if (sem_v(semid, SEM_VAR2) == -1) { perror("sem_v var2"); exit(1); }

        // 7: Dorme mais um tempo aleatório
        int tempo_sleep2 = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (PID: %d) - Dormindo %d ms após operações\n",
               numero_filho, meu_pid, tempo_sleep2);
        sleep_ms(tempo_sleep2);

        long long fim = get_time_ms();
        double tempo_execucao = (double)(fim - inicio);
        tempos_execucao[i] = tempo_execucao;
        soma_tempos += tempo_execucao;
    }

    double tempo_medio = soma_tempos / NUM_ITERACOES;
    double desvio = calcular_desvio_padrao(tempos_execucao, NUM_ITERACOES, tempo_medio);
    printf("=== ESTATÍSTICAS DO FILHO %d (PID: %d) ===\n", numero_filho, meu_pid);
    printf("Tempo médio por iteração: %.2f ms\n", tempo_medio);
    printf("Desvio padrão: %.2f ms\n", desvio);
    printf("Valores finais parciais - Var1: %d, Var2: %d\n",
           mem->primeira_variavel, mem->segunda_variavel);
    printf("Filho %d (PID: %d) terminando...\n", numero_filho, meu_pid);
    fflush(stdout);

    exit(0);
}

int main(void) {
    pid_t pids_filhos[NUM_FILHOS];
    long long inicio_total = get_time_ms();
    int shmid, semid;
    MemoriaCompartilhada *mem;

    printf("=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA 3 (Semáforos) ===\n");
    printf("Processo pai (PID: %d) iniciando...\n", getpid());

    // Criar área de memória compartilhada
    shmid = shmget(SHM_KEY, sizeof(MemoriaCompartilhada), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget"); exit(1); }

    mem = (MemoriaCompartilhada *)shmat(shmid, NULL, 0);
    if (mem == (MemoriaCompartilhada *)-1) { perror("shmat"); exit(1); }

    // Inicializar variáveis compartilhadas
    mem->primeira_variavel = 0;
    mem->segunda_variavel = 300;
    printf("Memória compartilhada inicializada: Var1=%d, Var2=%d\n",
           mem->primeira_variavel, mem->segunda_variavel);

    // Criar array de semáforos (2: um para cada variável)
    semid = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget"); exit(1); }
    if (inicializa_semaforos(semid) == -1) { perror("semctl SETVAL"); exit(1); }
    printf("Semáforos criados e inicializados (semid=%d).\n", semid);

    // Criar processos filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            processo_filho(i, mem, semid);
        } else if (pid > 0) {
            pids_filhos[i] = pid;
            printf("[PAI] Criado filho %d com PID: %d\n", i, pid);
        } else {
            perror("fork");
            exit(1);
        }
    }

    printf("\n[PAI] Todos os filhos foram criados. Aguardando término...\n\n");

    // Aguardar término dos filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        int status;
        pid_t pid_terminado = wait(&status);
        int numero_filho = -1;
        for (int j = 0; j < NUM_FILHOS; j++) {
            if (pids_filhos[j] == pid_terminado) { numero_filho = j; break; }
        }
        printf("*** Filho %d, PID %d, terminou (status=%d) ***\n", numero_filho, pid_terminado, status);
    }

    long long fim_total = get_time_ms();
    long long duracao_total = fim_total - inicio_total;

    printf("\n=== PROCESSAMENTO CONCLUÍDO ===\n");
    printf("Todos os filhos terminaram. Processamento teve a duração de %lld ms\n", duracao_total);
    printf("\n=== VALORES FINAIS DAS VARIÁVEIS COMPARTILHADAS ===\n");
    printf("Primeira variável (incrementada pelos filhos): %d\n", mem->primeira_variavel);
    printf("Segunda variável (decrementada pelos filhos): %d\n", mem->segunda_variavel);

    // Liberar recursos
    if (shmdt(mem) == -1) perror("shmdt");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl IPC_RMID");
    if (semctl(semid, 0, IPC_RMID) == -1) perror("semctl IPC_RMID");
    printf("Recursos liberados (shm e semáforos).\n");

    return 0;
}

