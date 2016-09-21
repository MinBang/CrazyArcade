#include "stdafx.h"
#include "BObject.h"


BObject::BObject(void)
{
}


BObject::~BObject(void)
{
}


int BObject::init(int x, int y, int type)
{
	this->x = x;
	this->y = y;
	this->width = OBJECT_SIZE;
	this->height = OBJECT_SIZE;
	this->type = type;

	return 0;
}


int BObject::draw(HDC hdc, HBITMAP image)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	BITMAP bit;
	int bx, by;

	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, image);
 
	GetObject(image, sizeof(BITMAP),&bit);

	bx = bit.bmWidth;
	by = bit.bmHeight;
 
	int width = 52;
	int height = 60;

	//BitBlt(hdc, x, y, width, height, MemDC, 0, 0, SRCCOPY);
	if(type == IMG_BLOCK_SIDE_TURTLE){

		TransparentBlt(hdc, x - 8, y - 23, width, height + 10, MemDC, 0, 0, bx, by, RGB(255,0, 255));
	}else if(type == IMG_BLOCK_CONER){

		TransparentBlt(hdc, x - 8, y - 28, width, height + 15, MemDC, 0, 0, bx, by, RGB(255,0, 255));
	}else if(type == IMG_BLOCK_BONG){
	
		TransparentBlt(hdc, x - 8, y - 28, width, height + 12, MemDC, 0, 0, bx, by, RGB(255,0, 255));
	}else{

		TransparentBlt(hdc, x - 8, y - 13, width, height, MemDC, 0, 0, bx, by, RGB(255,0, 255));
	}
	

	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);

	return 0;
}
