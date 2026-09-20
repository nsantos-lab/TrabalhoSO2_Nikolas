#include <iostream>
#include <windows.h> // API nativa do Windows para processos
#include <string>
#include <vector>
using namespace std;

// Dimensões do domínio da simulação
const double BOX_WIDTH = 800.0;
const double BOX_HEIGHT = 600.0;
const double GRAVITY = -9.81;
const double DT = 0.001; // Passo de tempo (Delta t)
const double DGravity = 50.0; // Distancia máxima para aplicar a gravidade entre partículas
const double FGravity = 50.0; // Força padrão da gravidade entre partículas

// 1. Cole aqui a função do seu código sequencial original
void executar_sua_logica(int id_do_processo) {
    cout << "Processo filho " << id_do_processo << " executando a logica...\n";
    
    // [TODO]: Coloque o corpo da sua função sequencial aqui
    
    cout << "Processo filho " << id_do_processo << " terminou.\n";
}

struct Particle {
    double x, y;   // Posição
    double vx, vy; // Velocidade
    double radius; // Raio da partícula
};

void initParticles(vector<Particle>& particles, int numParticles) {
    srand(time(0));
    for (int i = 0; i < numParticles; ++i) {
        Particle p; // cria a particula
        p.radius = 5.0; // Raio fixo para simplificação
        // Garante que a partícula comece totalmente dentro da caixa
        p.x = p.radius + (rand() % static_cast<int>(BOX_WIDTH  - 2 * p.radius + 1));
        p.y = p.radius + (rand() % static_cast<int>(BOX_HEIGHT - 2 * p.radius + 1));
        // Velocidades aleatórias entre -50 e 50
        p.vx = (rand() % 101) - 50;
        p.vy = (rand() % 101) - 50;
        particles.push_back(p);
    }
}

int main(int argc, char* argv[]) {
    // 2. VERIFICAÇÃO SE É O PROCESSO FILHO
    // Se o programa foi aberto com o argumento "sou_filho", ele pula o código do pai
    if (argc > 2 && string(argv[1]) == "sou_filho") {
        int id = stoi(argv[2]); // Pega o ID passado pelo pai
        executar_sua_logica(id);
        return 0; // Finaliza o processo filho imediatamente
    }

    // 3. CÓDIGO DO PROCESSO PAI
    cout << "--- Processo PAI Iniciado ---\n";

    const int NUM_PARTICLES = 10000;
    vector<Particle> particles; // Exemplo: vetor de partículas
    initParticles(particles, NUM_PARTICLES);


    // Estruturas exigidas pelo Windows para gerenciar os novos processos
    STARTUPINFOA si1 = {};
    STARTUPINFOA si2 = {};
    si1.cb = sizeof(si1);
    si2.cb = sizeof(si2);
    PROCESS_INFORMATION pi1, pi2;

    // Constrói a linha de comando para chamar o próprio executável (argv[0])
    // Exemplo de comando final: "C:\pasta\programa.exe sou_filho 1"
    string comando1 = string(argv[0]) + " sou_filho 1";
    string comando2 = string(argv[0]) + " sou_filho 2";

    // Criando o Processo Filho 1
    // (Precisamos converter a string do C++ para uma array de char modificável)
    if (!CreateProcessA(NULL, comando1.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si1, &pi1)) {
        cerr << "Falha ao criar o Processo 1. Erro: " << GetLastError() << "\n";
        return 1;
    }

    // Criando o Processo Filho 2
    if (!CreateProcessA(NULL, comando2.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2)) {
        cerr << "Falha ao criar o Processo 2. Erro: " << GetLastError() << "\n";
        return 1;
    }

    cout << "Processo Pai criou os dois filhos e esta aguardando...\n";

    // 4. SINCRONIZAÇÃO (Equivalente ao wait() do Linux / join() de threads)
    // Cria um array com os "Handles" (identificadores) dos processos filhos
    HANDLE processos_filhos[2] = { pi1.hProcess, pi2.hProcess };
    
    // Trava o processo pai até que AMBOS (TRUE) os processos terminem
    WaitForMultipleObjects(2, processos_filhos, TRUE, INFINITE);

    // 5. LIMPEZA
    // Fecha os handles abertos para liberar os recursos no Windows
    CloseHandle(pi1.hProcess); CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess); CloseHandle(pi2.hThread);

    cout << "--- Ambos os processos filhos terminaram. Processo PAI finalizado! ---\n";
    return 0;
}