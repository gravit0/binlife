#ifndef RANDOM_H
#define RANDOM_H
class Random {
    unsigned long long int pool;
    unsigned char pos;
public:

    Random();

    unsigned long long int rand64();

    inline char rand2() {
        if (pos >= 63) {
            pool = rand64();
            pos = 0;
        }
        int ret = ((pool & (1 << pos)) == 0) ? 0 : 1;
        pos++;
        return ret;
    }

    inline unsigned char rand8() {
        if (pos >= (63 - 8)) {
            pool = rand64();
            pos = 0;
        }
        int ret = (pool >> pos) & 255;
        pos += 8;
        return ret;
    }

    inline char rand6() {
        if (pos >= (63 - 6)) {
            pool = rand64();
            pos = 0;
        }
        int ret = (pool >> pos) & 63;
        pos++;
        return ret;
    }

    inline unsigned short rand16() {
        if (pos >= (63 - 16)) {
            pool = rand64();
            pos = 0;
        }
        int ret = (pool >> pos) & 65535;
        pos++;
        return ret;
    }

    inline char rand4() {
        int a = rand2();
        a |= (rand2() << 1);
        return a;
    }

    void printMap();
};
extern Random rnd;
#endif
