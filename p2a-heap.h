// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef P2AHEAP_H
#define P2AHEAP_H

#include <getopt.h>
#include <iostream>
#include "planet.h"

// for movie watcher mode:
    // remember best jedis time and force
    // remember best siths time and force
    // remember maybe jedis time and force
    // remember which state

class Galaxy {
private:
    bool modeVerb = false; // verbose toggle
    bool modeMed = false; // running median toggle
    bool modeGen = false; // general-eval toggle
    bool modeWatch = false; // watcher toggle
    
    uint32_t numPlans; // number of planets
    std::vector<Planet> planets;
    
    uint32_t numGens; // number of generals
    std::vector<General> generals;

    bool modeDL; // true if deployment list, false if pseudorandom
    uint32_t numBattles = 0;

    // only for pseudorandom input
    int randomSeed;
    uint32_t numDeployments;
    uint32_t arrivalRate;
    //
    
    uint16_t JediID = 0; // to break ties
    uint16_t SithID = 0; // to break ties

    // designed to be put in a while loop to read input before each round of simulations
    // returns false if at the end of the input file
    bool readInputDL(uint16_t &prevTime) {
        // read input
        uint16_t timestamp;
        if(!(std::cin >> timestamp)) return false;
        std::string inputString;
        DepType side; char inputChar; uint16_t numForce, numTroops; 
        int intNumGen, intNumPlan;
        std::cin >> inputString >> inputChar >> intNumGen >> inputChar >> intNumPlan 
                                >> inputChar >> numForce >> inputChar >> numTroops;
        if(inputString == "JEDI") { side = DepType::Jedi; }            
        else { side = DepType::Sith; }
        //

        if(intNumPlan < 0 || intNumGen < 0) {
            std::cerr << "negative planet or general number given\n";
            exit(1);
        }

        uint16_t numPlan = static_cast<uint16_t>(intNumPlan);
        uint16_t numGen = static_cast<uint16_t>(intNumGen);

        // check that input is possible
        if(numPlan >= numPlans) {
            std::cerr << "invalid planet number\n";
            exit(1);
        }
        else if (numGen >= numGens) {
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
        //

        // create troop with given characteristics and push to correct queue
        if(side == DepType::Jedi) {
            Deployment dep(JediID++, numGen, side, numForce, numTroops);
            planets[numPlan].jedi.push(dep);
        }
        else {
            Deployment dep(SithID++, numGen, side, numForce, numTroops);
            planets[numPlan].sith.push(dep);
            
        }
        generals[numGen].totalTroops += numTroops;
        generals[numGen].aliveTroops += numTroops;
        prevTime = timestamp;
        return true;
    }

    // called by runSim if mode is DL
    void runSimDL() {
        uint16_t prevTime = 0;
        while(readInputDL(prevTime)) {
            
        }
    }

    // called by runSim if mode is PR
    void runSimPR() {

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
        if(input == "DL") { modeDL = true; }
        else { modeDL = false; }

        std::cin >> input >> numGens >> input >> numPlans;

        // get info needed to PR mode if in that mode
        if(!modeDL) {
            std::cin >> input >> randomSeed >> input >> numDeployments >> 
                                                    input >> arrivalRate;
        }

        std::cout << "Deploying troops...\n";
        planets.resize(numPlans);
        generals.resize(numGens);
    } // getMode

    void runSim() {
        if(modeDL) {
            runSimDL();
        }
        else {
            runSimPR();
        }
    }
};

#endif