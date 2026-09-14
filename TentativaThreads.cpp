#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void hello0() {
    for (int i = 0; i < 1000; i++) {
        cout << "Hello0 " << i << endl;
        //this_thread::sleep_for(chrono::seconds(1));
    }
}
void hello1() {
    for (int i = 0; i < 1000; i++) {
        cout << "Hello1 " << i << endl;
        //this_thread::sleep_for(chrono::seconds(1));
    }
}
void hello2() {
    for (int i = 0; i < 1000; i++) {
        cout << "Hello2 " << i << endl;
        //this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    thread t0(hello0);
    thread t1(hello1);
    thread t2(hello2);

    t0.join();
    cout << "Join t0\n";
    t1.join();
    cout << "Join t1\n";
    t2.join();
    cout << "Join t2\n";

    return 0;
}