#include <iostream>
#include <thread>
#include <chrono>
#include <string>
using namespace std;

void funcao(int p_limite, string p_string){
    cout << "String: " << p_string << endl;
    for (int i=0;i<p_limite;++i){
        cout << "Sequencia: " << i << endl;
    }
}

int main() {
    int i=10;

    thread p0(funcao, i, "Hello World!!");
    p0.join();


    return 0;
}