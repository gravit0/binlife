#ifndef ZOO_H
#define ZOO_H
struct Zoo {
    long long int flags; //Полезный геном
    long long int flags2; //Мусорный геном
    int status; // Состояние (геном)
    int gstatus; //Глобальный статус
    short power;
    short age;
    short timer;
    int dead_score;
    bool isEmpty();

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
#endif
