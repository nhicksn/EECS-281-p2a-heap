// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef P2AHEAP_H
#define P2AHEAP_H

#include <getopt.h>
#include <iostream>
#include <sstream>
#include <queue>
#include <cstdint>
#include "planet.h"
#include "P2random.h"

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

    // vector of priority queues for the median
    std::vector<std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>>> upper;
    std::vector<std::priority_queue<uint32_t, std::vector<uint32_t>, std::less<uint32_t>>> lower;
    //

    // vectors containing best attacks and ambushes
    std::vector<movieFight> ambushes;
    std::vector<movieFight> attacks;
    //

    // to break ties
    uint32_t JediID = 0;
    uint32_t SithID = 0;
    //

    // designed to be put in a while loop to read input before each round of simulations
    // returns false if no more input
    bool readInput(std::istream &input, uint32_t &prevTime, Deployment &depOut) {
        // read input
        uint32_t timestamp;
        if(!(input >> timestamp)) return false;
        std::string inputString;
        DepType side; char inputChar; uint32_t numForce, numTroops; 
        int intNumGen, intNumPlan;
        input >> inputString >> inputChar >> intNumGen >> inputChar >> intNumPlan 
                                >> inputChar >> numForce >> inputChar >> numTroops;
        if(inputString == "JEDI") { side = DepType::Jedi; }            
        else { side = DepType::Sith; }
        // read input

        // check that input is possible
        if(intNumPlan < 0 || intNumGen < 0) {
            std::cerr << "negative planet or general number given\n";
            exit(1);
        }
        uint32_t numPlan = static_cast<uint32_t>(intNumPlan);
        uint32_t numGen = static_cast<uint32_t>(intNumGen);
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
        // check that input is possible

        // create troop with given characteristics and push to correct queue
        if(side == DepType::Jedi) { // if Jedi
            Deployment dep(JediID++, numGen, side, numForce, numTroops);
            planets[numPlan].jedi.push(dep);
            depOut = dep;
            if(modeGen) { 
                generals[numGen].totalJedi += numTroops;
                generals[numGen].totalAlive += numTroops;
            }
        }
        else { // if Sith
            Deployment dep(SithID++, numGen, side, numForce, numTroops);
            planets[numPlan].sith.push(dep);
            depOut = dep;
            if(modeGen) {
                generals[numGen].totalSith += numTroops;
                generals[numGen].totalAlive += numTroops;
            }
        }
        // create troop with given characteristics and push to correct queue

        prevTime = timestamp;
        return true;
    }

    // used by runSim to check if there is a valid battle
    bool validBattle(size_t i) {
        // make sure that neither pq is empty
        if(planets[i].sith.empty() || planets[i].jedi.empty()) {
            return false;
        }
        // make sure there is a sith with higher forceSens than a jedi
        if(planets[i].sith.top().forceSens >= planets[i].jedi.top().forceSens) {
            return true;
        }
        return false;
    }

    // used by runSim if modeVerb is enabled
    void printVerbose(uint32_t gen1, uint32_t gen2, size_t plan, uint32_t troops) {
        std::cout << "General " << gen1 << "'s battalion attacked General " << gen2 << 
            "'s battalion on planet " << plan << ". " << troops << " troops were lost.\n";
    }

    // used by runSim to see if best ambush needs to be updated
    void evaluateAmbush(Deployment &dep) {
        // TODO:
        dep.quantity++;
    }

    // used by runSim to see if best attack needs to be updated
    void evaluateAttack(Deployment &dep) {
        // TODO:
        dep.quantity++;
    }

    // used by runSim if modeMed, prints the median data for the inputted timestamp
    void printMedian(uint32_t time) {
        for(uint32_t i = 0; i < numPlans; i++) {
            if(upper[i].size() > 0 || lower[i].size() > 0) {
                std::cout << "Median troops lost on planet " << i << " at time " << time 
                    << " is " << calcMedian(i) << ".\n";
            }
        }
    }

    // prints all necessary info at the end of the program
    void printSummary() {
        std::cout << "---End of Day---\n" << "Battles: " << numBattles << '\n';
        if(modeGen) {
            printGen();
        }
        if(modeWatch) {
            printWatch();
        }
    }

    // used by printSummary if modeGen is enabled, prints the general data
    void printGen() {
        std::cout << "---General Evaluation---\n";
        for(size_t i = 0; i < numGens; i++) {
            uint32_t totalTroops = generals[i].totalJedi + generals[i].totalSith;
            std::cout << "General " << i << " deployed " << generals[i].totalJedi << 
                   " Jedi troops and " << generals[i].totalSith << " Sith troops, and " << 
                   generals[i].totalAlive << "/" << totalTroops << " troops survived.\n";
        }
    }
    
    // used by printSummary if modeWatch is enabled, prints the movie watcher data
    void printWatch() {
        std::cout << "---Movie Watcher---\n";
        for(uint32_t i = 0; i < numPlans; i++) {
            // check if a good ambush was found on this planet
            if(ambushes[i].timeStamp1 == UINT32_MAX) { 
                std::cout << "A movie watcher would not see an interesting ambush on planet " 
                            << i << '\n';
            }
            //

            // if there was, cout best ambush data for that planet
            else {
                std::cout << "A movie watcher would enjoy an ambush on planet " << i << 
                " with Sith at time " << ambushes[i].timeStamp1 << " and Jedi at time " 
                            << ambushes[i].timeStamp2 << " with a force difference of " 
                                                    << ambushes[i].forceDiff << ".\n";
            }
            //

            // check if a good attack was found on this planet
            if(attacks[i].timeStamp1 == UINT32_MAX) {
                std::cout << "A movie watcher would not see an interesting attack on planet " 
                            << i << '\n';
            }
            //

            // if there was, cout best attack data for that planet
            else {
                std::cout << "A movie watcher would enjoy an attack on planet " << i << 
                " with Jedi at time " << attacks[i].timeStamp1 << " and Sith at time " 
                            << attacks[i].timeStamp2 << " with a force difference of " 
                                                    << attacks[i].forceDiff << ".\n";
            }
            //
        }
    }

    // used for modeMed
    void pqPush(uint32_t planNum, uint32_t numTroops) {
        if(upper[planNum].size() == 0 || numTroops >= upper[planNum].top()) { upper[planNum].push(numTroops); }
        else { lower[planNum].push(numTroops); }

        if(upper[planNum].size() == (lower[planNum].size() + 2)) {
            lower[planNum].push(upper[planNum].top());
            upper[planNum].pop();
        }
        else if((upper[planNum].size() + 2) == lower[planNum].size()) {
            upper[planNum].push(lower[planNum].top());
            lower[planNum].pop();
        }
    }

    // used to calculate median
    int calcMedian(uint32_t planNum) {
        if(upper[planNum].size() == lower[planNum].size()) {
            return (upper[planNum].top() + lower[planNum].top())/2;
        }
        else if(upper[planNum].size() > lower[planNum].size()) {
            return upper[planNum].top();
        }
        else {
            return lower[planNum].top();
        }
    }

    // gets command line args, and settings of input file
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
        //

        std::cout << "Deploying troops...\n";
        planets.resize(numPlans);
        if(modeGen) { generals.resize(numGens); }
        if(modeMed) { upper.resize(numPlans); lower.resize(numPlans); }
        if(modeWatch) { ambushes.resize(numPlans); attacks.resize(numPlans); }
        
    } // getMode
    
public:
    
    // custom constructor
    Galaxy(const int &argc, char *argv[]) {
        getMode(argc, argv);
    }
    
    // used by user to run the simulation
    // handles DL and PR input mode
    void runSim() {
        std::stringstream ss;
        if(!modeDL) {
            P2random::PR_init(ss, randomSeed, numGens, numPlans, numDeployments, arrivalRate);
        }
        std::istream &inputStream = !modeDL ? ss : std::cin;
        uint32_t prevTime = 0;
        uint32_t currentTime = 0;
        Deployment dep;
        while(readInput(inputStream, prevTime, dep)) {
            if(modeMed && (currentTime != prevTime)) {
                printMedian(currentTime);
                currentTime = prevTime; // updates currentTime
            }
            for(uint32_t i = 0; i < planets.size(); i++) {
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
                        if(modeMed) {
                            pqPush(i, numTroopsSith*2);
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
                        if(modeMed) {
                            pqPush(i, numTroopsJedi*2);
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
                        if(modeMed) {
                            pqPush(i, numTroopsJedi*2);
                        }
                        planets[i].jedi.pop();
                        planets[i].sith.pop();
                    }
                    if(modeWatch) {
                        evaluateAmbush(dep);
                        evaluateAttack(dep);
                    }
                    numBattles++;
                }
            }
        }
        if(modeMed) {
            printMedian(prevTime);
        }
        printSummary();
    }

};

#endif