#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;

// Dimensões do domínio da simulação
const double BOX_WIDTH = 800.0;
const double BOX_HEIGHT = 600.0;
const double GRAVITY = -9.81;
const double DT = 0.01; // Passo de tempo (Delta t)

struct Particle {
    double x, y;   // Posição
    double vx, vy; // Velocidade
    double radius; // Raio da partícula
};

// Inicializa as partículas com posições e velocidades aleatórias
void initParticles(vector<Particle>& particles, int numParticles) {
    srand(time(0));
    for (int i = 0; i < numParticles; ++i) {
        Particle p;
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

// Atualiza a física do sistema de forma sequencial
void updateSimulation(vector<Particle>& particles) {
    for (size_t i = 0; i < particles.size(); ++i) {
        // 1. Aplica a força da gravidade na velocidade vertical
        particles[i].vy += GRAVITY * DT;

        // 2. Atualiza a posição com base na velocidade
        particles[i].x += particles[i].vx * DT;
        particles[i].y += particles[i].vy * DT;

        // 3. Trata colisão com as bordas verticais (Esquerda e Direita)
        if (particles[i].x - particles[i].radius < 0) {
            particles[i].x = particles[i].radius;
            particles[i].vx = -particles[i].vx * 0.8; // Perde 20% de energia no impacto
        } else if (particles[i].x + particles[i].radius > BOX_WIDTH) {
            particles[i].x = BOX_WIDTH - particles[i].radius;
            particles[i].vx = -particles[i].vx * 0.8;
        }

        // 4. Trata colisão com as bordas horizontais (Teto e Chão)
        if (particles[i].y - particles[i].radius < 0) {
            particles[i].y = particles[i].radius;
            particles[i].vy = -particles[i].vy * 0.8;
        } else if (particles[i].y + particles[i].radius > BOX_HEIGHT) {
            particles[i].y = BOX_HEIGHT - particles[i].radius;
            particles[i].vy = -particles[i].vy * 0.8;
        }
    }
}

int main() {
    const int NUM_PARTICLES = 1000;
    const int TOTAL_STEPS = 30; // Número de passos da simulação

    vector<Particle> particles;
    initParticles(particles, NUM_PARTICLES);

    cout << "Iniciando simulacao sequencial de " << NUM_PARTICLES << " particulas...\n"; 

    cout << "Antes #" << " | Particula 0 -> Pos: (" 
                      << particles[0].x << ", " << particles[0].y << ") | Vel: (" 
                      << particles[0].vx << ", " << particles[0].vy << ")\n";

    // Loop principal da simulação
    for (int step = 0; step < TOTAL_STEPS; ++step) {
        updateSimulation(particles);

        // Imprime o estado da primeira partícula a cada 100 passos como amostra
        if ((step % 2 == 0)) {
            cout << "Passo " << step << " | Particula 0 -> Pos: (" 
                             << particles[0].x << ", " << particles[0].y << ") | Vel: (" 
                             << particles[0].vx << ", " << particles[0].vy << ")\n";
        }
    }

    cout << "Simulacao concluida com sucesso.\n";
    return 0;
}
