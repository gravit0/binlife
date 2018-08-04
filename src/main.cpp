#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <boost/random.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
boost::mt19937_64 rng(time(NULL));
boost::uniform_int<> six(1, 16);
boost::variate_generator<boost::mt19937_64, boost::uniform_int<> > die(rng, six);
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

int ZooBalance = 0;
int ZooCreated = 0;
int ZooMutBISEXUAL = 0;
int ZooDeadByGenome = 0;
unsigned long long int RandomNumbers = 0;

class Random {
    unsigned long long int pool;
    unsigned char pos;
public:

    Random() {
        RandomNumbers++;
        pool = rng();
        pos = 0;
    }

    unsigned long long int rand64() {
        RandomNumbers++;
        return rng();
    }

    inline char rand2() {
        if (pos >= 63) {
            pool = rng();
            pos = 0;
            RandomNumbers++;
        }
        int ret = ((pool & (1 << pos)) == 0) ? 0 : 1;
        pos++;
        return ret;
    }

    inline unsigned char rand8() {
        if (pos >= (63 - 8)) {
            pool = rng();
            pos = 0;
            RandomNumbers++;
        }
        int ret = (pool >> pos) & 255;
        pos += 8;
        return ret;
    }

    inline char rand6() {
        if (pos >= (63 - 6)) {
            pool = rng();
            pos = 0;
            RandomNumbers++;
        }
        int ret = (pool >> pos) & 63;
        pos++;
        return ret;
    }

    inline unsigned short rand16() {
        if (pos >= (63 - 16)) {
            pool = rng();
            pos = 0;
            RandomNumbers++;
        }
        int ret = (pool >> pos) & 65535;
        pos++;
        return ret;
    }

    char rand4() {
        int a = rand2();
        a |= (rand2() << 1);
        return a;
    }

    void printMap() {
        int a[64];
        for (int i = 0; i < 2000; ++i) {
            pool = rng();
            for (int i = 0; i < 64; ++i) {
                int r = (pool & (1 << i));
                a[i] += (r == 0) ? 1 : -1;
            }
        }
        for (int i = 0; i < 64; ++i) {
            std::cout << i << ": " << ((float) a[i] / (float) 2000.0) * 100 << std::endl;
        }
    }
};
Random rnd;

struct Zoo {
    long long int flags; //Полезный геном
    long long int flags2; //Мусорный геном
    int status; // Состояние (геном)
    int gstatus; //Глобальный статус
    short power;
    short age;
    short timer;
    int dead_chance;

    bool isEmpty() {
        if ((gstatus & GSTATUS_ENABLE) != 0) return false;
        if ((gstatus & GSTATUS_BARRIER) != 0) return false;
        return true;
    }

};
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
        parent1.power = 0;
        parent2.power = 0;
        z.age = 0;
        z.timer = 0;
        z.gstatus = GSTATUS_ENABLE;
        z.dead_chance = 0;
        if ((z.status & STATUS_FEMALE) != 0 && (z.status & STATUS_MALE) != 0) {
        } else if ((z.status & STATUS_FEMALE) != 0) ZooBalance++;
        else if ((z.status & STATUS_MALE) != 0) ZooBalance--;
        ZooCreated++;
        //Мутируем (полезный геном)
        if (rnd.rand8() == 0) {
            int fpos = rnd.rand6();
            z.flags = z.flags ^ (1 << fpos);
            if ((z.flags & GEN_BISEXUAL) != 0) {
                z.status |= (STATUS_MALE | STATUS_FEMALE);
                ZooMutBISEXUAL++;
            }
        }
        //Мутируем (бесполезный геном)
        if (rnd.rand8() == 0) {
            int fpos = rnd.rand6();
            z.flags2 = z.flags2 ^ (1 << fpos);
            if ((z.flags2 & 0x1444000000200004) > 0x100000) {
                z.gstatus = GSTATUS_DEAD;
                ZooDeadByGenome++;
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

struct ZooPosition {
    int pos = 0; //Смещение
    Zoo* up = nullptr;
    Zoo* down = nullptr;
    Zoo* left = nullptr;
    Zoo* right = nullptr;
    bool isAllowUp = false;
    bool isAllowDown = false;
    bool isAllowLeft = false;
    bool isAllowRight = false;
};

class Engine {
public:
    World world;

    Engine(World&& world) : world(world) {
    }

    void run(Zoo& zoo, int pos) {
        if ((zoo.flags & GEN_FOTO) != 0) zoo.power += 3;
        if ((zoo.flags & GEN_BISEXUAL) != 0) zoo.power--;
        if (((zoo.flags & GEN_MOVE) != 0) && zoo.power >= 5) {
            if ((zoo.flags & GEN_FINDPARTNER) != 0) {
                int find_parametr = 0;
                if ((zoo.status & STATUS_MALE) != 0) find_parametr |= STATUS_FEMALE;
                if ((zoo.status & STATUS_FEMALE) != 0) find_parametr |= STATUS_MALE;
                Zoo* i = world.getUp(pos);
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos, pos - world.map_size);
                i = world.getDown(pos);
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos, pos + world.map_size);
                i = world.getLeft(pos);
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos, pos - 1);
                i = world.getRight(pos);
                if (i != nullptr && (i->status & find_parametr) != 0) world.newZoo(zoo, *i, pos, pos + 1);
            }
            if (!world.isAllowUp(pos) && !world.isAllowDown(pos) && !world.isAllowLeft(pos) && !world.isAllowRight(pos)) return;
            int ran = rnd.rand4();
            switch (ran) {
                case 0: if (world.isAllowUp(pos)) {
                        world.MoveTo(world.map[pos], world.map[pos - world.map_size]);
                        zoo.power -= 2;
                        return;
                    }
                case 1: if (world.isAllowRight(pos)) {
                        world.MoveTo(world.map[pos], world.map[pos + 1]);
                        zoo.power -= 2;
                        return;
                    }
                case 2: if (world.isAllowDown(pos)) {
                        world.MoveTo(world.map[pos], world.map[pos + world.map_size]);
                        zoo.power -= 2;
                        return;
                    }
                case 3: if (world.isAllowLeft(pos)) {
                        world.MoveTo(world.map[pos], world.map[pos - 1]);
                        zoo.power -= 2;
                        return;
                    }
            }
        }
        zoo.age++;
        if (zoo.power < 0) {
            zoo.gstatus = GSTATUS_DEAD;
        }
        if (zoo.age > 100 && (zoo.flags & GEN_MOREAGE) == 0) {
            zoo.gstatus = GSTATUS_DEAD;
        }
    }

    void cycle() {
        ZooPosition pos;
        for (int i = 0; i < world.map_maxpos; ++i) {
            if ((world.map[i].gstatus & GSTATUS_ENABLE) != 0) {
                if (pos + world.map_size < world.map_maxpos) {
                    pos.down = world.map[pos + world.map_size];
                    if (pos.down->isEmpty()) pos.isAllowDown = true;
                    else pos.isAllowDown = false;
                } else pos.isAllowDown = false;
                if (pos - world.map_size < world.map_maxpos) {
                    pos.up = world.map[pos - world.map_size];
                    if (pos.down->isEmpty()) pos.isAllowUp = true;
                    else pos.isAllowUp = false;
                } else pos.isAllowUp = false;
                if (pos - 1 < world.map_maxpos) {
                    pos.up = world.map[pos - world.map_size];
                    if (pos.down->isEmpty()) pos.isAllowLeft = true;
                    else pos.isAllowLeft = false;
                } else pos.isAllowLeft = false;
                if (pos + 1 < world.map_maxpos) {
                    pos.up = world.map[pos - world.map_size];
                    if (pos.down->isEmpty()) pos.isAllowLeft = true;
                    else pos.isAllowLeft = false;
                } else pos.isAllowLeft = false;
                run(world.map[i], i);
            }
            pos.pos++;
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
    for (int i = 0; i < 4000; ++i) {
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
    std::cout << "Random:" << RandomNumbers << std::endl;
    std::cout << "ZooBalance:" << ZooBalance << std::endl;
    std::cout << "ZooCreated:" << ZooCreated << std::endl;
    std::cout << "Zoo Mutation BISEXUAL:" << ZooMutBISEXUAL << std::endl;
    std::cout << "ZooDeadByGenome:" << ZooDeadByGenome << std::endl;
    std::cout << "Deviation of chance:" << ((float) ZooBalance / (float) ZooCreated) * 100 << "%" << std::endl;
}
