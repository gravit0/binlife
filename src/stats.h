#ifndef STATS_H
#define STATS_H
struct zoo_stats_dead
{
    long genome;
    long no_eat;
    long more_age;
    void print();
};
struct zoo_stats_mut
{
    long bisexual;
    void print();
};
struct zoo_stats
{
    long created;
    long balance;
    long random;
    zoo_stats_dead dead;
    zoo_stats_mut mut;
    void print();
};
extern zoo_stats stats;
#endif
