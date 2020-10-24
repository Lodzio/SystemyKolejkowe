#include <iostream>
#include <cmath>
#include <cstdlib>
#include <queue>
#define M 4294967296
#define A 134775813
#define C 1
//long lastX = time(NULL);

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
int exponentialRand(int lambda){
    double x = (LCG()%10000)/10000.0;
    return -log(1-x)*120;
}

std::queue < int > events;

int getEventOperationTime(){
    return 60;
}
int main(){
    srand(time(NULL));
    int operationWaitSum = 0;
    int operatedEventsAmount = 0;

    int operationTimeLeft = getEventOperationTime();
    int nextEventTime = exponentialRand(120);

    for (int i = 0; i < 1000000; i++){
        int timeDiff = nextEventTime < operationTimeLeft ? nextEventTime: operationTimeLeft;
        operationTimeLeft -= timeDiff;
        nextEventTime -= timeDiff;
//        std::cout << "==================" << std::endl;
//        std::cout << operationTimeLeft << std::endl;
//        std::cout << nextEventTime << std::endl;
//        std::cout << timeDiff << std::endl;
//        std::cout << events.size() << std::endl;

        if (operationTimeLeft == 0){
            if (events.empty()){
                events.push(operationTimeLeft);
                nextEventTime = exponentialRand(120);
                continue;
            }
            operationWaitSum += events.front();
            events.pop();
            operatedEventsAmount++;
            operationTimeLeft = getEventOperationTime();
            std::queue < int > eventsCopy;
            while (!events.empty())
            {
                eventsCopy.push(events.front() + operationTimeLeft);
                events.pop();
            }
            events = eventsCopy;
        } else {
            events.push(operationTimeLeft);
            nextEventTime = exponentialRand(120);
        }
//        std::cout << GaussRand(60, 20) << std::endl;
//        std::cout << exponentialRand(120) << std::endl;
    }
std::cout << "final: " << std::endl;
    std::cout << operationWaitSum << std::endl;
    std::cout << operatedEventsAmount << std::endl;
    std::cout << operationWaitSum/(double)operatedEventsAmount << std::endl;
}