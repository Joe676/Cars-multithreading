#ifndef MAIN_H
#define MAIN_H
#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include "car.h"

#define MAX_LOWER_CARS 3
#define MAX_UPPER_CARS 5
#define NUM_OF_SYMBOLS 11

class SimData
{
public:
    const char* carSymbols[NUM_OF_SYMBOLS] = {"!", "@", "#", "$", "%%", "^", "&", "*", "/", "?", "~"};

    std::vector<Car*> lowerTrackCars;
    std::vector<Car*> upperTrackCars;
    bool stop = false;


    SimData();
};

#endif