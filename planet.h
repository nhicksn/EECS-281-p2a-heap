// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#ifndef PLANET_H
#define PLANET_H

#include <queue>
#include <cstdint>
#include "p2a-heap.h"

enum class State { Initial, SeenOne, SeenBoth };

enum class DepType { Jedi, Sith };

struct movieFight {
    uint32_t jediTime;
    uint32_t sithTime;
    uint32_t maybeTime;
    uint32_t jediForce;
    uint32_t sithForce;
    uint32_t maybeForce;
    State status = State::Initial;
    movieFight() : jediTime(UINT32_MAX), sithTime(UINT32_MAX), maybeTime(UINT32_MAX),
                    jediForce(UINT32_MAX), sithForce(UINT32_MAX), maybeForce(UINT32_MAX)
                    {}
};

struct General {
    uint32_t totalJedi;
    uint32_t totalSith;
    uint32_t totalAlive;
};

struct Deployment {
    uint32_t timeID;
    uint32_t genID;
    DepType side;
    uint32_t forceSens;
    uint32_t quantity;
    Deployment(uint32_t t, uint32_t g, DepType s, uint32_t f, uint32_t q) : 
                timeID(t), genID(g), side(s), forceSens(f), quantity(q) {}
    Deployment() : timeID(0), genID(0), side(DepType::Jedi), forceSens(UINT32_MAX), quantity(0) {}
};

struct compareJedi {
    bool operator()(const Deployment &a, const Deployment &b) {
        if(a.forceSens == b.forceSens) return a.timeID > b.timeID;
        return a.forceSens > b.forceSens;
    }
};

struct compareSith {
    bool operator()(const Deployment &a, const Deployment &b) {
        if(a.forceSens == b.forceSens) return a.timeID > b.timeID;
        return a.forceSens < b.forceSens;
    }
};

struct Planet {
    std::priority_queue<Deployment, std::vector<Deployment>, compareJedi> jedi;
    std::priority_queue<Deployment, std::vector<Deployment>, compareSith> sith;
};

#endif