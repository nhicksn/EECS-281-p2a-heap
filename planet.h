// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef planet.h
#define planet.h

#include <queue>
#include "p2a-heap.h"


class Planet {
    std::priority_queue<Deployment> jedi;
    std::priority_queue<Deployment> sith;

};

#endif