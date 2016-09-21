#include "stdafx.h"
#include "BResource.h"


BResource::BResource(void)
{
}


BResource::~BResource(void)
{
	images.clear();
}


int BResource::setImage(int type, HBITMAP image)
{
	images.insert(pair<int, HBITMAP>(type, image));

	return 0;
}


HBITMAP BResource::getImage(int type)
{
	return images.at(type);
}
