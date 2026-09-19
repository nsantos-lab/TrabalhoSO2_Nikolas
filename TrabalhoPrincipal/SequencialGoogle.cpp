#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

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

// Inicializa as partículas com posições e velocidades aleatórias
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

int main() {
    const int NUM_PARTICLES = 10000;
    const int TOTAL_STEPS = 30; // Número de passos da simulação

    vector<Particle> particles;
    initParticles(particles, NUM_PARTICLES);

    cout << "Iniciando simulacao sequencial de " << NUM_PARTICLES << " particulas...\n"; 

    // Loop principal da simulação
    for (int step = 0; step < TOTAL_STEPS; ++step) {
        updateForces(particles);
        updatePosition(particles);

        // Imprime o estado da primeira partícula
        if ((step % 5 == 0)) {
            cout << "Passo " << step << " | Particula 0 -> Pos: (" 
                             << particles[0].x << ", " << particles[0].y << ") | Vel: (" 
                             << particles[0].vx << ", " << particles[0].vy << ")\n";
        }
    }

    cout << "Simulacao concluida com sucesso.\n";
    return 0;
}
