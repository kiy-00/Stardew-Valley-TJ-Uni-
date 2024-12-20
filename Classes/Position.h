#ifndef __POSITION_H__
#define __POSITION_H__

struct Position {
	int x;
	int y;

	Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}

	bool operator<(const Position& other) const {
		if (x != other.x) return x < other.x;
		return y < other.y;
	}

	bool operator==(const Position& other) const {
		return x == other.x && y == other.y;
	}
};

#endif // __POSITION_H__