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
    //
    double power;
    double dPower;
    int dead_score = 30; //30 тиков мертвый Zoo будет гнить
    enum class Status
    {
        ALIVE, //Жив
        DEAD //Мертв, но всё еще занимает место
    };
    Status status;
    //
    void prepareTick(World& world);
    void tick(World& world);
    bool move(Slot* slot);
    //Функции Бога
    bool remove(); // Удаляет из мира
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
