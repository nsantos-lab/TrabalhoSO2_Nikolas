#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
using namespace std;

#define NUM_THREADS 4 // Número de threads a serem usadas na simulação

// Dimensões do domínio da simulação
const double BOX_WIDTH = 800.0;
const double BOX_HEIGHT = 600.0;
const double GRAVITY = -9.81;
const double DT = 0.001; // Passo de tempo (Delta t)
const double DGravity = 50.0; // Distancia máxima para aplicar a gravidade entre partículas
const double FGravity = 50.0; // Força da gravidade entre partículas

struct Particle {
    double x, y;   // Posição
    double vx, vy; // Velocidade
    double radius; // Raio da partícula
    // double nvx, nvy; // Velocidade a ser atualizada (para evitar interferência durante a atualização)
};

struct Thread_Controler {
    int id;
    bool finalizada;
    int Bpmin;
    int Bpmax;
    int pmin;
    int pmax;
};

// Inicializa as partículas com posições e velocidades aleatórias
void initParticles(vector<Particle>& particles, struct Thread_Controler & f0) {
    srand(time(0));
    for (; f0.pmin <= f0.pmax; f0.pmin++) {
        Particle p; // cria a particula
        p.radius = 5.0; // Raio fixo para simplificação
        // Garante que a partícula comece totalmente dentro da caixa
        p.x = p.radius + (rand() % static_cast<int>(BOX_WIDTH  - 2 * p.radius + 1));
        p.y = p.radius + (rand() % static_cast<int>(BOX_HEIGHT - 2 * p.radius + 1));
        // Velocidades aleatórias entre -50 e 50
        p.vx = (rand() % 101) - 50;
        p.vy = (rand() % 101) - 50;
        // coloca a particula em uma posicao especifica
        particles[f0.pmin] = p;
    }
    f0.finalizada = true;
}

// Atualiza a física do sistema de forma sequencial
void updateForces(vector<Particle>& particles) {
    for (size_t i = 0; i < particles.size(); ++i) {
        // Aplica a força da gravidade na velocidade vertical
        particles[i].vy += GRAVITY * DT;

        // Trata colisão com as bordas verticais (Esquerda e Direita)
        if (particles[i].x - particles[i].radius < 0) {
            particles[i].vx = -particles[i].vx; // Inverte a velocidade horizontal
        } else if (particles[i].x + particles[i].radius > BOX_WIDTH) {
            particles[i].x = BOX_WIDTH - particles[i].radius; // Força a partícula para dentro da caixa
            particles[i].vx = -particles[i].vx; // Inverte a velocidade horizontal
        }

        // Trata colisão com as bordas horizontais (Teto e Chão)
        if (particles[i].y - particles[i].radius < 0) {
            particles[i].vy = -particles[i].vy; // Inverte a velocidade vertical
        } else if (particles[i].y + particles[i].radius > BOX_HEIGHT) {
            particles[i].y = BOX_HEIGHT - particles[i].radius; // Força a partícula para dentro da caixa
            particles[i].vy = -particles[i].vy; // Inverte a velocidade vertical
        }

        // Trata colisão entre partículas
        for (size_t j = 0; j < particles.size(); ++j) {
            if (j != i) { // Para não ser a mesma partícula
                double dx = particles[j].x - particles[i].x;
                double dy = particles[j].y - particles[i].y;
                double distance = sqrt(dx * dx + dy * dy);
                double minDistance = particles[i].radius + particles[j].radius;

                if (distance < minDistance) {
                    // De acordo com as regras de colisão elástica, se é trocada as velocidades das partículas
                    double troca = particles[i].vx;
                    particles[i].vx = particles[j].vx;
                    particles[j].vx = troca;
                    troca = particles[i].vy;
                    particles[i].vy = particles[j].vy;
                    particles[j].vy = troca;                   
                }                
            }
        }

        // Trata a gravidade entre particulas
        // Já que as partículas possuem a mesma massa, densidade e raio
        for (size_t j = 0; j < particles.size(); ++j) {
            if (j != i) { // Para não ser a mesma partícula
                double dx = particles[j].x - particles[i].x;
                double dy = particles[j].y - particles[i].y;
                double distance = sqrt(dx * dx + dy * dy);

                if (distance > 0.0 && distance < DGravity) {
                    // Aplica a força da gravidade entre as partículas
                    double force = FGravity / (distance * distance); // Força proporcional à inversa do quadrado da distância 
                    particles[i].vx += force * (dx / distance);
                    particles[i].vy += force * (dy / distance);
                }
            }
        }
    }
}

// Atualiza a posição das partículas com base nas forças
void updatePosition(vector<Particle>& particles) {
    for (size_t i = 0; i < particles.size(); ++i) {
        // Atualiza a posição com base na velocidade
        particles[i].x += particles[i].vx * DT;
        particles[i].y += particles[i].vy * DT;
    }
}

void setRange(vector<Thread_Controler>& Vthread_controler) {
    for (int i = 0; i < NUM_THREADS; ++i) {
        Vthread_controler[i].pmin = Vthread_controler[i].Bpmin;
        Vthread_controler[i].pmax = Vthread_controler[i].Bpmax; 
    }
}

void printRange(vector<Thread_Controler>& Vthread_controler) {
    for (int i = 0; i < NUM_THREADS; ++i) {
        cout << "Thread N: " << i << endl;
        cout << "Range Min: " << Vthread_controler[i].pmin << endl;
        cout << "Range Max: " << Vthread_controler[i].pmax << endl;
    }
}

void printParticle(vector<Particle>& particles, int N, int step) {
    cout << "Passo " << step << " | Particula 0 -> Pos: (" 
    << particles[N].x << ", " << particles[N].y << ") | Vel: (" 
    << particles[N].vx << ", " << particles[N].vy << ")\n";
}

int main() {
    const int NUM_PARTICLES = 100;
    const int TOTAL_STEPS = 30; // Número de passos da simulação

    vector<Particle> particles(NUM_PARTICLES);
    vector<thread> Vthreads;
    vector<Thread_Controler> Vthread_controler(NUM_THREADS);

    int range = NUM_PARTICLES/NUM_THREADS;
    int rmod  = NUM_PARTICLES%NUM_THREADS;
    
    // Inicializa os controlers de cada Thread
    for (int i = 0; i < NUM_THREADS; ++i) {
        Vthread_controler[i].id = i;
        Vthread_controler[i].finalizada = false;

        if(i==0) { // significa que é o primeiro
            Vthread_controler[i].Bpmin = 0;
            Vthread_controler[i].Bpmax = range;          
        }else if(i==NUM_THREADS-1) { // significa que é o ultimo
            Vthread_controler[i].Bpmin = i*range + 1;
            Vthread_controler[i].Bpmax = (i+1)*range + rmod - 1;
        }else {
            Vthread_controler[i].Bpmin = i*range + 1;
            Vthread_controler[i].Bpmax = (i+1)*range;
        }
    }
    cout << "Controlers Inicializados\n";

    // seta o range de cada Thread
    setRange(ref(Vthread_controler));
    // printa o range de cada Thread
    //printRange(ref(Vthread_controler));
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        Vthreads.emplace_back(initParticles, ref(particles), ref(Vthread_controler[i]));
    }

    // espera as threads acabarem suas funcoes
    for (auto& thread : Vthreads) {
        thread.join();
    }

    setRange(ref(Vthread_controler));
    //printRange(ref(Vthread_controler));

    cout << "Iniciando simulacao em threads de " << NUM_PARTICLES << " particulas...\n";
    
    // Imprime o estado de uma particula antes da simulacao
    cout << "Antes #" << " | Particula 0 -> Pos: (" 
                      << particles[0].x << ", " << particles[0].y << ") | Vel: (" 
                      << particles[0].vx << ", " << particles[0].vy << ")\n";

    // Loop principal da simulação
    for (int step = 0; step < TOTAL_STEPS; ++step) {
        updateForces(particles);
        updatePosition(particles);

        if ((step % 5 == 0)) {
            // Imprime o estado de uma particula
            cout << "Passo " << step << " | Particula 0 -> Pos: (" 
            << particles[0].x << ", " << particles[0].y << ") | Vel: (" 
            << particles[0].vx << ", " << particles[0].vy << ")\n";
        }
    }

    cout << "Simulacao concluida com sucesso.\n";
    return 0;
}
