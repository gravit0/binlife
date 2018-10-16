#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>

#include "stats.h"
#include "random.h"
#include "zoo.h"
//Биты
#define GSTATUS_ENABLE 1 << 0
#define GSTATUS_DEAD 1 << 1
#define GSTATUS_SEX 1 << 2
#define GSTATUS_BARRIER 1 << 3
#define STATUS_MALE 1 << 0
#define STATUS_FEMALE 1 << 1
#define STATUS_RANDOMDEAD 1 << 2

#define GEN_FOTO 1 << 0
#define GEN_MOVE 1 << 1
#define GEN_FINDPARTNER 1 << 2
#define GEN_MOREAGE 1 << 3
#define GEN_BISEXUAL 1 << 4
bool Zoo::isEmpty() {
        if ((gstatus & GSTATUS_ENABLE) != 0) return false;
        if ((gstatus & GSTATUS_BARRIER) != 0) return false;
        return true;
    }
zoo_stats stats;
Random rnd;
class Engine;

class World {
private:
    Zoo* map;
    int map_size;
    int map_maxpos;
    friend Zoo;
    friend Engine;
public:

    World(int size) {
        map_size = size;
        map_maxpos = map_size*map_size;
        map = new Zoo[map_maxpos];
        memset(map, 0, map_maxpos * sizeof (Zoo));
    }

    ~World() {
        //delete[] map;
    }

    inline bool isAllowUp(int pos) {
        Zoo* z = getUp(pos);
        if (z != nullptr) return z->isEmpty();
        else return false;
    }

    inline bool isAllowDown(int pos) {
        Zoo* z = getDown(pos);
        if (z != nullptr) return z->isEmpty();
        else return false;
    }

    inline bool isAllowRight(int pos) {
        Zoo* z = getRight(pos);
        if (z != nullptr) return z->isEmpty();
        else return false;
    }

    inline bool isAllowLeft(int pos) {
        Zoo* z = getLeft(pos);
        if (z != nullptr) return z->isEmpty();
        else return false;
    }

    void MoveTo(Zoo& z1, Zoo& z2) {
        z2 = z1;
        z1.gstatus = 0;
        z1.status = 0;
    }

    Zoo* getUp(int pos) {
        int upPos = pos - map_size;
        if (upPos <= 0 || upPos >= map_maxpos) return nullptr;
        return &(map[upPos]);
    }

    Zoo* getDown(int pos) {
        int upPos = pos + map_size;
        if (upPos <= 0 || upPos >= map_maxpos) return nullptr;
        return &(map[upPos]);
    }

    Zoo* getLeft(int pos) {
        int upPos = pos - 1;
        if (upPos <= 0 || upPos >= map_maxpos) return nullptr;
        return &(map[upPos]);
    }

    Zoo* getRight(int pos) {
        int upPos = pos + 1;
        if (upPos <= 0 || upPos >= map_maxpos) return nullptr;
        return &(map[upPos]);
    }

    void printMap() {
        for (int i = 0; i < map_maxpos; ++i) {
            if ((map[i].gstatus & GSTATUS_ENABLE) != 0) {
                if ((map[i].status & (STATUS_MALE | STATUS_FEMALE)) == (STATUS_MALE | STATUS_FEMALE)) std::cout << "I";
                else if ((map[i].status & STATUS_MALE) != 0) std::cout << "M";
                else if ((map[i].status & STATUS_FEMALE) != 0) std::cout << "F";
            } else if ((map[i].gstatus & GSTATUS_DEAD) != 0) std::cout << "D";
            else std::cout << "0";
            if (i % map_size == 0) std::cout << std::endl;
        }
    }

    void newZoo(Zoo& parent1, Zoo& parent2, int pos1, int pos2) {
        if (!isAllowUp(pos1) && !isAllowDown(pos1) && !isAllowLeft(pos1) && !isAllowRight(pos1))
            if (!isAllowUp(pos2) && !isAllowDown(pos2) && !isAllowLeft(pos2) && !isAllowRight(pos2)) return;
        if (parent1.power < 30 || parent2.power < 30) return;
        Zoo z;
        z.status = (rnd.rand2() == 0) ? parent1.status : parent2.status;
        unsigned long long int ran64 = rnd.rand64();
        z.flags = (ran64 & parent1.flags) | ((~ran64) & parent2.flags);
        ran64 = rnd.rand64();
        z.flags2 = (ran64 & parent1.flags2) | ((~ran64) & parent2.flags2);
        z.power = 1;
        parent1.power -= 30;
        parent2.power -= 30;
        z.age = 0;
        z.timer = 0;
        z.gstatus = GSTATUS_ENABLE;
        z.dead_chance = 0;
        if ((z.status & STATUS_FEMALE) != 0 && (z.status & STATUS_MALE) != 0) {
        } else if ((z.status & STATUS_FEMALE) != 0) stats.balance++;
        else if ((z.status & STATUS_MALE) != 0) stats.balance--;
        stats.created++;
        //Мутируем (полезный геном)
        if (rnd.rand8() == 0) {
            int fpos = rnd.rand6();
            z.flags = z.flags ^ (1 << fpos);
            if ((z.flags & GEN_BISEXUAL) != 0) {
                z.status |= (STATUS_MALE | STATUS_FEMALE);
                stats.mut.bisexual++;
            }
        }
        //Мутируем (бесполезный геном)
        if (rnd.rand8() == 0) {
            int fpos = rnd.rand6();
            z.flags2 = z.flags2 ^ (1 << fpos);
            if ((z.flags2 & 0x1444000000200004) > 0x100000) {
                z.gstatus = GSTATUS_DEAD;
                stats.dead.genome++;
            }
        }
        if (isAllowUp(pos1)) {
            std::swap(z, map[pos1 - map_size]);
            return;
        }
        if (isAllowUp(pos2)) {
            std::swap(z, map[pos2 - map_size]);
            return;
        }
        if (isAllowDown(pos1)) {
            std::swap(z, map[pos1 + map_size]);
            return;
        }
        if (isAllowDown(pos2)) {
            std::swap(z, map[pos2 + map_size]);
            return;
        }
        if (isAllowLeft(pos1)) {
            std::swap(z, map[pos1 - 1]);
            return;
        }
        if (isAllowLeft(pos2)) {
            std::swap(z, map[pos2 - 1]);
            return;
        }
        if (isAllowRight(pos1)) {
            std::swap(z, map[pos1 + 1]);
            return;
        }
        if (isAllowRight(pos2)) {
            std::swap(z, map[pos2 + 1]);
            return;
        }
    }

    void firstLife() {
        for (int i = 20; i < 60; i++)
            map[i] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER, 0, STATUS_MALE, GSTATUS_ENABLE, 0, 0, 0, 0};
        for (int i = 80; i < 120; i++)
            map[i] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER, 0, STATUS_FEMALE, GSTATUS_ENABLE, 0, 0, 0, 0};
    }
};

class Engine {
public:
    World world;

    Engine(World&& world) : world(world) {
    }

    void run(Zoo& zoo, ZooPosition pos) {
        if ((zoo.flags & GEN_FOTO) != 0) zoo.power += 3;
        if ((zoo.flags & GEN_BISEXUAL) != 0) zoo.power--;
        if (((zoo.flags & GEN_MOVE) != 0) && zoo.power >= 5) {
            if ((zoo.flags & GEN_FINDPARTNER) != 0) {
                int find_parametr = 0;
                if ((zoo.status & STATUS_MALE) != 0) find_parametr |= STATUS_FEMALE;
                if ((zoo.status & STATUS_FEMALE) != 0) find_parametr |= STATUS_MALE;
                Zoo* i = pos.up;
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos.pos, pos.pos - world.map_size);
                i = pos.down;
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos.pos, pos.pos + world.map_size);
                i = pos.left;
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos.pos, pos.pos - 1);
                i = pos.right;
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos.pos, pos.pos + 1);
            }
            if (!pos.isAllowUp && !pos.isAllowDown && !pos.isAllowLeft && !pos.isAllowRight) return;
            int ran = rnd.rand4();
            switch (ran) {
                case 0: if (pos.isAllowUp) {
                        world.MoveTo(world.map[pos.pos], world.map[pos.pos - world.map_size]);
                        zoo.power -= 2;
                        return;
                    }
                case 1: if (pos.isAllowRight) {
                        world.MoveTo(world.map[pos.pos], world.map[pos.pos + 1]);
                        zoo.power -= 2;
                        return;
                    }
                case 2: if (pos.isAllowDown) {
                        world.MoveTo(world.map[pos.pos], world.map[pos.pos + world.map_size]);
                        zoo.power -= 2;
                        return;
                    }
                case 3: if (pos.isAllowLeft) {
                        world.MoveTo(world.map[pos.pos], world.map[pos.pos - 1]);
                        zoo.power -= 2;
                        return;
                    }
            }
        }
        zoo.age++;
        if (zoo.power < 0) {
            zoo.gstatus = GSTATUS_DEAD;
            stats.dead.no_eat++;
        }
        if (zoo.age > 100 && (zoo.flags & GEN_MOREAGE) == 0) {
            zoo.gstatus = GSTATUS_DEAD;
            stats.dead.more_age++;
        }
    }

    void cycle() {
        ZooPosition pos;
        for (int i = 0; i < world.map_maxpos; ++i) {
            pos.pos = i;
            if ((world.map[i].gstatus & GSTATUS_ENABLE) != 0) {
                if (i + world.map_size < world.map_maxpos) {
                    pos.down = &world.map[i + world.map_size];
                    if (pos.down->isEmpty()) pos.isAllowDown = true;
                    else pos.isAllowDown = false;
                } else {pos.isAllowDown = false; pos.down = nullptr; }
                
                if (i - world.map_size > 0) {
                    pos.up = &world.map[i - world.map_size];
                    if (pos.up->isEmpty()) pos.isAllowUp = true;
                    else pos.isAllowUp = false;
                } else {pos.isAllowUp = false; pos.up = nullptr; }
                
                if (i - 1 > 0) {
                    pos.left = &world.map[i - 1];
                    if (pos.left->isEmpty()) pos.isAllowLeft = true;
                    else pos.isAllowLeft = false;
                } else {pos.isAllowLeft = false; pos.left = nullptr; }
                
                if (i + 1 < world.map_maxpos) {
                    pos.right = &world.map[i + 1];
                    if (pos.right->isEmpty()) pos.isAllowRight = true;
                    else pos.isAllowRight = false;
                } else {pos.isAllowRight = false; pos.right = nullptr; }
                
                run(world.map[i], pos);
            }
        }
    }
};
#define TABLE_SIZE 1024

int main() {
    std::cout << "Build game map ";
    World world(TABLE_SIZE);
    Engine engine(std::move(world));
    std::cout << "OK" << std::endl;

    std::cout << "1 Live ";
    world.firstLife();
    //map[124] = {GEN_FOTO | GEN_MOVE,STATUS_MALE | STATUS_FEMALE,GSTATUS_ENABLE,0,0,0};
    std::cout << "OK" << std::endl;
    world.printMap();
    std::cout << "Test cycles ";
    auto ptime = boost::posix_time::microsec_clock::local_time();
    for (int i = 0; i < 1000; ++i) {
        engine.cycle();
        if ((i % 100) == 0) std::cerr << i << std::endl;
    }
    auto ptime2 = boost::posix_time::microsec_clock::local_time();
    std::cout << "OK" << std::endl;
    std::cout << "Print map " << std::endl;
    world.printMap();
    std::cout << std::endl;
    std::cout << "Stopping game ";
    std::cout << "OK" << std::endl;
    std::cout << "Perforance:" << (ptime2 - ptime) << std::endl;
    std::cout << "Table memory:" << TABLE_SIZE * TABLE_SIZE * sizeof (Zoo) << std::endl;
    stats.print();
    std::cout << "Deviation of chance:" << ((float) stats.balance / (float) stats.created) * 100 << "%" << std::endl;
}
