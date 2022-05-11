#include <ncurses.h>
#include <unistd.h>
#include "gui.h"
#include "main.h"
#include "car.h"
#include <iostream>

WINDOW * cars_list;

void rect(int y_start, int x_start, int width, int height){

    mvvline(y_start, x_start, 0, height); //LEFT LINE
    mvvline(y_start, x_start + width-1, 0, height); //RIGHT LINE

    mvhline(y_start, x_start, 0, width); //TOP LINE
    mvhline(y_start + height-1, x_start, 0, width); //BOTTOM LINE
    
    mvprintw(y_start, x_start, "+");
    mvprintw(y_start + height-1, x_start, "+");
    mvprintw(y_start, x_start + width-1, "+");
    mvprintw(y_start + height-1, x_start + width-1, "+");
}

void coloredText(int y, int x, const char * text, int color_pair_number, WINDOW* win){
    wattron(win, COLOR_PAIR(color_pair_number));
    mvwprintw(win, y, x, text);
    wattroff(win, COLOR_PAIR(color_pair_number));

}

void drawLowerRaceTrack(){
    attron(A_BOLD);
    rect(12, 35, 30, 10); //BOTTOM RACE TRACK
    attroff(A_BOLD);
}

void drawTopRaceTrack(){
    attron(A_BOLD);
    rect(2, 45, 10, 30); //TOP RACE TRACK
    attroff(A_BOLD);
}

void drawBackground(){
    attron(COLOR_PAIR(1));
    mvprintw(0, 0, "~ Welcome to the GRAND RACE ~");
    attroff(COLOR_PAIR(1));

    attron(A_BOLD);
    //FINISH LINE
    mvprintw(28, 47, "FINISH");
    mvprintw(29, 48, "LINE");
    mvprintw(30, 50, "v");
    mvprintw(32, 50, "^");
    attroff(A_BOLD);

    coloredText(0, 53, "~IN~", 20, stdscr);
    coloredText(1, 54, "v", 20, stdscr);

    coloredText(0, 43, "~OUT~", 21, stdscr);
    coloredText(1, 45, "^", 21, stdscr);
}

void initGUI()
{
    initscr();
    
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_YELLOW);
        init_pair(2, COLOR_BLACK, COLOR_RED);
        init_pair(3, COLOR_WHITE, COLOR_GREEN);
        init_pair(4, COLOR_YELLOW, COLOR_BLUE);
        init_pair(5, COLOR_BLACK, COLOR_CYAN);
        init_pair(6, COLOR_GREEN, COLOR_MAGENTA);
        init_pair(7, COLOR_BLACK, COLOR_WHITE);
        init_pair(8, COLOR_CYAN, COLOR_BLACK);
        init_pair(9, COLOR_WHITE, COLOR_BLUE);

        init_pair(20, COLOR_GREEN, COLOR_BLACK);
        init_pair(21, COLOR_RED, COLOR_BLACK);
    }
    drawBackground();

    drawLowerRaceTrack();
    drawTopRaceTrack();
    refresh();
}


void drawCar(Car& c, SimData& sd){
    if (c.getTrack() == 0)
    {
        const int TLy=12, TLx=35, w=30, h=10;
        int y=TLy, x=TLx;
        int pos = c.getPosition();
        pos %= (2*w + 2*h - 4);
        if (pos<w)//top 
        {
            x += pos;
        }
        else if (pos<w+h-1)//right
        {
            x += w - 1;
            y += pos-w+1;
        }
        else if (pos<2*w+h-2)//down
        {
            x += w - (pos-h-w) - 3;
            y += h - 1;
        }
        else//left
        {
            y += h - (pos - 2*w - h) - 4;
        }
     
        coloredText(y, x, sd.carSymbols[c.getSymbol()], c.getColor(), stdscr);
    }
    else if (c.getTrack() == 1)
    {
        const int TRx = 54, TRy = 2, w = 10, h = 30;
        int y = TRy, x = TRx;
        int pos = c.getPosition();
        if (pos < 0){
            y += pos;
            coloredText(y, x, sd.carSymbols[c.getSymbol()], c.getColor(), stdscr);
            return;
        }
        pos %= (2*w + 2*h - 4);
        if (pos < h) // right
        {
            y+=pos;
        }
        else if (pos < h + w - 1) // down
        {
            y += h - 1;
            x -= pos - h + 1;
        }
        else if (pos < 2*h + w - 2 || c.getLaps()==0) // left
        {
            x -= w-1;
            y += h - (pos - h - w) - 3;
        }
        else // up
        {
            x -= w - (pos - 2*h - w) - 4;
        }

        coloredText(y, x, sd.carSymbols[c.getSymbol()], c.getColor(), stdscr);
    }
    
}

void putCarOnLeaderboard(int posOnBoard, Car& c, SimData& sd){
    if(c.getTrack() == 0){
        int y = 5+posOnBoard, x = 1;
        coloredText(y, x, sd.carSymbols[c.getSymbol()], c.getColor(), cars_list);
        mvwprintw(cars_list, y, 5, "%3d", c.getSpd());
        mvwprintw(cars_list, y, 11, "%3d", c.getId());
        mvwprintw(cars_list, y, 16, "%3d", c.getPosition());
        mvwprintw(cars_list, y, 21, "--");
    }
    else
    {
        int y = 10+c.getId()-3, x = 1;
        coloredText(y, x, sd.carSymbols[c.getSymbol()], c.getColor(), cars_list);
        mvwprintw(cars_list, y, 5, "%3d", c.getSpd());
        mvwprintw(cars_list, y, 11, "%3d", c.getId());
        mvwprintw(cars_list, y, 16, "%3d", c.getPosition());
        mvwprintw(cars_list, y, 21, "%2d", c.getLaps());
    }
}
void runGUI(SimData& sd){

    initGUI();
    noecho();
    timeout(0);

    while (true)
    {    
        erase();
        //draw background
        drawBackground();
        //draw lower track
        drawLowerRaceTrack();
        drawTopRaceTrack();
        // Gates testGates;
        // Car testCar(&testGates, 200, 0, 0, 0, 1, 1, 47);
        // drawCar(testCar, sd);

        for (int i = 0; i < MAX_LOWER_CARS; i++)
        {
            //get lower cars data
            Car* c = sd.lowerTrackCars[i];
            // draw lower cars
            drawCar(*c, sd);
        }
        

        for (Car* c : sd.upperTrackCars)
        {
            //draw top cars   
            if (c->visible) 
                drawCar(*c, sd);
        }
        refresh();
        usleep(50000);
        //nonblocking getch returns ERR if there was no input 
        char c = getch();
        if(c == 'q')break;

    }
    echo();
    endwin();
    return;
}

