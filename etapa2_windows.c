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

// Estrutura para a memória compartilhada (simulada com variáveis globais)
typedef struct {
    int primeira_variavel;  // Inicializada com 0
    int segunda_variavel;   // Inicializada com 300
} MemoriaCompartilhada;

// Variáveis globais simulando memória compartilhada
MemoriaCompartilhada memoria_compartilhada;

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
        
        // Exibir informações incluindo variáveis compartilhadas
        printf("Filho %d (Thread ID: %lu) - Passo %d/%d - Var1: %d, Var2: %d\\n", 
               numero_filho, thread_id, i + 1, NUM_ITERACOES, 
               memoria_compartilhada.primeira_variavel, memoria_compartilhada.segunda_variavel);
        
        // Nova rotina da Etapa 2:
        // 1. Lê a primeira variável compartilhada
        int variavel_local = memoria_compartilhada.primeira_variavel;
        
        // 2. Incrementa a variável local
        variavel_local++;
        
        // 3. Dorme um tempo aleatório
        int tempo_sleep1 = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (Thread ID: %lu) - Dormindo %d ms antes de escrever\\n", 
               numero_filho, thread_id, tempo_sleep1);
        Sleep(tempo_sleep1);
        
        // 4. Copia "de volta" para a primeira variável compartilhada
        memoria_compartilhada.primeira_variavel = variavel_local;
        
        // 5. Decrementa a segunda variável compartilhada (diretamente)
        memoria_compartilhada.segunda_variavel--;
        
        // 6. Dorme mais um tempo aleatório
        int tempo_sleep2 = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        printf("Filho %d (Thread ID: %lu) - Dormindo %d ms após operações\\n", 
               numero_filho, thread_id, tempo_sleep2);
        Sleep(tempo_sleep2);
        
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
    printf("Valores finais das variáveis compartilhadas - Var1: %d, Var2: %d\\n", 
           memoria_compartilhada.primeira_variavel, memoria_compartilhada.segunda_variavel);
    printf("Filho %d (Thread ID: %lu) terminando...\\n", numero_filho, thread_id);
    
    return 0;
}

int main() {
    HANDLE threads[NUM_FILHOS];
    ThreadData thread_data[NUM_FILHOS];
    long long inicio_total = get_time_ms();
    
    printf("=== PROGRAMA DE SISTEMAS CONCORRENTES - ETAPA 2 (Windows) ===\\n");
    printf("Processo pai (Process ID: %lu) iniciando...\\n", GetCurrentProcessId());
    
    // Inicializar variáveis compartilhadas
    memoria_compartilhada.primeira_variavel = 0;
    memoria_compartilhada.segunda_variavel = 300;
    
    printf("Memória compartilhada (simulada) inicializada:\\n");
    printf("Primeira variável: %d\\n", memoria_compartilhada.primeira_variavel);
    printf("Segunda variável: %d\\n", memoria_compartilhada.segunda_variavel);
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
    printf("\\n=== VALORES FINAIS DAS VARIÁVEIS COMPARTILHADAS ===\\n");
    printf("Primeira variável (incrementada pelos filhos): %d\\n", memoria_compartilhada.primeira_variavel);
    printf("Segunda variável (decrementada pelos filhos): %d\\n", memoria_compartilhada.segunda_variavel);
    
    printf("\\nNOTA: Em ambiente Windows, as variáveis são compartilhadas entre threads.\\n");
    printf("Em sistemas Unix/Linux, seria usada memória compartilhada real (shmget/shmat).\\n");
    
    return 0;
}