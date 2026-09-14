#include <iostream>
#include <thread>
#include <chrono>

void hello0() {
    for (int i = 0; i < 50; i++) {
        std::cout << "Hello0 " << i << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void hello1() {
    for (int i = 0; i < 50; i++) {
        std::cout << "Hello1 " << i << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void hello2() {
    for (int i = 0; i < 50; i++) {
        std::cout << "Hello2 " << i << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::thread t0(hello0);
    std::thread t1(hello1);
    std::thread t2(hello2);

    t0.join();
    t1.join();
    t2.join();

    return 0;
}