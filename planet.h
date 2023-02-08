// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef PLANET_H
#define PLANET_H

#include <queue>
#include "p2a-heap.h"

enum class State { Initial, SeenOne, SeenBoth };

enum class DepType { Jedi, Sith };

struct compareJedi {
    bool operator()(const Deployment &a, const Deployment &b) {
        if(a.forceSens == b.forceSens) return a.timeStamp > b.timeStamp;
        return a.forceSens > b.forceSens;
    }
};

struct compareSith {
    bool operator()(const Deployment &a, const Deployment &b) {
        if(a.forceSens == b.forceSens) return a.timeStamp > b.timeStamp;
        return a.forceSens < b.forceSens;
    }
};

struct General {
    uint16_t numTroops;
};

struct Deployment {
    // might not need this
    uint16_t timeStamp;
    uint16_t genID;
    DepType side;
    uint16_t forceSens;
    // might not need this
    uint16_t quantity;
    Deployment(uint16_t t, uint16_t g, DepType s, uint16_t f, uint16_t q) : 
                timeStamp(t), genID(g), side(s), forceSens(f), quantity(q) {}
};

class Planet {
public:
    std::priority_queue<Deployment, compareJedi> jedi;
    std::priority_queue<Deployment, compareSith> sith;

};

#endif