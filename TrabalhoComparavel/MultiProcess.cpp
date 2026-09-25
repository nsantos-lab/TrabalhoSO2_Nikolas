#include <iostream>
#include <windows.h> // API nativa do Windows para processos
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
using namespace std;

#define NUM_PARTICLES 10000 // Número de partículas na simulação
#define TOTAL_STEPS 30 // Número de passos da simulação

// Dimensões do domínio da simulação
const double BOX_WIDTH = 800.0;
const double BOX_HEIGHT = 600.0;
const double GRAVITY = -9.81;
const double DT = 0.001; // Passo de tempo (Delta t)
const double DGravity = 50.0; // Distancia máxima para aplicar a gravidade entre partículas
const double FGravity = 50.0; // Força padrão da gravidade entre partículas

struct Particle {
    double x, y;   // Posição
    double Fvx, Fvy; // Velocidade final
    double Avx, Avy; // Velocidade que é atualizada dentro do updateForces
    double radius; // Raio da partícula
};

struct SharedState {
    int count;
    Particle particles[NUM_PARTICLES]; 
};

void initParticles(SharedState* shared) {
    srand(100);
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        Particle p; // cria a particula
        p.radius = 5.0; // Raio fixo para simplificação
        // Garante que a partícula comece totalmente dentro da caixa
        p.x = p.radius + (rand() % static_cast<int>(BOX_WIDTH  - 2 * p.radius + 1));
        p.y = p.radius + (rand() % static_cast<int>(BOX_HEIGHT - 2 * p.radius + 1));
        // Velocidades aleatórias entre -50 e 50
        p.Fvx = (rand() % 101) - 50;
        p.Avx = p.Fvx;
        p.Fvy = (rand() % 101) - 50;
        p.Avy = p.Fvy;
        shared->particles[i] = p;
    }
}

void updateForces(SharedState* shared, int min, int max) {
    int totalParticles = NUM_PARTICLES;
    for (int i = min; i <= max; i++) {
        // Aplica a força da gravidade na velocidade vertical
        shared->particles[i].Avy += GRAVITY * DT;

        // Trata colisão com as bordas verticais (Esquerda e Direita)
        if (shared->particles[i].x - shared->particles[i].radius < 0) {
            shared->particles[i].Avx = -shared->particles[i].Avx; // Inverte a velocidade horizontal
        } else if (shared->particles[i].x + shared->particles[i].radius > BOX_WIDTH) {
            shared->particles[i].x = BOX_WIDTH - shared->particles[i].radius; // Força a partícula para dentro da caixa
            shared->particles[i].Avx = -shared->particles[i].Avx; // Inverte a velocidade horizontal
        }

        // Trata colisão com as bordas horizontais (Teto e Chão)
        if (shared->particles[i].y - shared->particles[i].radius < 0) {
            shared->particles[i].Avy = -shared->particles[i].Avy; // Inverte a velocidade vertical
        } else if (shared->particles[i].y + shared->particles[i].radius > BOX_HEIGHT) {
            shared->particles[i].y = BOX_HEIGHT - shared->particles[i].radius; // Força a partícula para dentro da caixa
            shared->particles[i].Avy = -shared->particles[i].Avy; // Inverte a velocidade vertical
        }

        // Trata colisão entre partículas
        for (int j = 0; j < totalParticles; ++j) {
            if (j != i) { // Para não ser a mesma partícula
                double dx = shared->particles[j].x - shared->particles[i].x;
                double dy = shared->particles[j].y - shared->particles[i].y;
                double distance = sqrt(dx * dx + dy * dy);
                double minDistance = shared->particles[i].radius + shared->particles[j].radius;

                if (distance < minDistance) {
                    // De acordo com as regras de colisão elástica, se é trocada as velocidades das partículas
                    shared->particles[i].Avx = shared->particles[j].Fvx;
                    shared->particles[i].Avy = shared->particles[j].Fvy;                
                }                
            }
        }

        // Trata a gravidade entre particulas
        // Já que as partículas possuem a mesma massa, densidade e raio
        for (int j = 0; j < totalParticles; ++j) {
            if (j != i) { // Para não ser a mesma partícula
                double dx = shared->particles[j].x - shared->particles[i].x;
                double dy = shared->particles[j].y - shared->particles[i].y;
                double distance = sqrt(dx * dx + dy * dy);

                if (distance > 0.0 && distance < DGravity) {
                    // Aplica a força da gravidade entre as partículas
                    double force = FGravity / (distance * distance); // Força proporcional à inversa do quadrado da distância 
                    shared->particles[i].Avx += force * (dx / distance);
                    shared->particles[i].Avy += force * (dy / distance);
                }
            }
        }
    }
}

void updateVelocity(SharedState* shared, int min, int max) {
    for (int i = min; i <= max; i++) {
        shared->particles[i].Fvx = shared->particles[i].Avx;
        shared->particles[i].Fvy = shared->particles[i].Avy;
    }
}

void updatePosition(SharedState* shared, int min, int max) {
    for (int i = min; i <= max; ++i) {
        // Atualiza a posição com base na velocidade
        shared->particles[i].x += shared->particles[i].Fvx * DT;
        shared->particles[i].y += shared->particles[i].Fvy * DT;
    }
}

void printParticle(SharedState* shared, int N, int step) {
    cout << "Passo " << step << " | Particula 0 -> Pos: (" 
    << shared->particles[N].x << ", " << shared->particles[N].y << ") | Vel: (" 
    << shared->particles[N].Fvx << ", " << shared->particles[N].Fvy << ")\n";
}

int main(int argc, char* argv[]) {
    // 2. VERIFICAÇÃO SE É O PROCESSO FILHO
    // Se o programa foi aberto com o argumento "sou_filho", ele pula o código do pai
    if (argc > 2 && string(argv[1]) == "sou_filho") {
        //int id = stoi(argv[2]); // Pega o ID passado pelo pai
        //cout << "Processo filho " << id << " iniciado.\n";
        int min = stoi(argv[3]); // Pega o valor mínimo do range
        int max = stoi(argv[4]); // Pega o valor máximo do range
        int F = stoi(argv[5]); // Serve como verificacao

        HANDLE hMap = OpenFileMappingA(
            FILE_MAP_ALL_ACCESS, FALSE, "ParticlesSharedMemory");
        
        if (hMap == NULL) {
            cout << "OpenFileMapping falhou\n";
            return 1;
        }

        SharedState* shared = (SharedState*)MapViewOfFile(
            hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedState));
        
        if (shared == NULL) {
            cout << "MapViewOfFile falhou\n";
            return 1;
        }

        /*
        for (int i = 0; i < argc; ++i) {
            cout << "Arg[ " << i << "] " << argv[i] << endl;
        }
        */
        

        if(F==1){
            updateForces(shared, min, max);
        }else if(F==2){
            updateVelocity(shared, min, max);
        }else if(F==3){
            updatePosition(shared, min, max);
        }

        UnmapViewOfFile(shared);
        CloseHandle(hMap);
        return 0; // Finaliza o processo filho imediatamente
    }

    // 3. CÓDIGO DO PROCESSO PAI
    cout << "--- Processo PAI Iniciado ---\n";
    auto inicio = chrono::high_resolution_clock::now();

    // Cria a memória compartilhada para as partículas
    HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, 
            PAGE_READWRITE, 0, sizeof(SharedState), "ParticlesSharedMemory");
    
    if (hMap == NULL) {
        cout << "CreateFileMapping falhou\n";
        return 1;
    }

    // Mapeia a memória compartilhada para o processo pai
    SharedState* shared = (SharedState*)MapViewOfFile(
        hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedState));
    
    if (shared == NULL) {
        cout << "MapViewOfFile falhou\n";
        CloseHandle(hMap);
        return 1;
    }

    vector<PROCESS_INFORMATION> filhos;
    vector<STARTUPINFOA> infos;

    int NUM_PROCESSES;

    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <numero_de_processos>\n";
        return 1;
    }

    NUM_PROCESSES = stoi(argv[1]);

    if (NUM_PROCESSES < 2) {
        cerr << "O numero de processos deve ser pelo menos 2.\n";
        return 1;
    }

    int range = NUM_PARTICLES/NUM_PROCESSES;
    int rmod  = NUM_PARTICLES%NUM_PROCESSES;

    initParticles(shared);
    cout << "Particulas inicializadas com sucesso.\n";
    

    
    
    cout << "Iniciando simulacao em processos de " << NUM_PARTICLES << " particulas...\n";
    // Loop principal da simulação :D
    // O COMECO DO FIM    D:
    for (int step = 0; step < TOTAL_STEPS; ++step) {
        // Cria os processos filhos com seus ranges e os faz iniciar a função updateForces
        for (int i = 0; i < NUM_PROCESSES; ++i) {
            int min, max;
            if(i==0) { // significa que é o primeiro
                min = 0;
                max = range;      
            }else if(i==NUM_PROCESSES-1) { // significa que é o ultimo
                min = i*range + 1;
                max = (i+1)*range + rmod - 1;
            }else {
                min = i*range + 1;
                max = (i+1)*range;
            }

            STARTUPINFOA si = {};
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi = {};
            string comando = string(argv[0]) + " sou_filho " + to_string(i) 
            + " " + to_string(min) + " " + to_string(max) + " " + to_string(1); 
                                        // 1 para updateForces, 2 para updateVelocity, 3 para updatePosition

            if (!CreateProcessA(NULL, // É o caminho do executável, NULL significa que será usado argv[0]
                            &comando[0], // São os argumentos passados para o processo filho
                            NULL, // Segurançaf do processo, NULL significa que o processo filho não terá segurança especial
                            NULL, // Segurança da thread, NULL significa que a thread do processo filho não terá segurança especial
                            FALSE, // Se o processo filho herdará handles do processo pai, FALSE significa que não herdará
                            0, // Flags de criação do processo, 0 significa que não há flags especiais
                            NULL, // Ponteiro para o bloco de ambiente do processo filho, NULL significa que o processo filho herdará o ambiente do processo pai
                            NULL, // Ponteiro para o diretório de trabalho do processo filho, NULL significa que o processo filho herdará o diretório de trabalho do processo pai
                            &si, // Ponteiro para a estrutura STARTUPINFOA, que contém informações sobre como o processo filho deve ser iniciado
                            &pi //Ponteiro para a estrutura PROCESS_INFORMATION, que receberá informações sobre o processo filho criado     
                            )) {
                cerr << "Falha ao criar o Processo " << i << ". Erro: " << GetLastError() << "\n";
                return 1;
            }

            filhos.push_back(pi);
            infos.push_back(si);
            //cout << "Processo Filho " << i << " criado com sucesso.\n";
        }

        //cout << "Processo Pai criou os filhos e esta aguardando...\n";
        for (auto& pi : filhos) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess); // feacha o processo
            CloseHandle(pi.hThread);  // fecha a thread
        }
        filhos.clear();
        infos.clear();
        //cout << "--- Todos os processos filhos terminaram de calcular as forcas.\n";

        // Cria os processos filhos com seus ranges e os faz iniciar a função updateVelocity
        for (int i = 0; i < NUM_PROCESSES; ++i) {
            int min, max;
            if(i==0) { // significa que é o primeiro
                min = 0;
                max = range;      
            }else if(i==NUM_PROCESSES-1) { // significa que é o ultimo
                min = i*range + 1;
                max = (i+1)*range + rmod - 1;
            }else {
                min = i*range + 1;
                max = (i+1)*range;
            }

            STARTUPINFOA si = {};
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi = {};
            string comando = string(argv[0]) + " sou_filho " + to_string(i) 
            + " " + to_string(min) + " " + to_string(max) + " " + to_string(2); 
                                        // 1 para updateForces, 2 para updateVelocity, 3 para updatePosition

            if (!CreateProcessA(NULL, &comando[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                cerr << "Falha ao criar o Processo " << i << ". Erro: " << GetLastError() << "\n";
                return 1;
            }

            filhos.push_back(pi);
            infos.push_back(si);
            //cout << "Processo Filho " << i << " criado com sucesso.\n";
        }

        //cout << "Processo Pai criou os filhos e esta aguardando...\n";
        for (auto& pi : filhos) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess); // feacha o processo
            CloseHandle(pi.hThread);  // fecha a thread
        }
        filhos.clear();
        infos.clear();
        //cout << "--- Todos os processos filhos terminaram de calcular as forcas.\n";
            
        // Cria os processos filhos com seus ranges e os faz iniciar a função updatePosition
        for (int i = 0; i < NUM_PROCESSES; ++i) {
            int min, max;
            if(i==0) { // significa que é o primeiro
                min = 0;
                max = range;      
            }else if(i==NUM_PROCESSES-1) { // significa que é o ultimo
                min = i*range + 1;
                max = (i+1)*range + rmod - 1;
            }else {
                min = i*range + 1;
                max = (i+1)*range;
            }

            STARTUPINFOA si = {};
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi = {};
            string comando = string(argv[0]) + " sou_filho " + to_string(i) 
            + " " + to_string(min) + " " + to_string(max) + " " + to_string(3); 
                                        // 1 para updateForces, 2 para updateVelocity, 3 para updatePosition

            if (!CreateProcessA(NULL, &comando[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                cerr << "Falha ao criar o Processo " << i << ". Erro: " << GetLastError() << "\n";
                return 1;
            }

            filhos.push_back(pi);
            infos.push_back(si);

            //cout << "Processo Filho " << i << " criado com sucesso.\n";
        }

        //cout << "Processo Pai criou os filhos e esta aguardando...\n";
        for (auto& pi : filhos) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess); // feacha o processo
            CloseHandle(pi.hThread);  // fecha a thread
        }
        filhos.clear();
        infos.clear();
        //cout << "--- Todos os processos filhos terminaram de atualizar a posicoes.\n";

        if ((step % 5 == 0)) {
            // Imprime o estado de uma particula
            printParticle(shared, 0, step);
        }
    }
        
    UnmapViewOfFile(shared);
    CloseHandle(hMap);
    cout << "--- Memoria compartilhada liberada.\n";

    cout << "--- Processo PAI finalizado! ---\n";
    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duracao = fim - inicio;
    cout << "O processo demorou: " << duracao.count() << " ms" << endl;
    return 0;
}