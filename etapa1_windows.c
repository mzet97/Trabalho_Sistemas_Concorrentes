#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <process.h>

#define NUM_FILHOS 3
#define NUM_ITERACOES 100
#define MIN_SLEEP_MS 5
#define MAX_SLEEP_MS 100

// Estrutura para passar dados para as threads
typedef struct {
    int numero_filho;
    DWORD thread_id;
} ThreadData;

// Função para obter tempo atual em milissegundos
long long get_time_ms() {
    SYSTEMTIME st;
    FILETIME ft;
    ULARGE_INTEGER ui;
    
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    
    return ui.QuadPart / 10000; // Converter de 100ns para ms
}

// Função para calcular desvio padrão
double calcular_desvio_padrao(double tempos[], int n, double media) {
    double soma = 0.0;
    for (int i = 0; i < n; i++) {
        soma += pow(tempos[i] - media, 2);
    }
    return sqrt(soma / n);
}

// Função executada pelas threads (simulando processos filhos)
unsigned __stdcall processo_filho(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int numero_filho = data->numero_filho;
    DWORD thread_id = data->thread_id;
    
    double tempos_execucao[NUM_ITERACOES];
    double soma_tempos = 0.0;
    
    // Inicializar gerador de números aleatórios com seed único para cada thread
    srand((unsigned int)(time(NULL) + thread_id));
    
    printf("Filho %d (Thread ID: %lu) iniciado\\n", numero_filho, thread_id);
    
    for (int i = 0; i < NUM_ITERACOES; i++) {
        long long inicio = get_time_ms();
        
        // Gerar tempo de sleep aleatório entre 5 e 100 ms
        int tempo_sleep = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        
        printf("Filho %d (Thread ID: %lu) - Passo %d/%d - Dormindo por %d ms\\n", 
               numero_filho, thread_id, i + 1, NUM_ITERACOES, tempo_sleep);
        
        Sleep(tempo_sleep); // Sleep do Windows
        
        long long fim = get_time_ms();
        double tempo_execucao = (double)(fim - inicio);
        tempos_execucao[i] = tempo_execucao;
        soma_tempos += tempo_execucao;
    }
    
    // Calcular estatísticas
    double tempo_medio = soma_tempos / NUM_ITERACOES;
    double desvio_padrao = calcular_desvio_padrao(tempos_execucao, NUM_ITERACOES, tempo_medio);
    
    printf("=== ESTATÍSTICAS DO FILHO %d (Thread ID: %lu) ===\\n", numero_filho, thread_id);
    printf("Tempo médio de execução por iteração: %.2f ms\\n", tempo_medio);
    printf("Desvio padrão: %.2f ms\\n", desvio_padrao);
    printf("Filho %d (Thread ID: %lu) terminando...\\n", numero_filho, thread_id);
    
    return 0;
}

int main() {
    HANDLE threads[NUM_FILHOS];
    ThreadData thread_data[NUM_FILHOS];
    long long inicio_total = get_time_ms();
    
    printf("=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA 1 (Windows) ===\\n");
    printf("Processo pai (Process ID: %lu) iniciando...\\n", GetCurrentProcessId());
    printf("Criando %d threads (simulando processos filhos)...\\n\\n", NUM_FILHOS);
    
    // Criar as threads (simulando processos filhos)
    for (int i = 0; i < NUM_FILHOS; i++) {
        thread_data[i].numero_filho = i;
        
        threads[i] = (HANDLE)_beginthreadex(
            NULL,                   // security attributes
            0,                      // stack size
            processo_filho,         // thread function
            &thread_data[i],        // argument
            0,                      // creation flags
            (unsigned*)&thread_data[i].thread_id  // thread id
        );
        
        if (threads[i] == NULL) {
            printf("Erro ao criar thread %d\\n", i);
            exit(1);
        }
        
        printf("Processo pai criou filho %d com Thread ID: %lu\\n", 
               i, thread_data[i].thread_id);
    }
    
    printf("\\nTodos os filhos foram criados. Aguardando término...\\n\\n");
    
    // Aguardar o término de todas as threads
    WaitForMultipleObjects(NUM_FILHOS, threads, TRUE, INFINITE);
    
    // Exibir mensagens de término
    for (int i = 0; i < NUM_FILHOS; i++) {
        printf("*** Filho %d, Thread ID %lu, terminou ***\\n", 
               i, thread_data[i].thread_id);
        CloseHandle(threads[i]);
    }
    
    long long fim_total = get_time_ms();
    long long duracao_total = fim_total - inicio_total;
    
    printf("\\n=== PROCESSAMENTO CONCLUÍDO ===\\n");
    printf("Todos os filhos terminaram. Processamento teve a duração de %lld ms\\n", duracao_total);
    
    return 0;
}