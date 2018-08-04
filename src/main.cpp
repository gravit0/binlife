#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <boost/random.hpp>
#include <ctime>
boost::mt19937 rng(time(NULL));
boost::uniform_int<> six(1,16);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > die(rng, six);

int ZooBalance = 0;
int ZooCreated = 0;
struct Zoo
{
    long long int flags; //Геном
    int status; // Состояние (геном)
    int gstatus; //Глобальный статус
    short power;
    short age;
    short timer;
};
//Биты
#define GSTATUS_ENABLE 1 << 0
#define GSTATUS_DEAD 1 << 1
#define GSTATUS_SEX 1 << 2
#define GSTATUS_BARRIER 1 << 3
#define STATUS_MALE 1 << 0
#define STATUS_FEMALE 1 << 1

#define GEN_FOTO 1 << 0
#define GEN_MOVE 1 << 1
#define GEN_FINDPARTNER 1 << 2
#define GEN_MOREAGE 1 << 3

Zoo* map;
int map_size = 100;
int map_maxpos = map_size*map_size;
void MoveTo(Zoo& z1,Zoo& z2)
{
    z2 = z1;
    z1.gstatus = 0;
    z1.status = 0;
}
Zoo* getUp(int pos)
{
    int upPos = pos - map_size;
    if(upPos <= 0 || upPos >= map_maxpos) return nullptr;
    return &(map[upPos]);
}
Zoo* getDown(int pos)
{
    int upPos = pos + map_size;
    if(upPos <= 0 || upPos >= map_maxpos) return nullptr;
    return &(map[upPos]);
}
Zoo* getLeft(int pos)
{
    int upPos = pos - 1;
    if(upPos <= 0 || upPos >= map_maxpos) return nullptr;
    return &(map[upPos]);
}
Zoo* getRight(int pos)
{
    int upPos = pos + 1;
    if(upPos <= 0 || upPos >= map_maxpos) return nullptr;
    return &(map[upPos]);
}

bool ifAllowUp(int pos)
{
    int upPos = pos - map_size;
    if(upPos <= 0 || upPos >= map_maxpos) return false;
    Zoo z = map[upPos];
    if(z.gstatus & GSTATUS_ENABLE != 0) return false;
    if(z.gstatus & GSTATUS_BARRIER != 0) return false;
    return true;
}
bool ifAllowDown(int pos)
{
    int upPos = pos + map_size;
    if(upPos <= 0 || upPos >= map_maxpos) return false;
    Zoo z = map[upPos];
    if(z.gstatus & GSTATUS_ENABLE != 0) return false;
    if(z.gstatus & GSTATUS_BARRIER != 0) return false;
    return true;
}
bool ifAllowLeft(int pos)
{
    int upPos = pos - 1;
    if(upPos <= 0 || upPos >= map_maxpos) return false;
    Zoo z = map[upPos];
    if(z.gstatus & GSTATUS_ENABLE != 0) return false;
    if(z.gstatus & GSTATUS_BARRIER != 0) return false;
    return true;
}
bool ifAllowRight(int pos)
{
    int upPos = pos + 1;
    if(upPos <= 0 || upPos >= map_maxpos) return false;
    Zoo z = map[upPos];
    if(z.gstatus & GSTATUS_ENABLE != 0) return false;
    if(z.gstatus & GSTATUS_BARRIER != 0) return false;
    return true;
}
void newZoo(Zoo& parent1,Zoo& parent2,int pos1,int pos2)
{
    if((parent1.status  & STATUS_MALE) && (parent2.status  & STATUS_MALE)) std::cout << "W: MALE!";
    if(parent1.power < 30 || parent2.power < 30) return;
    Zoo z;
    int ran = die();
    z.status = (ran % 2 == 0) ? parent1.status : parent2.status;
    ran = die();
    z.flags = (ran % 2 == 0) ? parent1.flags : parent2.flags;
    z.power = 1;
    parent1.power = 0;
    parent2.power = 0;
    z.age = 0;
    z.timer = 0;
    z.gstatus = GSTATUS_ENABLE;
    if((z.status  & STATUS_FEMALE) != 0) ZooBalance++;
    if((z.status  & STATUS_MALE) != 0)   ZooBalance--;
    ZooCreated++;
    if(ifAllowUp(pos1)) { std::swap(z,map[pos1 - map_size]);  return; }
    if(ifAllowUp(pos2)) { std::swap(z,map[pos2 - map_size]);  return; }
    if(ifAllowDown(pos1)) { std::swap(z,map[pos1 + map_size]);  return; }
    if(ifAllowDown(pos2)) { std::swap(z,map[pos2 + map_size]);  return; }
    if(ifAllowLeft(pos1)) { std::swap(z,map[pos1 - 1]);  return; }
    if(ifAllowLeft(pos2)) { std::swap(z,map[pos2 - 1]);  return; }
    if(ifAllowRight(pos1)) { std::swap(z,map[pos1 + 1]);  return; }
    if(ifAllowRight(pos2)) { std::swap(z,map[pos2 + 1]);  return; }
}
void run(Zoo& zoo,int pos)
{
    if(zoo.flags & GEN_FOTO != 0) zoo.power += 3;
    if(zoo.flags & GEN_MOVE != 0 && zoo.power >= 5) {
        if((zoo.flags & GEN_FINDPARTNER) != 0 )
        {
            int find_parametr = 0;
            if((zoo.status & STATUS_MALE) != 0) find_parametr |= STATUS_FEMALE;
            if((zoo.status & STATUS_FEMALE) != 0) find_parametr |= STATUS_MALE;
            Zoo* i = getUp(pos);
            if(i != nullptr && (i->status & find_parametr) != 0) newZoo(zoo,*i,pos,pos - map_size);
            i = getDown(pos);
            if(i != nullptr && (i->status & find_parametr) != 0) newZoo(zoo,*i,pos,pos + map_size);
            i = getLeft(pos);
            if(i != nullptr && (i->status & find_parametr) != 0) newZoo(zoo,*i,pos,pos - 1);
            i = getRight(pos);
            if(i != nullptr && (i->status & find_parametr) != 0) newZoo(zoo,*i,pos,pos + 1);
        }
        int ran = die() % 4;
        switch(ran) {
            case 1: if(ifAllowUp(pos)) { MoveTo(map[pos],map[pos - map_size]); zoo.power-= 2;  return;}
            case 2: if(ifAllowRight(pos)) { MoveTo(map[pos],map[pos + 1]); zoo.power-= 2;  return;}
            case 3: if(ifAllowDown(pos)) { MoveTo(map[pos],map[pos + map_size]); zoo.power-= 2;  return;}
            case 4: if(ifAllowLeft(pos)) { MoveTo(map[pos],map[pos - 1]); zoo.power-= 2;  return;}
        }
    }
    zoo.age++;
    if(!ifAllowUp(pos) && !ifAllowRight(pos) && !ifAllowLeft(pos) && !ifAllowDown(pos)) zoo.power -= 4;
    if(zoo.power < 0) { zoo.gstatus = GSTATUS_DEAD;}
    if(zoo.age > 100 && (zoo.flags & GEN_MOREAGE == 0)) { zoo.gstatus = GSTATUS_DEAD; }
}
void cycle()
{
    for(int i=0;i<map_maxpos;++i)
    {
        if(map[i].gstatus & GSTATUS_ENABLE != 0) run(map[i],i);
    }
}

void printMap()
{
    for(int i=0;i<map_maxpos;++i)
    {
        if((map[i].gstatus & GSTATUS_ENABLE) != 0) {
            if((map[i].status & (STATUS_MALE | STATUS_FEMALE)) == (STATUS_MALE | STATUS_FEMALE) ) std::cout << "I";
            else if((map[i].status & STATUS_MALE) != 0) std::cout << "M";
            else if((map[i].status & STATUS_FEMALE) != 0) std::cout << "F";
        }
        else if((map[i].gstatus & GSTATUS_DEAD) != 0) std::cout << "D";
        else std::cout << "0";
        if(i % map_size == 0) std::cout << std::endl;
    }
}

int main()
{
    std::cout << "Build game map ";
    map = new Zoo[map_size*map_size];
    memset(map,0,map_size*map_size*sizeof(Zoo));
    std::cout << "OK" << std::endl;
    
    std::cout << "1 Live ";
    map[20] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_MALE,GSTATUS_ENABLE,0,0,0};
    map[21] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_MALE,GSTATUS_ENABLE,0,0,0};
    map[22] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_MALE,GSTATUS_ENABLE,0,0,0};
    
    map[11] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_FEMALE,GSTATUS_ENABLE,0,0,0};
    map[12] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_FEMALE,GSTATUS_ENABLE,0,0,0};
    map[13] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_FEMALE,GSTATUS_ENABLE,0,0,0};
    map[14] = {GEN_FOTO | GEN_MOVE | GEN_FINDPARTNER,STATUS_FEMALE,GSTATUS_ENABLE,0,0,0};
    
    //map[124] = {GEN_FOTO | GEN_MOVE,STATUS_MALE | STATUS_FEMALE,GSTATUS_ENABLE,0,0,0};
    std::cout << "OK" << std::endl;
    printMap();
    std::cout << "Test cycles ";
    for(int i=0;i<10000;++i) { cycle(); if(i % 100 == 0) std::cerr << i << std::endl;}
    std::cout << "OK" << std::endl;
    std::cout << "Print map " << std::endl;
    printMap();
    std::cout << std::endl;
    std::cout << "Stopping game ";
    delete[] map;
    std::cout << "OK" << std::endl;
    std::cout << "ZooBalance:" << ZooBalance << std::endl;
    std::cout << "ZooCreated:" << ZooCreated << std::endl;
}
