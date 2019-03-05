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
void Zoo::prepareTick(World& world)
{
    
}
void Zoo::tick(World& world)
{
    
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
