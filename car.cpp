#include <unistd.h>
#include "car.h"
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <mutex>

Gates::Gates()
{
    northOpen = true;
    southOpen = true;
}

Car::Car(Gates* _gates, int _id, int _track_number, int _speed, int _laps_to_go, int _color, int _symbol, int start_position)
{
    gates = _gates;
    id = _id;
    track_number = _track_number;
    speed = _speed;
    laps_to_go = _laps_to_go;
    color = _color;
    symbol = _symbol;
    position = start_position*100;
}

Car::Car()
{
    id = -1;
    track_number = -1;
    speed = -1;
    laps_to_go = -1;
    color = -1;
    symbol = -1;
    position = 0;
}

void Car::reset(int _id, int _track_number, int _speed, int _laps_to_go, int _color, int _symbol, int start_position)
{
    id = _id;
    track_number = _track_number;
    speed = _speed;
    laps_to_go = _laps_to_go;
    color = _color;
    symbol = _symbol;
    position = start_position*100;
    prev_pos = position;
}

void Car::run()
{
    while (!stop)
    {
        prev_pos = position;
        usleep(10.0/(speed)*1000000);
        position += 10;
        int boardPosition = (position/100)%76;
        
        //pass the finish line
        if (boardPosition>=34 && (prev_pos/100)%76<34)
        {
            laps_to_go--;
        }
        //pass the exit
        else if(laps_to_go == 0 && boardPosition>=69)
        {
            visible = false;
            stop = true;
        }

        //pass gates/crossroads
        if (track_number == 1)//top car
        {
            switch (boardPosition)
            {
            //north close
            case 57:
                {
                    std::lock_guard<std::mutex> lk(gates->northMutex);
                    gates->northOpen = false;
                }
                gates->northCV.notify_all();
                break;
            //south close
            case 19:
                {
                    std::lock_guard<std::mutex> lk(gates->southMutex);
                    gates->southOpen = false;
                }
                gates->southCV.notify_all();    
                break;
            //north open
            case 10:
                {
                    std::lock_guard<std::mutex> lk(gates->northMutex);
                    gates->northOpen = true;
                }
                gates->northCV.notify_all();
                break;
            //south open
            case 48:
                {
                    std::lock_guard<std::mutex> lk(gates->southMutex);
                    gates->southOpen = true;
                }
                gates->southCV.notify_all();
                break;
            
            default:
                break;
            }
        }
        else
        {
            switch (boardPosition)
            {
            //north wait
            case 9:
                {
                    std::unique_lock<std::mutex> lk(gates->northMutex);
                    gates->northCV.wait(lk, [this]{return this->gates->northOpen;});
                }
                break;
            //south wait
            case 47:
                {
                    std::unique_lock<std::mutex> lk(gates->southMutex);
                    gates->southCV.wait(lk, [this]{return this->gates->southOpen;});
                }
            default:
                break;
            }
        }
        
    }
}

int Car::getPosition()
{
    return position/100;
}

int Car::getTrack()
{
    return track_number;
}

int Car::getSymbol()
{
    return symbol;
}

int Car::getColor()
{
    return color;
}

int Car::getSpd()
{
    return speed;
}

int Car::getId()
{
    return id;
}

int Car::getLaps()
{
    return laps_to_go;
}
