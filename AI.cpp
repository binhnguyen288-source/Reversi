
#include "AI.h"
#include <algorithm>

BB set(int i)
{
    return (BB)1 << i;
}


BB hor(QBB b)
{
    QBB k = b | _mm256_permute4x64_epi64(b, 78);
    return (k |= _mm256_shuffle_epi32(k, 78))[0];
}

QBB pfill(QBB r, QBB e)
{
    static const QBB mask = { (long long)nota, (long long)noth, -1ll, (long long)nota };
    static const QBB shift1 = { 1, 7, 8, 9 };
    static const QBB shift2 = { 2, 14, 16, 18 };
    static const QBB shift3 = { 4, 28, 32, 36 };
    e &= mask;
    r = _mm256_sllv_epi64(r, shift1) & e;
    r |= _mm256_sllv_epi64(r, shift1) & e;
    e &= _mm256_sllv_epi64(e, shift1);
    r |= _mm256_sllv_epi64(r, shift2) & e;
    e &= _mm256_sllv_epi64(e, shift2);
    r |= _mm256_sllv_epi64(r, shift3) & e;

    return _mm256_sllv_epi64(r, shift1) & mask;
}
QBB nfill(QBB r, QBB e)
{
    static const QBB mask = { (long long)noth, (long long)nota, -1ll, (long long)noth };
    static const QBB shift1 = { 1, 7, 8, 9 };
    static const QBB shift2 = { 2, 14, 16, 18 };
    static const QBB shift3 = { 4, 28, 32, 36 };

    e &= mask;
    r = _mm256_srlv_epi64(r, shift1) & e;
    r |= _mm256_srlv_epi64(r, shift1) & e;
    e &= _mm256_srlv_epi64(e, shift1);
    r |= _mm256_srlv_epi64(r, shift2) & e;
    e &= _mm256_srlv_epi64(e, shift2);
    r |= _mm256_srlv_epi64(r, shift3) & e;

    return _mm256_srlv_epi64(r, shift1) & mask;
}


QBB Pfill(QBB r, QBB e)
{
    static const QBB mask = { (long long)nota, (long long)noth, -1ll, (long long)nota };
    static const QBB shift1 = { 1, 7, 8, 9 };
    static const QBB shift2 = { 2, 14, 16, 18 };
    static const QBB shift3 = { 4, 28, 32, 36 };
    e &= mask;
    r |= _mm256_sllv_epi64(r, shift1) & e;
    e &= _mm256_sllv_epi64(e, shift1);
    r |= _mm256_sllv_epi64(r, shift2) & e;
    e &= _mm256_sllv_epi64(e, shift2);
    r |= _mm256_sllv_epi64(r, shift3) & e;

    return r;
}
QBB Nfill(QBB r, QBB e)
{
    static const QBB mask = { (long long)noth, (long long)nota, -1ll, (long long)noth };
    static const QBB shift1 = { 1, 7, 8, 9 };
    static const QBB shift2 = { 2, 14, 16, 18 };
    static const QBB shift3 = { 4, 28, 32, 36 };

    e &= mask;
    r |= _mm256_srlv_epi64(r, shift1) & e;
    e &= _mm256_srlv_epi64(e, shift1);
    r |= _mm256_srlv_epi64(r, shift2) & e;
    e &= _mm256_srlv_epi64(e, shift2);
    r |= _mm256_srlv_epi64(r, shift3) & e;

    return r;
}


BB movegen(QBB X, QBB O)
{
    return hor(nfill(X, O) | pfill(X, O));
}

int eva(BB x)
{
    static const BB border = 0xff000000000000ff | ~nota | ~noth;
    static const BB corner = set(0) | set(7) | set(63) | set(56);
    return _mm_popcnt_u64(x) + _mm_popcnt_u64(x & border) + 8 * _mm_popcnt_u64(x & corner);
}

BB bestmove;
BB KH[100];
bool flag = false;

BB getbestmove()
{
    return bestmove;
}

void resetKH()
{
    for (int i = 0; i < 100; ++i)
        KH[i] = 0;
}

bool checkvalid(BB x, BB o, BB move)
{
    if ((movegen(_mm256_set1_epi64x(x), _mm256_set1_epi64x(o)) & ~(o | x)) & move) return true;
    return false;
}
int node;

int getnode()
{
    return node;
}

int search(BB x, BB o, int alpha, int beta, int depth, int ply, bool flag)
{
    node++;
    if (depth <= 0) return eva(x) - eva(o);
    int best = -mate;
    const QBB O = _mm256_set1_epi64x(o);
    const QBB X = _mm256_set1_epi64x(x);
    BB allmove = movegen(X, O) & ~(o | x);
    if (allmove == 0)
    {
        if (flag == true) 
        {
            int countx = _mm_popcnt_u64(x);
            int counto = _mm_popcnt_u64(o);
            if (countx > counto) return mate;
            else if (countx < counto) return -mate;
            else return 0;
        }
        return -search(o, x, -beta, -alpha, depth - 1, ply + 1, true);
    }
    if (allmove & KH[ply])
    {
        BB move = KH[ply];
        QBB MOVE = _mm256_set1_epi64x(move);
        BB delta = hor((Nfill(MOVE, O) & Pfill(X, O)) | (Pfill(MOVE, O) & Nfill(X, O))) | move;
        int score = -search(o & ~delta, x | delta, -beta, -alpha, depth - 1, ply + 1, false);
        if (score > best)
        {
            best = score;
            alpha = std::max(score, alpha);
            if (ply == 0) bestmove = move;
            if (score >= beta) return score;
        }
        allmove &= ~move;
    }
    while (allmove)
    {
        BB move = allmove & -allmove;
        QBB MOVE = _mm256_set1_epi64x(move);
        BB delta = hor((Nfill(MOVE, O) & Pfill(X, O)) | (Pfill(MOVE, O) & Nfill(X, O))) | move;
        int score = -search(o & ~delta, x | delta, -beta, -alpha, depth - 1, ply + 1, false);
        if (score > best)
        {
            best = score;
            alpha = std::max(score, alpha);
            if (ply == 0) bestmove = move;
            if (score >= beta)
            {
                KH[ply] = move;
                return score;
            }
        }
        allmove &= ~move;
    }
    return best;

}
void makemove(BB& x, BB& o, BB move)
{
    QBB X = _mm256_set1_epi64x(x);
    QBB O = _mm256_set1_epi64x(o);
    QBB MOVE = _mm256_set1_epi64x(move);
    BB delta = hor((Nfill(MOVE, O) & Pfill(X, O)) | (Pfill(MOVE, O) & Nfill(X, O))) | move;
    x |= delta;
    o &= ~delta;

}
