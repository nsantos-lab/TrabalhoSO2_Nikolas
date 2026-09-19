#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
using namespace std;

mutex trava;

void funcao_f0 (int &p_int){
    int *i = &p_int;
    for (int j = 0; j < 10; ++j){
        trava.lock();
        int tmp = *i;

        tmp = tmp + 1;
        
        *i = tmp;
        cout << "f0: " << *i << endl;
        trava.unlock();

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void funcao_f1 (int &p_int){
    int *i = &p_int;
    for (int j = 0; j < 10; ++j){
        trava.lock();
        int tmp = *i;

        tmp = tmp + 1;
        
        *i = tmp;
        cout << "f1: " << *i << endl;
        trava.unlock();

        this_thread::sleep_for(chrono::milliseconds(900));
    }
}

int main() {
    int i = 0;

    thread f0(funcao_f0, ref(i));
    thread f1(funcao_f1, ref(i));

    f0.join();
    f1.join();

    cout << "Fim\n";

    return 0;
}