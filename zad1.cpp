#include <iostream>
#include <cmath>
#include <cstdlib>
#include <queue>
#define M 4294967296
#define A 134775813
#define C 1
long lastX = time(NULL);

unsigned long LCG(){
    lastX = (A*lastX+C)%M;
    return lastX;
}

int exponentialRand(double lambda){
    double x = (LCG()%10000)/10000.0;
    return -log(1-x)/lambda;
}

int main(){
    int nextEventTime = 0;

    for (int i = 0; i < 100000; i++){
        nextEventTime += exponentialRand(1/120.0);
        std::cout << nextEventTime << std::endl;
    }
}