#include <iostream>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <random>
#include <queue>
#include <iomanip>
#define M 4294967296
#define A 134775813
#define C 1
long lastX = time(NULL);
std::default_random_engine generator;
std::normal_distribution<double> gaussDistribution(60.0,20.0);

struct Car {
    Car(double _time, double _queueTime, int _id){
        time = _time;
        queueTime = _queueTime;
        id = _id;
    }
    double queueTime;
    double time;
    int id;
};

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

std::queue < Car > cars;


double sumTime(int maxN, double operationTimes[]){
    double result = 0;
    for (int i = 0; i < maxN; i++){
        result += operationTimes[i];
    }
    return result;
}

double standardDeviation(int maxN, double operationTimes[]){
    double result = 0;
    const double averageTime = sumTime(maxN, operationTimes)/(double)maxN;
    for (int i = 0; i < maxN; i++){
        result += pow(operationTimes[i] - averageTime, 2);
    }
    result /= maxN;
    result = sqrt(result);
    return result;
}

void printId(int id){
    if(id == -1){
        std::cout << "        ";
    } else {
        std::cout << std::setfill(' ') <<std::setw(8) << id;
    }
}

void printState(int operatedEventsAmount,int allCars, int missed, double* operationTimes, double* queueTimes, int id, std::string state){
    int systemSize = 3;
    int carsAmount = cars.size();
    int* queueStatus = new int[systemSize];
    std::queue < Car > eventsCopy;
    for(int i = 0; i < systemSize; i++){
        if(!cars.empty()){
            Car car = cars.front();
            queueStatus[i] = car.id;
            eventsCopy.push(car);
            cars.pop();
        } else {
            queueStatus[i] = -1;
        }
    }
    cars = eventsCopy;
    std::cout << "------------------------------------"<< std::endl;
    std::cout << "samochód nr: " << id << " " << state << std::endl;
    std::cout << "klienci obsłużeni: " << (allCars-missed)*100.0/allCars << "%" << std::endl;
    std::cout << "         ******************************"<< std::endl;
    std::cout << "status   ";
    for (int i = 2; i >= 0; i--){
        int id = queueStatus[i];
        if (i == 0){
            std::cout << "||";
            printId(id);
            std::cout << "||";
        } else{
            printId(id);
        }
        if (i != 0){
            std::cout << "," ;
        }
    }
    std::cout << std::endl;
    std::cout << "         ******************************"<< std::endl;
    std::cout << "czas przebywania w systemie: av=" << sumTime(operatedEventsAmount, operationTimes)/operatedEventsAmount;
    std::cout << " ,sigm=: " << standardDeviation(operatedEventsAmount, operationTimes) << "s" << std::endl;
//    std::cout << "czas przebywania w kolejce: av=" << sumTime(operatedEventsAmount, queueTimes)/operatedEventsAmount;
//    std::cout << " ,sigm=: " << standardDeviation(operatedEventsAmount, queueTimes) << "s" << std::endl;
}

void getAverageQueueTime(std::function< int() > getEventOperationTime, int N){
    double* operationTimes = new double[N];
    double* queueTimes = new double[N];
    int operatedEventsAmount = 0;
    int missed = 0;
    int allCars = 0;
    double operationTimeLeft = 0;
    double nextEventTime = 0;

    for (int i = 0; operatedEventsAmount < N; i++){
        double timeDiff = nextEventTime < operationTimeLeft || !operationTimeLeft ? nextEventTime: operationTimeLeft;
        operationTimeLeft -= timeDiff;
        operationTimeLeft = std::max(operationTimeLeft, 0.0);
        nextEventTime -= timeDiff;
        if (operationTimeLeft == 0 && !cars.empty()){
            Car car = cars.front();
            operationTimes[operatedEventsAmount] = car.time;
            queueTimes[operatedEventsAmount] = car.queueTime;
            cars.pop();
            operatedEventsAmount++;
            operationTimeLeft = getEventOperationTime();
            std::queue < Car > eventsCopy;
            bool first = true;
            while (!cars.empty())
            {
                Car car = cars.front();
                if (first){
                    first = false;
                } else {
                    car.queueTime += operationTimeLeft;
                }
                car.time += operationTimeLeft;
                eventsCopy.push(car);
                cars.pop();
            }
            cars = eventsCopy;
            printState(operatedEventsAmount, allCars, missed, operationTimes, queueTimes, car.id, "zakończył");
        }
        if (nextEventTime == 0){
            allCars++;
            if (cars.size() < 3){
                int queueTime = operationTimeLeft;
                if (operationTimeLeft == 0){
                    operationTimeLeft = getEventOperationTime();
                }
                Car car(operationTimeLeft, queueTime, allCars);
                cars.push(car);
                printState(operatedEventsAmount, allCars, missed, operationTimes, queueTimes, allCars, "wchodzi");
            } else {
                missed++;
                printState(operatedEventsAmount, allCars, missed, operationTimes, queueTimes, allCars, "uciekł");
            }
            nextEventTime = exponentialRand(1/120.0);
        }
    }
    printState(operatedEventsAmount, allCars, missed, operationTimes, queueTimes, allCars, "uciekł");
}

int main(){
    auto exponentialOperationTime = []{return exponentialRand(1/60.0);};
    int N = 10000000;
    getAverageQueueTime(exponentialOperationTime, N);
    return 0;
}