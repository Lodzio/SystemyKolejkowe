#include <iostream>
#include <cmath>
#include <cstdlib>
#define M 4294967296
#define A 134775813
#define C 1
//long lastX = time(NULL);
long lastX = 10;

unsigned long LCG(){
//    return lastX = (A*lastX+C)%M;
    return rand();
}

double laplace(int m, int l){
    double x = (LCG() % 10000)/10000.0;
    if( x < 0.5)
        return (log(2*x)*l)+m;
    if (x > 0.5)
        return -(log(2*(1-x))*l)+m;
    return 0;
}
double clearGauss(double x, int m, int l){
    return (1/(sqrt(M_PI*2*l)))*exp(-pow((x - m), 2)/(2*l));
}
double clearLaplace(double x,int m,int l){
    return (1/2.0*l)*exp(-abs(x - m)/l);
}
long GaussRand(int m, int l){
    while(true){
        double y = (LCG()%1000)/1000.0;
        double x = laplace(m, l);
        if (clearGauss(x, m, l) > clearLaplace(x, m, l)*y*2)
            return x;
    }
}

int main(){
    srand(time(NULL));
    for (int i = 0; i < 100; i++){
        std::cout << GaussRand(60, 20) << std::endl;
    }
//    int maxT = 10000;
//    int time = 0;
//    while(maxT > time){
//        time += 60;
//        std::cout << time << std::endl;
//    }
}