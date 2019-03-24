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
#include "engine.h"
zoo_stats stats;
Engine engine;
MemoryEntry ZooHead::findMemory(long long int mask)
{
    for(MemoryEntry enity : memory)
    {
        if((enity.mask & mask) != 0)
        {
            return enity;
        }
    }
}
void Zoo::prepareTick(World& world)
{
    Slot* this_slot = slot->up;
    if(this_slot != nullptr && this_slot->tree != Slot::TreeType::BARRIER)
    {
        if(this_slot->zoo != nullptr)
        {
            //TODO: NEXT
        }
        else if(head.joy > -100)
        {
            head.next_action = Action(Action::ActionType::UP);
        }
    }
}
void Zoo::tick(World& world)
{
    if(head.next_action.type == Action::ActionType::UP)
    {
        if(move(slot->up)) return;
    }
    else if(head.next_action.type == Action::ActionType::DOWN)
    {
        if(move(slot->down)) return;
    }
    else if(head.next_action.type == Action::ActionType::LEFT)
    {
        if(move(slot->left)) return;
    }
    else if(head.next_action.type == Action::ActionType::RIGHT)
    {
        if(move(slot->right)) return;
    }
}
int main()
{
    std::cout << "Init world...";
    World* testworld = new World("test",0,10,10);
    engine.newWorld(testworld);
    std::cout << "OK" << std::endl;
    testworld->printPosTable();
    std::cout << "First 10 ticks...";
    testworld->run(10);
    std::cout << "OK" << std::endl;
    return 0;
}
