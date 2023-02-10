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
    
    uint32_t JediID = 0; // to break ties
    uint32_t SithID = 0; // to break ties

    // designed to be put in a while loop to read input before each round of simulations
    // returns false if at the end of the input file
    bool readInputDL(uint32_t &prevTime) {
        // read input
        uint32_t timestamp;
        if(!(std::cin >> timestamp)) return false;
        std::string inputString;
        DepType side; char inputChar; uint32_t numForce, numTroops; 
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

        uint32_t numPlan = static_cast<uint32_t>(intNumPlan);
        uint32_t numGen = static_cast<uint32_t>(intNumGen);

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
            if(modeGen) { 
                generals[numGen].totalJedi += numTroops;
                generals[numGen].totalAlive += numTroops;
            }
        }
        else {
            Deployment dep(SithID++, numGen, side, numForce, numTroops);
            planets[numPlan].sith.push(dep);
            if(modeGen) {
                generals[numGen].totalSith += numTroops;
                generals[numGen].totalAlive += numTroops;
            }
        }
        prevTime = timestamp;
        return true;
    }

    // called by runSim if mode is DL // TODO:
    void runSimDL() {
        uint32_t prevTime = 0;
        uint32_t currentTime = 0;
        while(readInputDL(prevTime)) {
            if(modeMed && (currentTime != prevTime)) {
                // print median information
                currentTime = prevTime; // updates currentTime
            }
            for(size_t i = 0; i < planets.size(); i++) {
                while(validBattle(i)) {
                    uint32_t numTroopsJedi = planets[i].jedi.top().quantity;
                    uint32_t numTroopsSith = planets[i].sith.top().quantity;
                    if(numTroopsJedi > numTroopsSith) {
                        if(modeVerb) {
                            printVerbose(planets[i].sith.top().genID, 
                            planets[i].jedi.top().genID, i, numTroopsSith * 2);
                        }
                        if(modeGen) {
                            generals[planets[i].jedi.top().genID].totalAlive -= numTroopsSith;
                            generals[planets[i].sith.top().genID].totalAlive -= numTroopsSith;
                        }
                        planets[i].sith.pop();
                        Deployment temp = planets[i].jedi.top();
                        temp.quantity -= numTroopsSith;
                        planets[i].jedi.pop();
                        planets[i].jedi.push(temp);
                    }
                    else if(numTroopsJedi < numTroopsSith) {
                        if(modeVerb) {
                            printVerbose(planets[i].sith.top().genID, 
                            planets[i].jedi.top().genID, i, numTroopsJedi * 2);
                        }
                        if(modeGen) {
                            generals[planets[i].jedi.top().genID].totalAlive -= numTroopsJedi;
                            generals[planets[i].sith.top().genID].totalAlive -= numTroopsJedi;
                        }
                        planets[i].jedi.pop();
                        Deployment temp = planets[i].sith.top();
                        temp.quantity -= numTroopsJedi;
                        planets[i].sith.pop();
                        planets[i].sith.push(temp);
                    }
                    else {
                        if(modeVerb) {
                            printVerbose(planets[i].sith.top().genID, 
                            planets[i].jedi.top().genID, i, numTroopsJedi * 2);
                        }
                        if(modeGen) {
                            generals[planets[i].jedi.top().genID].totalAlive -= numTroopsJedi;
                            generals[planets[i].sith.top().genID].totalAlive -= numTroopsJedi;
                        }
                        planets[i].jedi.pop();
                        planets[i].sith.pop();
                    }
                    numBattles++;
                }
            }
        }
    }

    // called by runSim if mode is PR // TODO:
    void runSimPR() {

    }

    // called by runSimDL to check if there is a valid battle
    bool validBattle(size_t i) {
        // makes sure that neither pq is empty
        if(planets[i].sith.empty() || planets[i].jedi.empty()) {
            return false;
        }
        if(planets[i].sith.top().forceSens >= planets[i].jedi.top().forceSens) {
            return true;
        }
        return false;
    }

    void printVerbose(uint32_t gen1, uint32_t gen2, size_t plan, uint32_t troops) {
        std::cout << "General " << gen1 << "'s battalion attacked General " << gen2 << 
            "'s battalion on planet " << plan << ". " << troops << " troops were lost.\n";
    }

    // used by runSim if modeMed is enabled // TODO:
    void printMedian() {

    }

    // used by runSim if modeGen is enabled // TODO:
    void printGen() {
        std::cout << "---General Evaluation---\n";
        for(size_t i = 0; i < numGens; i++) {
            uint32_t totalTroops = generals[i].totalJedi + generals[i].totalSith;
            std::cout << "General " << i << " deployed " << generals[i].totalJedi << 
                   " Jedi troops and " << generals[i].totalSith << " Sith troops, and " << 
                   generals[i].totalAlive << "/" << totalTroops << " troops survived.\n";
        }
    }

    // used by runSim if modeWatch is enabled // TODO:
    void printWatch() {

    }

    void printSummary() {
        std::cout << "---End of Day---\n" << "Battles: " << numBattles << '\n';
        if(modeGen) {
            printGen();
        }
    }
    
public:
    
    Galaxy(const int &argc, char *argv[]) {
        getMode(argc, argv);
    }

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
        if(modeGen) { generals.resize(numGens); }
        
    } // getMode

    void runSim() {
        if(modeDL) {
            runSimDL();
        }
        else {
            runSimPR();
        }
        printSummary();
    }
};

#endif