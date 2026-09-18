#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
using namespace std;

struct thread_controler{
    int id;
    bool finalizada;
    int pmin;
    int pmax;
};

void funcao(struct thread_controler & f0){
    for (; f0.pmin <= f0.pmax; f0.pmin++){
        cout << " " << f0.pmin << " ";
    }
    f0.finalizada = true;
}

int main() {
    
    vector<thread> VThreads;
    int TAM = 15;
    int core = 4;
    int range = TAM/core;
    int Mrange = TAM%core;
    vector<thread_controler> VThread_Controler;
    VThread_Controler.resize(core);

    for (int i = 0; i < core; i++){
        //VThread_Controler[i];
        VThread_Controler[i].id = i;
        VThread_Controler[i].finalizada = false;

        if (i==0){
            VThread_Controler[i].pmin = 0;
            VThread_Controler[i].pmax = range;
        }else if (i==core-1){
            VThread_Controler[i].pmin = range * i + 1;
            VThread_Controler[i].pmax = (i+1)*range + Mrange -1;
        }else {
            VThread_Controler[i].pmin = range * i + 1;
            VThread_Controler[i].pmax = (i+1)*range;
        }
        cout << "Thread" << i << " Min:" << VThread_Controler[i].pmin 
             << " Max:" << VThread_Controler[i].pmax << endl;
    }

    for (int i = 0; i < core; i++){
        VThreads.emplace_back(funcao, ref(VThread_Controler[i]));
        this_thread::sleep_for(chrono::seconds(1));
    }

    for (int i = 0; i< core; i++){
        VThreads[i].join();
        //cout << "Thread: " << i << " recolhida\n";
    }

    cout << "Fim\n";

    return 0;
}