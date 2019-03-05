#ifndef ENGINE_H
#define ENGINE_H
#include <string>
#include <vector>
class Slot;
class World;
class Zoo
{
public:
    Slot* slot;
    void prepareTick(World& world);
    void tick(World& world);
    bool move(Slot* slot);
};
class Slot
{
public:
    Slot* up = nullptr;
    Slot* down = nullptr;
    Slot* left = nullptr;
    Slot* right = nullptr;
    char getPositionChar();
    enum class TreeType
    {
        BARRIER, //Пройти нельзя. Совсем
        HARD, //Очень сложная территория
        NORMAL, //Обычная территория
        FAST //Безопасная территория
    };
    //Tree information
    TreeType tree;
    Zoo* zoo = nullptr;
};
class World
{
protected:
    int maxPosition;
    long long int ticks_old;
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
    bool put(int x, int y, Zoo* zoo);
    void printPosTable();
    void run(int ticks);
    void tick();
};

class Engine
{
public:
    World* world;
    std::vector<World*> worlds;
    void newWorld(World* new_world);
    ~Engine();
};

extern Engine engine;
#endif
