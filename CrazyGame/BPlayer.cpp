#include "stdafx.h"
#include "BPlayer.h"


BPlayer::BPlayer(void)
{
	init();
}


BPlayer::~BPlayer(void)
{
}


void BPlayer::init(void)
{
	serial = -1;
	room = -1;
	ready = 0;

	x = 0, y = 0;
	width = 0, height = 0;
	
	dir = DIR_DOWN;
	type = -1;
	isMove = false;

	speed = 5;
	bombPower = 1;

	for(int i=0;i<20;i++){

		id[i] = 0;
	}
}
