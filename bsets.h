#ifndef __FASTICA_BSETS_H
#define __FASTICA_BSETS_H
//--------------------------------------------------------------------------
#include <time.h>
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
// BitSet class
class BitSet
{
protected:
	int size;
	std::vector<UC> bset;
	BitSet() {}
public:
	BitSet(int nmax)
	{
		size = (nmax + 7) / 8;
		bset = std::vector<UC>(size, 0);
	}

	~BitSet() {}

	void operator += (int bit)
	{
		bset[bit / 8] |= (UC)(1 << (bit % 8));
	}

	void operator -= (int bit)
	{
		bset[bit / 8] &= ~(UC)(1 << (bit % 8));
	}

	bool operator [] (int bit)
	{
		return (bset[bit / 8] & (UC)(1 << (bit % 8))) != 0;
	}

	void clear()
	{
		memset(&bset[0], 0, size);
	}

	void fill()
	{
		memset(&bset[0], 0xFF, size);
	}
};
//--------------------------------------------------------------------------
// Random BitSet class using in sampling ICA sources
class RandomSet: public BitSet
{
private:
	int cidx;
	int rmax;
public:
	RandomSet(int nmax, int nrandom) : BitSet(nmax), cidx(0), rmax(nmax)
	{
		generate(nrandom);
	}

	RandomSet(const RandomSet &rs) : cidx(0)
	{
		size = rs.size;
		rmax = rs.rmax;
		bset = rs.bset;
	}

	u64 random()
	{
		u64 R0 = (u64)rand() << 48;
		u64 R1 = (u64)rand() << 48 >> 16;
		u64 R2 = (u64)rand() << 48 >> 32;
		u64 R3 = (u64)rand() << 48 >> 48;
		return (R0 | R1 | R2 | R3);
	}

	void generate(int nrandom)
	{
		cidx = 0;
		if (nrandom > rmax) {
			fill();
			return;
		}
		clear();
		srand((unsigned int)time(0));
		for (int n = 0; n < nrandom; n++) {
			*this += (int)(random() % rmax);
		}
	}

	int next()
	{
		while (cidx < rmax) {
			if ((*this)[cidx]) {
				int n = cidx++;
				return n;
			}
			cidx += 1;
		}
		cidx = 0;
		return -1;
	}
};
//--------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
