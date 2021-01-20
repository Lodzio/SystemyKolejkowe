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

#include <unistd.h>

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

int sumTime(int maxN, int operationTimes[]){
    int result = 0;
    for (int i = 0; i < maxN; i++){
        result += operationTimes[i];
    }
    return result;
}

int standardDeviation(int maxN, int operationTimes[]){
    unsigned long int result = 0;
    const double averageTime = sumTime(maxN, operationTimes)/(double)maxN;
    for (int i = 0; i < maxN; i++){
        result += pow(operationTimes[i] - averageTime, 2);
    }
    result /= maxN;
    result = sqrt(result);
    return result;
}

class FIFO {
    const int queueSize;
    std::queue < int > tasks;
    const std::function< int() > getEventOperationTime;
    int operationTimeLeft = 0; // in milis
    int actualEventTimeSpendInQueue = 0;

    public:
    FIFO(int queueSize, std::function< int() > getEventOperationTime): queueSize(queueSize), getEventOperationTime(getEventOperationTime){
    }

    bool hasSpaceInQueue(){
        return tasks.size() < queueSize;
    }

    int getOperationTimeLeft(){
        return operationTimeLeft;
    }

    void increaseTaskTimes(){
        std::queue < int > tasksCopy;
        while (!tasks.empty())
        {
            tasksCopy.push(tasks.front() + operationTimeLeft);
            tasks.pop();
        }
        tasks = tasksCopy;
    }

    int moveOnTimeLine(int timeDiff){
        operationTimeLeft -= timeDiff;
        operationTimeLeft = std::max(operationTimeLeft, 0);
        if (operationTimeLeft == 0 && tasks.size()){
            int timeSpentInQueue = tasks.front();
            operationTimeLeft = getEventOperationTime();
            tasks.pop();
            increaseTaskTimes();
            return timeSpentInQueue;
        }
        return -1;
    }

    void pushNewTask(int task){
        if (!hasSpaceInQueue()){
            std::cout << "error in pushNewTask" << std::endl;
        }
        if (operationTimeLeft == 0){
            operationTimeLeft = getEventOperationTime();
        } else {
            tasks.push(task);
        }
    }
};

int calculateTimeDiff(int operationTimeLeft1, int nextEventTime){
    int result = nextEventTime;
    result = std::min(result, operationTimeLeft1);
    return result == 0? nextEventTime:result;
}

void runFifo(int N){
    int* operationTimes = new int[N];
    int operatedEventsAmount = 0;
    int missed = 0;
    int allTasks = 0;
    int nextEventTime = 0;

    FIFO fifo1(100, []{return 60;});
    for (int i = 0; operatedEventsAmount < N; i++){
        int timeDiff = calculateTimeDiff(fifo1.getOperationTimeLeft(), nextEventTime);
        nextEventTime -= timeDiff;
        int operationTime1 = fifo1.moveOnTimeLine(timeDiff);
        if (operationTime1 != -1){
            operationTimes[operatedEventsAmount] = operationTime1;
            operatedEventsAmount++;
        }
        if (nextEventTime == 0){
            allTasks++;
            if (fifo1.hasSpaceInQueue()){
                fifo1.pushNewTask(0);
            } else {
                missed++;
            }
            nextEventTime = exponentialRand(1 / 120.0);
        }
    }
    std::cout << "klienci obsłużeni: " << (allTasks-missed)*100.0/allTasks << "%" << std::endl;
    std::cout << "%czas przebywania w systemie: av=" << sumTime(operatedEventsAmount, operationTimes)/operatedEventsAmount << "ms";
    std::cout << " ,sigm=: " << standardDeviation(operatedEventsAmount, operationTimes) << "ms" << std::endl;}

int main(){
    int N = 1000000;
    runFifo(N);
    return 0;
}