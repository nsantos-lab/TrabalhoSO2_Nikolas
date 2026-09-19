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
};

struct Thread_Controler {
    int id;
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
}

// Atualiza a física do sistema de forma sequencial
void updateForces(vector<Particle>& particles, struct Thread_Controler & f0) {
    int totalParticles = static_cast<int>(particles.size());
    for (; f0.pmin <= f0.pmax; f0.pmin++) {
        // Aplica a força da gravidade na velocidade vertical
        particles[f0.pmin].vy += GRAVITY * DT;

        // Trata colisão com as bordas verticais (Esquerda e Direita)
        if (particles[f0.pmin].x - particles[f0.pmin].radius < 0) {
            particles[f0.pmin].vx = -particles[f0.pmin].vx; // Inverte a velocidade horizontal
        } else if (particles[f0.pmin].x + particles[f0.pmin].radius > BOX_WIDTH) {
            particles[f0.pmin].x = BOX_WIDTH - particles[f0.pmin].radius; // Força a partícula para dentro da caixa
            particles[f0.pmin].vx = -particles[f0.pmin].vx; // Inverte a velocidade horizontal
        }

        // Trata colisão com as bordas horizontais (Teto e Chão)
        if (particles[f0.pmin].y - particles[f0.pmin].radius < 0) {
            particles[f0.pmin].vy = -particles[f0.pmin].vy; // Inverte a velocidade vertical
        } else if (particles[f0.pmin].y + particles[f0.pmin].radius > BOX_HEIGHT) {
            particles[f0.pmin].y = BOX_HEIGHT - particles[f0.pmin].radius; // Força a partícula para dentro da caixa
            particles[f0.pmin].vy = -particles[f0.pmin].vy; // Inverte a velocidade vertical
        }

        // Trata colisão entre partículas
        for (int j = 0; j < totalParticles; ++j) {
            if (j != f0.pmin) { // Para não ser a mesma partícula
                double dx = particles[j].x - particles[f0.pmin].x;
                double dy = particles[j].y - particles[f0.pmin].y;
                double distance = sqrt(dx * dx + dy * dy);
                double minDistance = particles[f0.pmin].radius + particles[j].radius;

                if (distance < minDistance) {
                    // De acordo com as regras de colisão elástica, se é trocada as velocidades das partículas
                    double troca = particles[f0.pmin].vx;
                    particles[f0.pmin].vx = particles[j].vx;
                    particles[j].vx = troca;
                    troca = particles[f0.pmin].vy;
                    particles[f0.pmin].vy = particles[j].vy;
                    particles[j].vy = troca;                   
                }                
            }
        }

        // Trata a gravidade entre particulas
        // Já que as partículas possuem a mesma massa, densidade e raio
        for (int j = 0; j < totalParticles; ++j) {
            if (j != f0.pmin) { // Para não ser a mesma partícula
                double dx = particles[j].x - particles[f0.pmin].x;
                double dy = particles[j].y - particles[f0.pmin].y;
                double distance = sqrt(dx * dx + dy * dy);

                if (distance > 0.0 && distance < DGravity) {
                    // Aplica a força da gravidade entre as partículas
                    double force = FGravity / (distance * distance); // Força proporcional à inversa do quadrado da distância 
                    particles[f0.pmin].vx += force * (dx / distance);
                    particles[f0.pmin].vy += force * (dy / distance);
                }
            }
        }
    }
}

// Atualiza a posição das partículas com base nas forças
void updatePosition(vector<Particle>& particles, struct Thread_Controler & f0) {
    for (; f0.pmin <= f0.pmax; f0.pmin++) {
        // Atualiza a posição com base na velocidade
        particles[f0.pmin].x += particles[f0.pmin].vx * DT;
        particles[f0.pmin].y += particles[f0.pmin].vy * DT;
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
    const int NUM_PARTICLES = 5000;
    const int TOTAL_STEPS = 30; // Número de passos da simulação

    vector<Particle> particles(NUM_PARTICLES);
    vector<thread> Vthreads;
    vector<Thread_Controler> Vthread_controler(NUM_THREADS);

    int range = NUM_PARTICLES/NUM_THREADS;
    int rmod  = NUM_PARTICLES%NUM_THREADS;
    
    // Inicializa os controlers de cada Thread
    for (int i = 0; i < NUM_THREADS; ++i) {
        Vthread_controler[i].id = i;

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
    Vthreads.clear(); // mesmo as threads tendo recebido join 
                      // elas continuam a existir dentro do vector
                      // logo nao seria possivel dar join() novamente
    setRange(ref(Vthread_controler));

    cout << "Iniciando simulacao em threads de " << NUM_PARTICLES << " particulas...\n";

    // Loop principal da simulação
    for (int step = 0; step < TOTAL_STEPS; ++step) {

        for (int i = 0; i < NUM_THREADS; ++i) {
            Vthreads.emplace_back(updateForces, ref(particles), ref(Vthread_controler[i]));
        }
        for (auto& thread : Vthreads) {
            thread.join();
        }
        Vthreads.clear();
        setRange(ref(Vthread_controler));


        for (int i = 0; i < NUM_THREADS; ++i) {
            Vthreads.emplace_back(updatePosition, ref(particles), ref(Vthread_controler[i]));
        }
        for (auto& thread : Vthreads) {
            thread.join();
        }
        Vthreads.clear();
        setRange(ref(Vthread_controler));


        if ((step % 5 == 0)) {
            // Imprime o estado de uma particula
            printParticle(particles, 0, step);
        }
    }

    cout << "Simulacao concluida com sucesso.\n";
    return 0;
}
