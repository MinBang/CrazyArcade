#pragma once
class BObject
{
public:

	int x, y;
	int width, height;
	int type;

public:
	BObject(void);
	~BObject(void);
	int init(int x, int y, int type);
	int draw(HDC hdc, HBITMAP image);
};

