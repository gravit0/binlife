#include <iostream>
#include "stats.h"
void zoo_stats::print()
{
    std::cout << "ZooBalance: " << balance << std::endl;
    std::cout << "ZooCreated: " << created << std::endl;
    std::cout << "Random: " << random << std::endl;
    std::cout << "Zoo Dead: " << std::endl;
    dead.print();
    std::cout << "Zoo Mutation: " << std::endl;
    mut.print();
}
void zoo_stats_dead::print()
{
    std::cout << "- No eat: " << no_eat << std::endl;
    std::cout << "- Genome: " << genome << std::endl;
    std::cout << "- More Age: " << more_age << std::endl;
}
void zoo_stats_mut::print()
{
    std::cout << "- BiSexual: " << bisexual << std::endl;
}
