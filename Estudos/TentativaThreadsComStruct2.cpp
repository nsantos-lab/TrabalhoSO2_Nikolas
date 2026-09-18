#include <iostream>
#include <thread>
#include <chrono>
#include <string>
using namespace std;

struct thread_controler{
    int id;
    bool continuar;
    bool finalizada;
    int Bpmin;
    int Bpmax;
    int pmin;
    int pmax;
};

void funcao_f0(struct thread_controler & f0){
    int i=0;
    cout << "PMAX: " << f0.pmax << endl;
    cout << "PMIN: " << f0.pmin << endl;
    cout << "Entrando no loop...\n";

    for (f0.pmin; f0.pmin <= f0.pmax; f0.pmin++){
        cout << "PMIN: " << f0.pmin << endl;
    }
    f0.finalizada = true;
}

int main() {
    struct thread_controler thread_p0;
    thread_p0.continuar = true;
    thread_p0.finalizada = false;
    thread_p0.pmin = 1;
    thread_p0.pmax = 10;

    thread p0(funcao_f0, ref(thread_p0));
    p0.detach();

    this_thread::sleep_for(chrono::seconds(7));
    //thread_p0.continuar = false;

    while (thread_p0.finalizada = false){
        this_thread::sleep_for(chrono::seconds(1));

    }

    cout << "Fim\n";

    return 0;
}