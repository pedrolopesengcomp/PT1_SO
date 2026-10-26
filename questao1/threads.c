#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define TOTAL_CALCULOS 1000000000ULL

typedef struct {
    unsigned long long inicio;
    unsigned long long fim;
    long double resultado_parcial;
} Thread;

void* calcula_pi(void* arg){

    Thread* dados = (Thread*) arg;
    long double soma = 0;
    
    unsigned long long inicio = dados -> inicio;
    unsigned long long fim = dados -> fim;
    
    for (unsigned long long i = inicio; i<fim; i++){
        long double dividendo = (i % 2 == 0) ? 1.0L : -1.0L;
        long double divisor = (2.0L * i) + 1.0L;
        long double resultado = (dividendo / divisor); // Isso por conta do cálculo da fórmula dada 
        
        soma += resultado;
    }
    
    dados->resultado_parcial = soma;
    
    return NULL;
}

int main(){
    int threads[] = {1,2,4,8,16,32};
    
    for(int i = 0; i<6; i++){
        int num_threads = threads[i];
        clock_t inicio = clock();
        pthread_t threads[num_threads];
        Thread T[num_threads];
        
        int particionamento = TOTAL_CALCULOS / num_threads;
        for(int i = 0; i<num_threads; i++){
            int inicio = i * particionamento;
            int fim = (i == num_threads - 1) ? TOTAL_CALCULOS : (i + 1) * particionamento;
            T[i].inicio = inicio;
            T[i].fim = fim;
        
            pthread_create(&threads[i], NULL, calcula_pi, &T[i]);
        }
        
        long double resultado_final = 0.0L;
        
        for(int i = 0; i<num_threads; i++){
            pthread_join(threads[i], NULL);
            resultado_final += T[i].resultado_parcial;
        }
        
        resultado_final = 4.0L * resultado_final;
        
        printf("Valor de pi calculado como: %.15Lf\n", resultado_final);
        
        clock_t fim = clock();
        double tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
        FILE *arquivo = fopen("resultados.csv", "a");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo CSV!\n");
            return 1;
        }
        
        fprintf(arquivo, "Linux,%d,%.4f\n", num_threads, tempo_gasto);
        fclose(arquivo);
        
        printf("Tempo salvo com sucesso: %.4f segundos para o numero de threads %d\n", tempo_gasto, num_threads);
    }
    
    return 0;
}