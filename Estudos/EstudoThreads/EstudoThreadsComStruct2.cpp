#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
using namespace std;

struct thread_controler{
    int id;
    bool finalizada;
    int Bmin;
    int Bmax;
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
    vector<thread_controler> VThread_Controler(core);

    for (int i = 0; i < core; i++){
        VThread_Controler[i].id = i;
        VThread_Controler[i].finalizada = false;

        if (i==0){
            VThread_Controler[i].Bmin = 0;
            VThread_Controler[i].Bmax = range;
        }else if (i==core-1){
            VThread_Controler[i].Bmin = range * i + 1;
            VThread_Controler[i].Bmax = (i+1)*range + Mrange -1;
        }else {
            VThread_Controler[i].Bmin = range * i + 1;
            VThread_Controler[i].Bmax = (i+1)*range;
        }
        cout << "Thread" << i << " Min:" << VThread_Controler[i].Bmin 
             << " Max:" << VThread_Controler[i].Bmax << endl;
    }

    // seta o range de cada thread
    for (int i = 0; i < core; i++){
        VThread_Controler[i].pmin = VThread_Controler[i].Bmin;
        VThread_Controler[i].pmax = VThread_Controler[i].Bmax;
    }

    for (int i = 0; i < core; i++){
        VThreads.emplace_back(funcao, ref(VThread_Controler[i]));
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << endl;

    // set o range de cada thread
    for (int i = 0; i < core; i++){
        VThread_Controler[i].pmin = VThread_Controler[i].Bmin;
        VThread_Controler[i].pmax = VThread_Controler[i].Bmax;
    }

    for (int i = 0; i < core; i++){
        cout << "Thread" << i << " Min:" << VThread_Controler[i].pmin 
             << " Max:" << VThread_Controler[i].pmax << endl;
    }


    for (int i = 0; i< core; i++){
        VThreads[i].join();
        //cout << "Thread: " << i << " recolhida\n";
    }

    cout << "Fim\n";

    return 0;
}