#pragma once

#include <WinSock2.h>
#include "BResource.h"
#include "../CrazyGameLib/BPlayer.h"
#include "BObject.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")

using namespace std;

class BGame
{

public:

	// window 관련 변수
	HWND m_hWnd;
	HINSTANCE m_hInst;

	// 서버주소정보
	char ip[128];
	int port;	

	//sock
	WSADATA wsaData;
	SOCKADDR_IN saddr;
	SOCKET s;

	char buf[BUF_SIZE];
	S_HEAD head;
	int hLen;

	// data

	char id[20];
	int serial;
	int my;
	int time;
	int gameState;
	char map[MAP_HEIGHT][MAP_WIDTH];
	BResource resources;
	HBITMAP tImage;
	HBITMAP tImage2;

	BPlayer players[MAX_USER_INROOM];
	vector<BObject> vObjects;

public:
	BGame(void);
	~BGame(void);

	void onCreate(HWND hWnd, HINSTANCE hInst, TCHAR ip[], int port);
	void onDraw(HDC hdc);
	int onTimer(int timerIndex);
	void onKeyEvent(int msg, int wParam);

	//SOCKET 관련함수
	int createServer(char* ip, int port);
	int sendContact(SOCKET s);

	//draw
	void regImage(void);
	void drawLogin(HDC hdc);
	void drawLoading(HDC hdc);
	void drawImage(HDC hdc, HBITMAP image, int x, int y);
	void drawRoom(HDC hdc);
	void playBgm(char* name);
	void stopBgm(void);
	void drawTransImage(HDC hdc, HBITMAP image, int x, int y, int width, int height);
	int sendAwaite(SOCKET s);
	int sendReady(SOCKET s);
	int onMouseEvent(int msg, int x, int y);
	int sendGameStart(SOCKET s);
	int drawGame(HDC hdc);
	int makeObject(void);
	int sendGameData(SOCKET s);
};

