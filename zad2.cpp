#include <iostream>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <random>
#include <queue>
#define M 4294967296
#define A 134775813
#define C 1
long lastX = time(NULL);
std::default_random_engine generator;
std::normal_distribution<double> gaussDistribution(60.0,20.0);

unsigned long LCG(){
    lastX = (A*lastX+C)%M;
    return lastX;
}

double laplaceRand(int m, int l){
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
int myGaussRand(int m, int l){
    while(true){
        double y = (LCG()%1000)/1000.0;
        double x = laplaceRand(m, l);
        if (clearGauss(x, m, l) > clearLaplace(x, m, l)*y*2){
            return x;
        }
    }
}
int gaussRand(){
    return gaussDistribution(generator);
}
double exponentialRand(double lambda){
    double x = (LCG()%10000)/10000.0;
    return -log(1-x)/lambda;
}

std::queue < double > events;

double getAverageQueueTime(std::function< int() > getEventOperationTime, int N){
    double operationWaitSum = 0;
    int operatedEventsAmount = 0;

    double operationTimeLeft = getEventOperationTime();
    double nextEventTime = exponentialRand(1/120.0);

    for (int i = 0; i < N; i++){
        double timeDiff = nextEventTime < operationTimeLeft ? nextEventTime: operationTimeLeft;
        operationTimeLeft -= timeDiff;
        nextEventTime -= timeDiff;

        if (operationTimeLeft == 0){
            if (events.empty()){
                events.push(operationTimeLeft);
                nextEventTime = exponentialRand(1/120.0);
                continue;
            }
            operationWaitSum += events.front();
            events.pop();
            operatedEventsAmount++;
            operationTimeLeft = getEventOperationTime();
            std::queue < double > eventsCopy;
            while (!events.empty())
            {
                eventsCopy.push(events.front() + operationTimeLeft);
                events.pop();
            }
            events = eventsCopy;
        } else {
            events.push(operationTimeLeft);
            nextEventTime = exponentialRand(1/120.0);
        }
    }
    return operationWaitSum/(double)operatedEventsAmount;
}

int main(){
    srand(time(NULL));
    auto constantOperationTime = []{return 60;};
    auto UOperationTime = []{return ((LCG() % 10000)/10000.0)*120;};
    auto exponentialOperationTime = []{return exponentialRand(1/60.0);};
    auto gaussOperationTime = []{return gaussRand();};
    auto myGaussOperationTime = []{return myGaussRand(60, 20);};
    double result;

//    result = getAverageQueueTime(constantOperationTime, 10000000);
//    std::cout << "czas obslugi dokladnie 60s: " << result << std::endl;
//    result = getAverageQueueTime(UOperationTime, 10000000);
//    std::cout << "jest zmienną losową o rozkładzie U(0,120): " << result << std::endl;
//    result = getAverageQueueTime(exponentialOperationTime, 10000000);
//    std::cout << "jest zmienną losową o rozkładzie E(60): " << result << std::endl;
//    result = getAverageQueueTime(gaussOperationTime, 10000000);
//    std::cout << "jest zmienną losową o rozkładzie N(60,20) " << result << std::endl;
    result = getAverageQueueTime(myGaussOperationTime, 1000000);
    std::cout << "jest zmienną losową o rozkładzie N(60,20) dla własnego generatora: " << result << std::endl;

// 30
// 40
// 60
// 33.333333

    return 0;
}