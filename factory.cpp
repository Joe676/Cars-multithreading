#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <thread>
#include "car.h"
#include "main.h"
#include "factory.h"

 
CarFactory::CarFactory(SimData& sd, Gates* _gates, int minTime, int maxTime, int _minSpeed, int _maxSpeed, int laps, int _maxColor, int _numOfSymbols, int startPos)
{
    simData = sd;
    gates = _gates;

    minTimeBetweenCars = minTime*4;
    maxTimeBetweenCars = maxTime*4;
    minSpeed = _minSpeed;
    maxSpeed = _maxSpeed;
    lapsToGo = laps;
    maxColor = _maxColor;
    numOfSymbols = _numOfSymbols;
    startPosition = startPos;
    nextId = 0;
}

Car* CarFactory::makeCar(int track)
{
    int speed = (maxSpeed+minSpeed)/2;    
    if(track==1) speed = rand()%(maxSpeed-minSpeed+1)+minSpeed;
    int color = rand()%(maxColor)+1;
    int symbol = rand()%numOfSymbols;
    int pos = (track==0?startPosition+rand()%76:-3);
    return new Car(gates, nextId++, track, speed, (track==0?-1:lapsToGo), color, symbol, pos);
    // return out;
}

void CarFactory::run(std::vector<Car*> &upper_cars, std::vector<std::thread*> &upper_threads)
{
    int newCarTimer = 0;

    while (!stop)
    {
        //Clearing stopped cars
        for (int i = upper_cars.size()-1; i >= 0 ; i--)
        {
            if (upper_cars[i]->stop)
            {
                upper_threads[i]->join();
                upper_threads.erase(upper_threads.begin() + i);

                delete upper_cars[i];
                upper_cars.erase(upper_cars.begin() + i);
            }
        }

        //Make new car when the time comes
        if(newCarTimer <= 0){
            upper_cars.push_back(makeCar(1)); //Make object
            upper_threads.push_back(new std::thread(&Car::run, upper_cars[upper_cars.size()-1])); //Make thread running object
            newCarTimer = rand()%(maxTimeBetweenCars-minTimeBetweenCars+1) + minTimeBetweenCars; //Get new timer
        }
        
        //Sleed 1/4 second, update timer
        usleep(250000);
        newCarTimer--;
    }
    
}
