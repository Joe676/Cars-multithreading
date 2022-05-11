#ifndef CAR_H
#define CAR_H
#pragma once

#include <mutex>
#include <vector>
#include <condition_variable>
#include <thread>


class Gates
{
    public:
    std::mutex northMutex;
    std::condition_variable northCV;
    bool northOpen;

    std::mutex southMutex;
    std::condition_variable southCV;
    bool southOpen;

    Gates();
};

class Car
{
public:
    int id;
    int track_number;
    int speed;
    int position;
    int prev_pos;
    int laps_to_go;

    int color;
    int symbol;

    Gates* gates;

    bool stop = false;
    bool visible = true;
    Car();
    Car(Gates* _gates, int _id, int _track_number, int _speed, int _laps_to_go, int _color, int _symbol, int start_position);
    void reset(int _id, int _track_number, int _speed, int _laps_to_go, int _color, int _symbol, int start_position);
    void run();
    int getPosition();
    int getTrack();
    int getSymbol();
    int getColor();
    int getSpd();
    int getId();
    int getLaps();
    void crossFinish();
    void crossOut();
    bool getNorthOpen();
    bool getSouthOpen();
};

void runCar(std::vector<Car> &carContainer, int carId);
#endif