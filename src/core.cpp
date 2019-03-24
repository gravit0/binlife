#include <iostream>
#include "engine.h"

Action::Action(ActionType type)
{
    this->type = type;
}

char Slot::getPositionChar()
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
World::World(const std::string& name, int id, int maxX, int maxY)
{
    this->name = name;
    this->id = id;
    this->ticks_old = 0;
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
void World::run(int ticks)
{
    for(int i=0;i<ticks;++i)
    {
        tick();
    }
}
void World::tick()
{
    for(int i=0;i<maxPosition;++i)
    {
        if(map[i].zoo == nullptr) continue;
        Zoo& zoo = *map[i].zoo;
        if(zoo.status == Zoo::Status::DEAD) continue;
        zoo.prepareTick(*this);
    }
    for(int i=0;i<maxPosition;++i)
    {
        if(map[i].zoo == nullptr) continue;
        Zoo& zoo = *map[i].zoo;
        if(zoo.status == Zoo::Status::DEAD)
        {
            zoo.dead_score--;
            if(zoo.dead_score<=0) zoo.remove();
            delete map[i].zoo;
        }
        else zoo.tick(*this);
    }
    ticks_old++;
}
bool World::put(int x, int y, Zoo* zoo)
{
    Slot& slot = get(x,y);
    if(slot.tree == Slot::TreeType::BARRIER) return false;
    if(slot.zoo != nullptr) return false;
    slot.zoo = zoo;
    zoo->slot = &slot;
    return true;
}
bool Zoo::move(Slot* slot)
{
    if(slot->tree == Slot::TreeType::BARRIER) return false;
    if(slot->zoo != nullptr) return false;
    this->slot->zoo = nullptr;
    this->slot = slot;
    this->slot->zoo = this;
    return true;
}
bool Zoo::remove()
{
    if(slot != nullptr)
    {
        slot->zoo = nullptr;
        slot = nullptr;
        return true;
    }
    return false;
}
