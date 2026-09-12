#include <stdio.h>
#include <thread>

// Variáveis globais são compartilhadas pelas threads
#define NTHREADS 40
int qtd = 0;
int v[1000];
int tam_intervalo = 1000/NTHREADS;


// Só permite no máximo UM parâmetro
// Caso seja necessário passar mais de um valor:
//       Se forem do mesmo tipo: Vetor
//       Se não forem do mesmo tipo: Struct
void *contabilizaPares(void *arg){
    int ini = (int)(long)arg;
    printf("INICIO: %d\n", ini);
    int inicio = ini*tam_intervalo;
    int final = (ini+1)*tam_intervalo;
    for(int i = inicio; i < final; i++){
        if(v[i] % 2 == 0){
            qtd++;
        }
    }
}
// Variáveis locais são exclusivas da thread
int main(){ // 1 processo -> 1 thread
    pthread_t id[NTHREADS];
    int inicio = 0;
    
    for(int i = 0; i < 1000; i++)
        v[i] = i;
    
    for(int i = 0; i < NTHREADS; i++)
        pthread_create(&id[i], NULL, contabilizaPares, (void *)(long)i);

    for(int i = 0; i < NTHREADS; i++)
        pthread_join(id[i], NULL); // wait

    printf("Total de pares: %d\n", qtd);

}