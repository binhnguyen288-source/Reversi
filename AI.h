#pragma once

#include <immintrin.h>

#define mate 1000
#define nota 0xfefefefefefefefe
#define noth 0x7f7f7f7f7f7f7f7f
typedef unsigned long long BB;
typedef __m256i QBB;

BB getbestmove();
BB set(int);
void resetKH();
void makemove(BB& x, BB& o, BB move);
bool checkvalid(BB x, BB o, BB move); 
BB movegen(QBB X, QBB O);
int search(BB x, BB o, int alpha, int beta, int depth, int ply, bool flag);
BB hor(QBB b);

int getnode();

QBB pfill(QBB r, QBB e);
QBB nfill(QBB r, QBB e);