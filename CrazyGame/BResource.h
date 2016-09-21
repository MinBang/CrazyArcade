#pragma once
#include <hash_map>

using namespace std;

class BResource
{
public:
	BResource(void);
	~BResource(void);

	hash_map<int, HBITMAP> images;
	int setImage(int type, HBITMAP image);
	HBITMAP getImage(int type);
};

