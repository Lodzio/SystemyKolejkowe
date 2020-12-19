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
int exponentialRand(double lambda){
    double x = (LCG()%10000)/10000.0;
    return -log(1-x)/lambda;
}

std::queue < int > cars;


int sumTime(int maxN, int operationTimes[]){
    int result = 0;
    for (int i = 0; i < maxN; i++){
        result += operationTimes[i];
    }
    return result;
}

int standardDeviation(int maxN, int operationTimes[]){
    int result = 0;
    const double averageTime = sumTime(maxN, operationTimes)/(double)maxN;
    for (int i = 0; i < maxN; i++){
        result += pow(operationTimes[i] - averageTime, 2);
    }
    result /= maxN;
    result = sqrt(result);
    return result;
}

void getAverageQueueTime(std::function< int() > getEventOperationTime, int N){
    int* operationTimes = new int[N];
    int operatedEventsAmount = 0;
    int missed = 0;
    int allCars = 0;
    int operationTimeLeft = 0;
    int nextEventTime = 0;

    for (int i = 0; operatedEventsAmount < N; i++){
        int timeDiff = nextEventTime < operationTimeLeft || !operationTimeLeft ? nextEventTime: operationTimeLeft;
        operationTimeLeft -= timeDiff;
        operationTimeLeft = std::max(operationTimeLeft, 0);
        nextEventTime -= timeDiff;
        if (operationTimeLeft == 0 && !cars.empty()){
            operationTimes[operatedEventsAmount] = cars.front();
            cars.pop();
            allCars++;
            if (operatedEventsAmount){
                std::cout << "klienci obsłużeni: " << (allCars-missed)*100.0/allCars << "%" << std::endl;
                std::cout << "%czas przebywania w systemie: av=" << sumTime(operatedEventsAmount, operationTimes)/operatedEventsAmount;
                std::cout << " ,sigm=: " << standardDeviation(operatedEventsAmount, operationTimes) << "s" << std::endl;
            }
            operatedEventsAmount++;
            operationTimeLeft = getEventOperationTime();
            std::queue < int > eventsCopy;
            while (!cars.empty())
            {
                eventsCopy.push(cars.front() + operationTimeLeft);
                cars.pop();
            }
            cars = eventsCopy;
        }
        if (nextEventTime == 0){
            if (cars.size() < 2){
                if (operationTimeLeft == 0){
                    operationTimeLeft = getEventOperationTime();
                }
                cars.push(operationTimeLeft);
            } else {
                missed++;
            }
            nextEventTime = exponentialRand(1/120.0);
        }
    }
}

int main(){
    auto exponentialOperationTime = []{return exponentialRand(1/60.0);};
    int N = 60;
    getAverageQueueTime(exponentialOperationTime, N);
    return 0;
}