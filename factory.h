#ifndef FACTORY_H
#define FACTORY_H
#pragma once
#include <vector>
#include <thread>
#include "car.h"

class CarFactory
{
private:
    int minTimeBetweenCars;
    int maxTimeBetweenCars;
    int minSpeed;
    int maxSpeed;
    int lapsToGo;
    int maxColor;
    int numOfSymbols;

    int startPosition;
    
    int nextId;

    SimData simData;
    Gates* gates;

public:
    bool stop = false;
    CarFactory(SimData& sd, Gates* _gates, int minTime, int maxTime, int _minSpeed, int _maxSpeed, int laps, int _maxColor, int _numOfSymbols, int startPos);

    void run(std::vector<Car*> &upper_cars, std::vector<std::thread*> &upper_threads);
    Car* makeCar(int track);
};

#endif