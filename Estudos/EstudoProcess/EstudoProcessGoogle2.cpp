#include <iostream>
#include <windows.h> // API nativa do Windows para processos
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#define NUM_PROCESSES 4 // Número de processos a serem usados na simulação
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
    double vx, vy; // Velocidade
    double radius; // Raio da partícula
};

struct SharedState {
    int count;
    Particle particles[NUM_PARTICLES]; 
};

void initParticles(SharedState* shared, int min, int max) {
    srand(time(0));
    for (int i = min; i <= max; ++i) {
        Particle p; // cria a particula
        p.radius = 5.0; // Raio fixo para simplificação
        // Garante que a partícula comece totalmente dentro da caixa
        p.x = p.radius + (rand() % static_cast<int>(BOX_WIDTH  - 2 * p.radius + 1));
        p.y = p.radius + (rand() % static_cast<int>(BOX_HEIGHT - 2 * p.radius + 1));
        // Velocidades aleatórias entre -50 e 50
        p.vx = (rand() % 101) - 50;
        p.vy = (rand() % 101) - 50;
        shared->particles[i] = p;
    }
}

void updateForces(SharedState* shared, int min, int max) {
    for (int i = min; i <= max; ++i) {
        // Aplica a força da gravidade na velocidade vertical
        shared->particles[i].vy += GRAVITY * DT;

        // Trata colisão com as bordas verticais (Esquerda e Direita)
        if (shared->particles[i].x - shared->particles[i].radius < 0) {
            shared->particles[i].vx = -shared->particles[i].vx; // Inverte a velocidade horizontal
        } else if (shared->particles[i].x + shared->particles[i].radius > BOX_WIDTH) {
            shared->particles[i].x = BOX_WIDTH - shared->particles[i].radius; // Força a partícula para dentro da caixa
            shared->particles[i].vx = -shared->particles[i].vx; // Inverte a velocidade horizontal
        }

        // Trata colisão com as bordas horizontais (Cima e Baixo)
        if (shared->particles[i].y - shared->particles[i].radius < 0) {
            shared->particles[i].vy = -shared->particles[i].vy; // Inverte a velocidade vertical
        } else if (shared->particles[i].y + shared->particles[i].radius > BOX_HEIGHT) {
            shared->particles[i].y = BOX_HEIGHT - shared->particles[i].radius; // Força a partícula para dentro da caixa
            shared->particles[i].vy = -shared->particles[i].vy; // Inverte a velocidade vertical
        }

        // Trata colisão entre partículas
        for (int j = 0; j < NUM_PARTICLES; ++j) {
            if (j != i) { // Para não ser a mesma partícula
                double dx = shared->particles[j].x - shared->particles[i].x;
                double dy = shared->particles[j].y - shared->particles[i].y;
                double distance = sqrt(dx * dx + dy * dy);
                double minDistance = shared->particles[i].radius + shared->particles[j].radius;

                if (distance < minDistance) {
                    // De acordo com as regras de colisão elástica, se é trocada as velocidades das partículas
                    double troca = shared->particles[i].vx;
                    shared->particles[i].vx = shared->particles[j].vx;
                    shared->particles[j].vx = troca;
                    troca = shared->particles[i].vy;
                    shared->particles[i].vy = shared->particles[j].vy;
                    shared->particles[j].vy = troca;                   
                }                
            }
        }

        // Trata a gravidade entre particulas
        // Já que as partículas possuem a mesma massa, densidade e raio
        for (int j = 0; j < NUM_PARTICLES; ++j) {
            if (j != i) { // Para não ser a mesma partícula
                double dx = shared->particles[j].x - shared->particles[i].x;
                double dy = shared->particles[j].y - shared->particles[i].y;
                double distance = sqrt(dx * dx + dy * dy);

                if (distance > 0.0 && distance < DGravity) {
                    // Aplica a força da gravidade entre as partículas
                    double force = FGravity / (distance * distance); // Força proporcional à inversa do quadrado da distância 
                    shared->particles[i].vx += force * (dx / distance);
                    shared->particles[i].vy += force * (dy / distance);
                }
            }
        }
    }
}

void updatePosition(SharedState* shared, int min, int max) {
    for (int i = min; i < max; ++i) {
        // Atualiza a posição com base na velocidade
        shared->particles[i].x += shared->particles[i].vx * DT;
        shared->particles[i].y += shared->particles[i].vy * DT;
    }
}

void printParticle(SharedState* shared, int N, int step) {
    cout << "Passo " << step << " | Particula 0 -> Pos: (" 
    << shared->particles[N].x << ", " << shared->particles[N].y << ") | Vel: (" 
    << shared->particles[N].vx << ", " << shared->particles[N].vy << ")\n";
}

int main(int argc, char* argv[]) {
    // 2. VERIFICAÇÃO SE É O PROCESSO FILHO
    // Se o programa foi aberto com o argumento "sou_filho", ele pula o código do pai
    if (argc > 2 && string(argv[1]) == "sou_filho") {
        int id = stoi(argv[2]); // Pega o ID passado pelo pai
        cout << "Processo filho " << id << " iniciado.\n";
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

        for (int i = 0; i < argc; ++i) {
            cout << "Arg[ " << i << "] " << argv[i] << endl;
        }

        if(F==1){
            initParticles(shared, min, max);
        }else if(F==2){
            updateForces(shared, min, max);
        }else if(F==3){
            updatePosition(shared, min, max);
        }

        

        UnmapViewOfFile(shared);
        CloseHandle(hMap);
        return 0; // Finaliza o processo filho imediatamente
    }

    // 3. CÓDIGO DO PROCESSO PAI
    cout << "--- Processo PAI Iniciado ---\n";

    HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, 
            PAGE_READWRITE, 0, sizeof(SharedState), "ParticlesSharedMemory");
    
    if (hMap == NULL) {
        cout << "CreateFileMapping falhou\n";
        return 1;
    }

    SharedState* shared = (SharedState*)MapViewOfFile(
        hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedState));
    
    if (shared == NULL) {
        cout << "MapViewOfFile falhou\n";
        CloseHandle(hMap);
        return 1;
    }

    vector<PROCESS_INFORMATION> filhos;
    vector<STARTUPINFOA> infos;
    int range = NUM_PARTICLES/NUM_PROCESSES;
    int rmod  = NUM_PARTICLES%NUM_PROCESSES;

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
                                    // 1 para initParticles, 2 para updateForces, 3 para updatePosition

        if (!CreateProcessA(NULL, comando.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            cerr << "Falha ao criar o Processo " << i << ". Erro: " << GetLastError() << "\n";
            return 1;
        }

        filhos.push_back(pi);
        infos.push_back(si);

        cout << "Processo Filho " << i << " criado com sucesso.\n";
    }

    cout << "Processo Pai criou os dois filhos e esta aguardando...\n";

    for (auto& pi : filhos) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess); // feacha o processo
        CloseHandle(pi.hThread);  // fecha a thread
    }
    cout << "--- Todos os processos filhos terminaram.\n";

    
    cout << "Passo " << 0 << " | Particula 0 -> Pos: (" 
                        << shared->particles[0].x << ", " << shared->particles[0].y << ") | Vel: (" 
                        << shared->particles[0].vx << ", " << shared->particles[0].vy << ")\n";


    for (int step = 0; step < TOTAL_STEPS; ++step) {

        
        


        
        


        if ((step % 5 == 0)) {
            // Imprime o estado de uma particula
            printParticle(shared, 0, step);
        }
    }
        
    UnmapViewOfFile(shared);
    CloseHandle(hMap);
    cout << "--- Memoria compartilhada liberada.\n";

    cout << "--- Processo PAI finalizado! ---\n";
    return 0;
}