// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef p2a-heap.h
#define p2a-heap.h

#include <getopt.h>
#include <iostream>
#include <queue>
#include "planet.h"

// ID Number to break ties in the deployment

// for movie watcher mode:
    // remember best jedis time and force
    // remember best siths time and force
    // remember maybe jedis time and force
    // remember which state

enum class State { Initial, SeenOne, SeenBoth };

enum class DepType { Jedi, Sith };

struct Deployment {
    // might not need this
    uint16_t timeStamp;
    uint16_t genID;
    DepType side;
    uint16_t forceSens;
    // might not need this
    uint16_t quantity;
};

class Galaxy {
private:
    bool modeVerb = false;
    bool modeMed = false;
    bool modeGen = false;
    bool modeWatch = false;
    std::vector<Planet> planets;
    // dont need general class, 
    std::vector<uint16_t> generals;
    // true if deployment list, false if pseudorandom
    bool modeDP;

public:
    void getMode(const int &argc, char *argv[]) {
        opterr = false;
        int choice;
        int index = 0;
        option long_options[] = {
            { "verbose",      no_argument,       nullptr, 'v' },
            { "median",       no_argument,       nullptr, 'm' },
            { "general-eval", no_argument,       nullptr, 'g' },
            { "watcher",      no_argument,       nullptr, 'w' },
            { nullptr,        0,                 nullptr, '\0'},
        }; // long_options

        while((choice = getopt_long(argc, argv, "vmgw", long_options, &index)) != -1) {
            switch(choice) {
                case 'v': {
                    modeVerb = true;
                } // case 'v'
                
                case 'm' : {
                    modeMed = true;
                } // case 'm'

                case 'g': {
                    modeGen = true;
                } // case 'g'
                
                case 'w': {
                    modeWatch = true;
                } // case 'w'

                default: {
                    std::cerr << "Unknown command line option\n";
                    exit(1);
                } // default case
            } // switch choice
        } // while choice
    } // getMode

};

#endif