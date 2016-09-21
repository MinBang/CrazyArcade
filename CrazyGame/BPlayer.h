#pragma once
class BPlayer
{
public:
	int serial;
	int room;
	int ready;
	char id[20];

	int x, y;
	int width, height;
	
	int dir;
	int type;
	bool isMove;

	int speed;
	int bombPower;

public:
	BPlayer(void);
	~BPlayer(void);
	void init(void);
};

