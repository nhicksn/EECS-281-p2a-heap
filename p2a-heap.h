// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef P2AHEAP_H
#define P2AHEAP_H

#include <getopt.h>
#include <iostream>
#include "planet.h"

// ID Number to break ties in the deployment

// for movie watcher mode:
    // remember best jedis time and force
    // remember best siths time and force
    // remember maybe jedis time and force
    // remember which state

class Galaxy {
private:
    // verbose toggle
    bool modeVerb = false;
    // running median toggle
    bool modeMed = false;
    // general-eval toggle
    bool modeGen = false;
    // watcher toggle
    bool modeWatch = false;
    // number of planets
    uint16_t numPlans;
    std::vector<Planet> planets;
    // number of generals
    uint16_t numGens;
    std::vector<General> generals;
    // true if deployment list, false if pseudorandom
    bool modeDP;
    uint16_t numBattles = 0;
    // only for pseudorandom input
    int randomSeed;
    uint16_t numDeployments;
    uint16_t arrivalRate;

    // designed to be put in a while loop to read input before each round of simulations
    // returns false if at the end of the input file
    bool readInputDL(const uint16_t &prevTime) {
        // read input
        std::string inputString;
        if(!(std::cin >> inputString)) return false;
        DepType side;
        char inputChar;
        uint16_t timestamp, numGen, numPlan, numForce, numTroops;
        std::cin >> inputChar >> numGen >> inputChar >> numPlan >> inputChar 
                                                    >> numForce >> inputChar >> numTroops;
        if(inputString == "JEDI") { side = DepType::Jedi; }            
        else { side = DepType::Sith; }
        // check that input is possible
        if(numPlan > numPlans || numPlan < 0) {
            std::cerr << "invalid planet number\n";
            exit(1);
        }
        else if (numGen > numGens || numGen < 0) {
            std::cerr << "invalid general number\n";
            exit(1);
        }
        else if(numForce <= 0 || numTroops <= 0) {
            std::cerr << "invalid force sensitivity or troop quantity\n";
            exit(1);
        }
        else if(timestamp < prevTime) {
            std::cerr << "invalid timestamp\n";
            exit(1);
        }
        // create troop with given characteristics and push to correct queue
        Deployment troop(timestamp, numGen, side, numForce, numTroops);
        if(side == DepType::Jedi) planets[numPlan].jedi.push(troop);
        else planets[numPlan].sith.push(troop);
        return true;
    }

    void runSim() {
        uint16_t prevTime = 0;
        while(readInputDL(prevTime)) {
            
        }
    }

public:
    void getMode(const int &argc, char *argv[]) {
        opterr = false;
        int choice;
        int index = 0;
        option long_options[] = {
            { "verbose",      no_argument, nullptr, 'v' },
            { "median",       no_argument, nullptr, 'm' },
            { "general-eval", no_argument, nullptr, 'g' },
            { "watcher",      no_argument, nullptr, 'w' },
            { nullptr,        0,           nullptr, '\0'},
        }; // long_options

        while((choice = getopt_long(argc, argv, "vmgw", long_options, &index)) != -1) {
            switch(choice) {
                case 'v': {
                    modeVerb = true;
                    break;
                } // case 'v'
                
                case 'm' : {
                    modeMed = true;
                    break;
                } // case 'm'

                case 'g': {
                    modeGen = true;
                    break;
                } // case 'g'
                
                case 'w': {
                    modeWatch = true;
                    break;
                } // case 'w'

                default: {
                    std::cerr << "Unknown command line option\n";
                    exit(1);
                } // default case
            } // switch choice
        } // while choice

        //get input info
        std::string input;
        std::getline(std::cin, input);
        std::cin >> input >> input;
        // check input mode
        if(input == "DL") { modeDP = true; }
        else { modeDP = false; }
        std::cin >> input >> numGens >> input >> numPlans;
        if(!modeDP) {
            std::cin >> input >> randomSeed >> input >> numDeployments >> 
                                                    input >> arrivalRate;
        }
        std::cout << "Deploying troops...\n";
        planets.resize(numPlans);
        generals.resize(numGens);
    } // getMode

};

#endif