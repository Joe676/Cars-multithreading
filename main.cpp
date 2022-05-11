#include "main.h"
#include "car.h"
#include "gui.h"
#include "factory.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <ncurses.h>
#include <iostream>

SimData::SimData()
{
    upperTrackCars = std::vector<Car*>();
}

/*
Etap 1 - Pojazdy jezdza po krzyzujacych sie torach. 
    Po dolnym torze jezdzi stala ich liczba(3) ze stala, jednakowa predkoscia.
    Na gorny tor wjezdzaja co losowy czas pojazdy, ktore po przejechaniu trzech okrazen zjezdzaja z toru.
    Pojazdow na gornym torze moze byc wirtualnie nieograniczona liczba.

Etap 2 - Pojazdy na stalym torze zatrzymuja sie warunkowo przed pierwszym skrzyzowaniem w prostej (lewe gorne i prawe dolne skrzyzowanie).
    Gdy pojazdy na dynamicznym torze przejezdzaja przez skrzyzowanie (lewe gorne lub prawe dolne) blokuja je tak, ze pojazdy ze stalego toru nie moga przez nie przejechac.
    Gdy przejezdzaja przez kolejne skrzyzowanie odblokowuja odpowiednia blokade.
    Sprowadza sie to do tego:
    (przyklad na "poludniowym" zestawie)
        - pojazd dynamiczny podjezdza do prawego skrzyzowania i blokuje je;
        - pojazd staly podjezdza do prawego skrzyzowania i zatrzymuje sie;
        - gdy dowolny dynamiczny pojazd przejedzie przez skrzyzowanie lewe pojazd statyczny moze sie ruszyc;

                 |      v blokada                                          
            ------------------
    odblokowanie ^      |
                 |      |
                 --------

Etap 3 - Pojazdy oczekujace wypuszczane sa po kolei, jeden za drugim w sekcji krytycznej
*/

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    SimData simData;
    Gates gates;
    
    CarFactory carFactory(std::ref(simData), &gates, 1, 7, 300, 600, 3, 9, NUM_OF_SYMBOLS, 0);

    for (int i = 0; i < MAX_LOWER_CARS; i++)
    {
        simData.lowerTrackCars.push_back(carFactory.makeCar(0));
    }

    std::vector<std::thread*> lower_car_threads;
    std::vector<std::thread*> upper_car_threads;
    for (int i = 0; i < MAX_LOWER_CARS; i++)
    {
        lower_car_threads.push_back(new std::thread(&Car::run, simData.lowerTrackCars[i]));
    }
    std::thread gui_thread(runGUI, std::ref(simData));
    std::thread factory_thread(&CarFactory::run, &carFactory, std::ref(simData.upperTrackCars), std::ref(upper_car_threads));

    gui_thread.join();
    carFactory.stop = true;
    factory_thread.join();

    for (int i = upper_car_threads.size()-1; i >= 0; i--)
    {
        simData.upperTrackCars[i]->stop = true;
    }
    for (int i = upper_car_threads.size()-1; i >= 0; i--)
    {
        upper_car_threads[i]->join();
        delete simData.upperTrackCars[i];
        simData.upperTrackCars.erase(simData.upperTrackCars.begin() + i);
        delete upper_car_threads[i];
        upper_car_threads.erase(upper_car_threads.begin() + i);
    }

    for (int i = MAX_LOWER_CARS-1; i >= 0; i--)
    {
        simData.lowerTrackCars[i]->stop = true;
    }
    //wake up any waiting cars
    {
        std::lock_guard<std::mutex> lk(gates.southMutex);
        gates.southOpen = true;
        gates.southCV.notify_all();
    }
    {
        std::lock_guard<std::mutex> lk(gates.northMutex);
        gates.northOpen = true;
        gates.northCV.notify_all();
    }
    for (int i = MAX_LOWER_CARS-1; i >= 0; i--)
    {
        lower_car_threads[i]->join();
        delete simData.lowerTrackCars[i];
        simData.lowerTrackCars.erase(simData.lowerTrackCars.begin() + i);
        delete lower_car_threads[i];
        lower_car_threads.erase(lower_car_threads.begin() + i);
    }

    return 0;
}
