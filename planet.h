// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef PLANET_H
#define PLANET_H

#include <queue>
#include "p2a-heap.h"

enum class State { Initial, SeenOne, SeenBoth };

enum class DepType { Jedi, Sith };

struct compareJedi {
    bool operator()(const Deployment &a, const Deployment &b) {
        return a.forceSens > b.forceSens;
    }
};

struct compareSith {
    bool operator()(const Deployment &a, const Deployment &b) {
        return a.forceSens < b.forceSens;
    }
};

// idk if this is needed
struct tieBreak {
    bool operator()(const Deployment &a, const Deployment &b) {
        return a.timeStamp > b.timeStamp;
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
};

class Planet {
    std::priority_queue<Deployment> jedi;
    std::priority_queue<Deployment> sith;

};

#endif