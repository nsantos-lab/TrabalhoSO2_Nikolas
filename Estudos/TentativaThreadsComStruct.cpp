#include <iostream>
#include <thread>
#include <chrono>
#include <string>
using namespace std;

struct thread_controler{
    bool continuar;
    bool finalizada;
};

void funcao_f0(struct thread_controler & f0){
    int i=0;

    while(f0.continuar){
        cout << "i: " << i++ << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    f0.finalizada = true;
}

int main() {
    struct thread_controler thread_p0;
    thread_p0.continuar = true;
    thread_p0.finalizada = false;

    thread p0(funcao_f0, ref(thread_p0));
    p0.detach();

    this_thread::sleep_for(chrono::seconds(7));
    thread_p0.continuar = false;

    while (thread_p0.finalizada = false){
        this_thread::sleep_for(chrono::seconds(1));

    }

    cout << "Fim\n";

    return 0;
}