#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>

#include "stats.h"
#include "random.h"
#include "zoo.h"
zoo_stats stats;
class Slot
{
public:
    Slot* up = nullptr;
    Slot* down = nullptr;
    Slot* left = nullptr;
    Slot* right = nullptr;
    char getPositionChar()
    {
        char ret = 'O';
        if(up == nullptr) ret = 'W';
        if(down == nullptr) ret = 'S';
        if(left == nullptr) ret = 'A';
        if(right == nullptr) ret = 'D';
        //Края
        if(up == nullptr && left == nullptr) ret = 'Q';
        if(up == nullptr && right == nullptr) ret = 'E';
        if(down == nullptr && left == nullptr) ret = 'Z';
        if(down == nullptr && right == nullptr) ret = 'C';
        return ret;
    }
};
class World
{
protected:
    int maxPosition;
public:
    World(const std::string& name, int id,int maxX, int maxY);
    ~World();
    std::string name;
    int id;
    int maxX;
    int maxY;
    Slot* map;
    //
    Slot& get(int x, int y);
    void printPosTable();
};
World::World(const std::string& name, int id, int maxX, int maxY)
{
    this->name = name;
    this->id = id;
    maxPosition = maxX*maxY;
    map = new Slot[maxPosition];
    this->maxX = maxX;
    this->maxY = maxY;
    for(int i=0;i<maxPosition;++i)
    {
        int x = i % maxX;
        int y = i / maxX;
        if(x != maxX-1) map[i].right=&map[i+1];
        if(x != 0) map[i].left=&map[i-1];
        if(y != 0) map[i].up=&map[i-maxX];
        if(y != maxY-1) map[i].down=&map[i+maxX];
    }
}
World::~World()
{
    delete[] map;
}
void World::printPosTable()
{
    for(int i=0;i<maxPosition;++i)
    {
        if((i % maxX) == 0) std::cout << std::endl;
        std::cout << map[i].getPositionChar();
    }
    std::cout << std::endl;
}
Slot& World::get(int x,int y)
{
    if(x>maxX || y>maxY || x<0 || y<0) throw new std::exception();
    return map[y*maxX+x];
}

class Engine
{
public:
    World* world;
    std::vector<World*> worlds;
    void newWorld(World* new_world);
    ~Engine();
};
void Engine::newWorld(World* new_world)
{
    worlds.push_back(new_world);
}
Engine::~Engine()
{
    for(World* w : worlds)
    {
        delete w;
    }
    worlds.clear();
}
int main()
{
    std::cout << "Init engine...";
    Engine engine;
    std::cout << "OK" << std::endl;
    std::cout << "Init world...";
    World* testworld = new World("test",0,10,10);
    engine.newWorld(testworld);
    std::cout << "OK" << std::endl;
    testworld->printPosTable();
    return 0;
}
