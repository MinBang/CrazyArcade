#include "stdafx.h"
#include "BGame.h"
#include <mmsystem.h>
#include <process.h>

// timerIndex 정의
#define TIMER_MAINFRAME 1
#define WAIT_TIME 30

// KEY 정의
#define KEY_SPACE VK_SPACE
#define KEY_ENTER VK_RETURN
#define KEY_LEFT  VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_DOWN  VK_DOWN
#define KEY_UP    VK_UP

int tKeyDown = 0;

BGame::BGame(void)
:serial(0),
my(-1)
{
	hLen = sizeof(head);
	gameState = GSTATE_LOGIN;

	//
	ZeroMemory(id, sizeof(id));
	strcpy_s(id, "test");
}


BGame::~BGame(void)
{
}


void BGame::onCreate(HWND hWnd, HINSTANCE hInst, TCHAR ip[], int port)
{
	SetTimer(hWnd, 1, 16, NULL);

	m_hWnd = hWnd;
	m_hInst = hInst;

	//wmemcpy(this->ip, ip, 128);
	WideCharToMultiByte(CP_ACP, 0, ip, 128, this->ip, 128, NULL, NULL);
	this->port = port;

	// server생성
	//createServer(this->ip, port);
	createServer("127.0.0.1", 2016);

	regImage();

	playBgm(SOUND_LOGIN);
}

int BGame::makeObject(void)
{
	for(int i=0;i<MAP_HEIGHT;i++){
		for(int j=MAP_WIDTH-1; 0<=j ;j--){

			if(map[i][j] != 0){

				int sx = START_X + j * OBJECT_SIZE;
				int sy = START_Y + i * OBJECT_SIZE;

				BObject tObject;
				tObject.init(sx, sy, map[i][j]);

				vObjects.push_back(tObject);
			}
		}
	}

	return 0;
}

void BGame::playBgm(char* name)
{
	TCHAR music[256];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, strlen(name)+1, music, 256);

	PlaySound(music, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void BGame::stopBgm(void)
{
	PlaySound(NULL, 0, 0);
}





//draw

void BGame::regImage(void)
{
	resources.setImage(IMG_MENU_LOGIN, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP1)));
	resources.setImage(IMG_MENU_LOADING, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP2)));
	resources.setImage(IMG_MENU_LOADING2, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP4)));
	resources.setImage(IMG_MENU_ROOM, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP3)));
	resources.setImage(IMG_CHRT_BAZZIFACE, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP5)));
	resources.setImage(IMG_CHRT_DAOFACE, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP6)));
	resources.setImage(IMG_CHRT_KEPHIFACE, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP7)));
	resources.setImage(IMG_CHRT_MARIDFACE, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP8)));

	resources.setImage(IMG_MAP_BACKGROUND, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP9)));
	resources.setImage(IMG_BLOCK_STATIC, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP10)));
	resources.setImage(IMG_BLOCK_PUSH, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP11)));
	resources.setImage(IMG_BLOCK_SIDE_TURTLE, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP12)));
	resources.setImage(IMG_BLOCK_SIDE_NORMAL, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP13)));
	resources.setImage(IMG_BLOCK_CENTER_SAIL, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP14)));
	resources.setImage(IMG_BLOCK_CONER, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP15)));
	resources.setImage(IMG_BLOCK_BONG, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP16)));

	resources.setImage(IMG_CHRT_BAZZI, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP17)));
	resources.setImage(IMG_CHRT_KEPHI, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP18)));
	resources.setImage(IMG_CHRT_MARID, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP19)));
	resources.setImage(IMG_CHRT_DAO, LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BITMAP20)));
}


void BGame::drawImage(HDC hdc, HBITMAP image, int x = 0, int y = 0)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx,by;
	BITMAP bit;

	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, image);
 
	GetObject(image,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;
 
	BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCPAINT );
 
	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}

void BGame::drawTransImage(HDC hdc, HBITMAP image, int x = 0, int y = 0, int width = 0 , int height = 0)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx,by;
	BITMAP bit;

	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, image);
 
	GetObject(image,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;
 
	//BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCPAINT );
	TransparentBlt(hdc, x, y, bx, by, MemDC, 0, 0, bx, by, RGB(255, 0, 255));
 
	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}

void BGame::drawLogin(HDC hdc)
{
	tImage = resources.getImage(IMG_MENU_LOGIN);

	drawImage(hdc, tImage);
}


void BGame::drawLoading(HDC hdc)
{
	tImage = resources.getImage(IMG_MENU_LOADING);
	tImage2 = resources.getImage(IMG_MENU_LOADING2);

	drawImage(hdc, tImage);
	drawTransImage(hdc, tImage2, 165, 200);
}


void BGame::drawRoom(HDC hdc)
{
	tImage = resources.getImage(IMG_MENU_ROOM);

	drawImage(hdc, tImage);

	for(int i=0;i<MAX_USER_INROOM;i++){

		if(players[i].type != -1){

			int sx = i * 150 + 100;

			tImage = resources.getImage(players[i].type);
			drawTransImage(hdc, tImage, sx, 200);

			if(players[i].ready == 1){

				Rectangle(hdc, sx, 350, sx + 90, 350 + 90); 
			}

			if(players[0].serial == serial && i == 0){

				Rectangle(hdc, 100, 450, 100 + 100, 450 + 100); 
			}
		}
	}

	
}

int BGame::drawGame(HDC hdc)
{
	//Rectangle(hdc,0, 0, 200, 200);

	tImage = resources.getImage(IMG_MAP_BACKGROUND);
	drawImage(hdc, tImage);

	for(int i=0;i<vObjects.size();i++){

		BObject tObject = vObjects[i];
		//Rectangle(hdc, tObject.x, tObject.y, tObject.x + tObject.width, tObject.y + tObject.height);

		tImage = resources.getImage(tObject.type);

		if(tObject.type != IMG_BLOCK_CENTER_SAIL){

			tObject.draw(hdc, tImage);
		}
		
	}

	tImage = resources.getImage(IMG_BLOCK_CENTER_SAIL);

	drawTransImage(hdc, tImage, 240, 150, 200, 200);

	for(int i=0;i<MAX_USER_INROOM;i++){

		/*if(players[i].serial != -1){

			Rectangle(hdc, players[i].x, players[i].y, players[i].x + players[i].width, players[i].y + players[i].height); 
		}*/

		if(players[i].serial != -1){

			tImage = resources.getImage(players[i].type);
			players[i].draw(hdc, tImage);
		}		
	}

	return 0;
}


void BGame::onDraw(HDC hdc)
{
	switch(gameState){
		
	case GSTATE_LOGIN:

		drawLogin(hdc);

		break;

	case GSTATE_LOADING:
	case GSTATE_LOADING_WAIT:
		
		drawLoading(hdc);

		break;

	case GSTATE_ROOM:
	case GSTATE_ROOM_WAIT:

		drawRoom(hdc);
		
		break;

	case GSTATE_RUN:

		drawGame(hdc);

		break;

	default:
		break;
	}
}







// Evnet


int BGame::onTimer(int timerIndex)
{
	if(timerIndex == TIMER_MAINFRAME){

		switch(gameState){

		case GSTATE_LOADING:

			time++;

			if(WAIT_TIME <= time){

				sendAwaite(s);

				gameState = GSTATE_LOADING_WAIT;

				stopBgm();
				playBgm(SOUND_ROOM);

				time = 0;
			}

			break;

		case GSTATE_RUN:

			sendGameData(s);

			time++;

			if(4 <= time){

				for(int i=0;i<MAX_USER_INROOM;i++){
					if(players[i].isMove == true){

						players[i].move();
					}
				}

				time = 0;
			}

			break;

		default:
			break;
		}

		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	return 0;
}


void BGame::onKeyEvent(int msg, int wParam)
{
	if(gameState == GSTATE_LOGIN){

		if(WM_KEYDOWN == msg){

			switch(wParam){

			case KEY_ENTER:

				gameState = GSTATE_LOADING;

				break;

			default:

				break;
			}
		}
	}else if(gameState == GSTATE_ROOM){

		if(WM_KEYDOWN == msg){

			switch(wParam){

			case KEY_ENTER:

				if(tKeyDown == 0){

					sendReady(s);
				}		

				tKeyDown++;

				break;

			default:

				break;
			}
		}else if(WM_KEYUP == msg){

			tKeyDown = 0;
		}

	}else if(gameState == GSTATE_RUN){

		if(WM_KEYDOWN == msg){

			switch(wParam){

			case KEY_UP:
				players[my].isMove = true;
				players[my].y -= players[my].speed;
				players[my].dir = DIR_UP;

				break;

			case KEY_DOWN:
				players[my].isMove = true;
				players[my].y += players[my].speed;
				players[my].dir = DIR_DOWN;

				break;

			case KEY_LEFT:
				players[my].isMove = true;
				players[my].x -= players[my].speed;
				players[my].dir = DIR_LEFT;

				break;

			case KEY_RIGHT:
				players[my].isMove = true;
				players[my].x += players[my].speed;
				players[my].dir = DIR_RIGHT;

				break;

			default:
				break;
			}

		}else if(WM_KEYUP == msg){

			players[my].isMove = false;
		}
	}
}

int BGame::onMouseEvent(int msg, int x, int y)
{
	if(gameState == GSTATE_ROOM){

		if(msg == WM_LBUTTONDOWN){

			if(players[0].serial == serial &&
				100 < x && x < 200 &&
				450 < y && y < 550){

				sendGameStart(s);
			}
		}
	}

	return 0;
}


// SOCKET 관련함수들

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int readLen = 0;

	while(true){

		readLen += recv(s, &buf[readLen], len, 0);
		if(readLen >= len){

			break;
		}
	}

	return 0;
}


int recvContact(SOCKET s, BGame* pGame){

	S_RCONTACT rData;
	
	recvn(s, (char*)&rData, sizeof(rData), 0);

	// data 처리
	pGame->serial = rData.serial;

	return 0;
}

int recvAwaite(SOCKET s, BGame* pGame)
{
	S_RAWAITE rData;

	recvn(s, (char*)&rData, sizeof(rData), 0);

	for(int i=0;i<MAX_USER_INROOM;i++){

		pGame->players[i].type = rData.awaiters[i].chrtType;
		pGame->players[i].ready = rData.awaiters[i].ready;
		pGame->players[i].serial = rData.awaiters[i].serial;
		pGame->players[i].room = rData.awaiters[i].room;
		strcpy_s(pGame->players[i].id, rData.awaiters[i].id);
	}

	return 0;
}

int recvGameData(SOCKET s, BGame* pGame)
{
	S_RGAMEDATA rData;

	recvn(s, (char*)&rData, sizeof(rData), 0);

	memcpy(pGame->map, rData.map, sizeof(rData.map));
	
	for(int i=0;i<MAX_USER_INROOM;i++){

		pGame->players[i] = rData.player[i];
	}

	if(pGame->my < 0){
		pGame->my = rData.num;
	}	

	return 0;
}

unsigned int WINAPI threadMain(LPVOID game){

	BGame* pGame = (BGame*)game;
	SOCKET s = pGame->s;
	S_HEAD head;

	while(true){

		recvn(s, (char*)&head, sizeof(head), 0);
		
		switch(head.dataType){

		case SC_CONTACT_FIRST:

			recvContact(s, pGame);

			break;

		case SC_AWAITE:

			recvAwaite(s, pGame);

			pGame->gameState = GSTATE_ROOM;

			break;

		case SC_GAMEDATA:

			recvGameData(s, pGame);

			if(pGame->gameState == GSTATE_ROOM){

				pGame->makeObject();
				pGame->gameState = GSTATE_RUN;
			}

			break;

		default:
			//MessageBox(pGame->m_hWnd, TEXT("unknown"), TEXT("GOOD"), 0);
			break;
		}
	}

	closesocket(pGame->s);
	WSACleanup();

	return 0;
}

int BGame::createServer(char* ip, int port)
{
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){

		exit(1);
	}

	s = socket(PF_INET, SOCK_STREAM, 0);
	if(s == INVALID_SOCKET){

		exit(1);
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip);
	saddr.sin_port = port;

	if(connect(s, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR){

		exit(1);
	}

	_beginthreadex(NULL, 0, threadMain, (LPVOID)this, 0, NULL);	

	sendContact(s);

	return 0;
}


int BGame::sendContact(SOCKET s)
{
	int dLen = 0;

	head.dataType = CS_CONTACT_FIRST;
	head.dLen = dLen;

	ZeroMemory(buf, sizeof(buf));
	memcpy(buf, (char*)&head, hLen);

	int re = send(s, buf, hLen + dLen, 0);	

	return re;
}

int BGame::sendAwaite(SOCKET s)
{
	S_AWAITE sData;
	int dLen = sizeof(sData);

	head.dataType = CS_AWAITE;
	head.dLen = dLen;

	sData.serial = serial;
	strcpy_s(sData.id, id);

	ZeroMemory(buf, sizeof(buf));
	memcpy(buf, (char*)&head, hLen);
	memcpy(buf + hLen, (char*)&sData, dLen);

	int re = send(s, buf, hLen + dLen, 0);	

	return re;
}


int BGame::sendReady(SOCKET s)
{
	S_READY sData;
	int dLen = sizeof(sData);

	head.dataType = CS_READY;
	head.dLen = dLen;

	sData.serial = serial;
	sData.ready = 1;
	sData.roomNum = players[0].room;

	ZeroMemory(buf, sizeof(buf));
	memcpy(buf, (char*)&head, hLen);
	memcpy(buf + hLen, (char*)&sData, dLen);

	int re = send(s, buf, hLen + dLen, 0);	

	return re;
}

int BGame::sendGameStart(SOCKET s)
{
	S_GAMESTART sData;
	int dLen = sizeof(sData);

	head.dataType = CS_GAMESTART;
	head.dLen = dLen;

	sData.room = players[0].room;

	ZeroMemory(buf, sizeof(buf));
	memcpy(buf, (char*)&head, hLen);
	memcpy(buf + hLen, (char*)&sData, dLen);

	int re = send(s, buf, hLen + dLen, 0);	

	return re;
}

int BGame::sendGameData(SOCKET s)
{
	if(my < 0){

		return 0;
	}

	S_GAMEDATA sData;
	int dLen = sizeof(sData);

	head.dataType = CS_GAMEDATA;
	head.dLen = dLen;

	sData.num = my;
	sData.player = players[my];

	ZeroMemory(buf, sizeof(buf));
	memcpy(buf, (char*)&head, hLen);
	memcpy(buf + hLen, (char*)&sData, dLen);

	int re = send(s, buf, hLen + dLen, 0);	

	return re;
}






















