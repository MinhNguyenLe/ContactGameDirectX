#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "SoPhia.h"
#include "Eye.h"
#include "Koopas.h"
#include "Map.h"
#include "TankWheel.h"
#include "MapObj.h"
#include "CLaserGuard.h"
#include "BallCarry.h"
#include "BallBot.h"
#include "CDRAP.h"
#include "CGX680.h"
#include "CGX680S.h"
#include "CSTUKA.h"
#include "Eyelet.h"
#include "Interrupt.h"
#include "TankBullet.h"
#include "CEvenType1.h"
#include "CinterruptBullet.h"
#include "CredWorm.h"
#include "TankBody.h"
#include "TankTurret.h"
#include "EFFECT.h"
#include "Jason.h"
#include "CBOOM.h"
#include "NoCollisionObject.h"

#include "Utils.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "DF.h"
#include "WaveBullet.h"
#include "CGRENADE.h"
#include "CGX_BULLET.h"
#include "ClaserBullet.h"
#include "MapCamera.h"
#include "StatBar.h"
#include "MiniJason.h"
#include "TankDoor.h"
#include "Items.h"

#define QUADTREE_SECTION_SETTINGS	1
#define QUADTREE_SECTION_OBJECTS	2
#define MAX_QUADTREE_LINE 1024

class CQuadTree
{
	bool isLeaf = false;
	float x = 0;
	float y = 0;
	float cellWidth = 0;
	float  cellHeight = 0;
	CQuadTree* BrachTL = NULL;
	CQuadTree* BrachTR = NULL;
	CQuadTree* BrachBL = NULL;
	CQuadTree* BrachBR = NULL;
	MapObj* obj;
	vector<LPGAMEOBJECT> listObjects;
	

	void _ParseSection_SETTINGS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_MapObj(string line);
public:
	CQuadTree(float cellWidth, float cellHeight, float x, float y);
	CQuadTree(LPCWSTR filePath);
	void GetObjects(vector<LPGAMEOBJECT>& listObject, int playerX, int playerY);
	void Load(LPCWSTR filePath);
	void Unload();
	void Add(CGameObject* obj)
	{
		listObjects.push_back(obj);
	}
	int getVollunm(){
		return listObjects.size();
	}
	void Render();
	void Plit();
	void ObjectPlit(CQuadTree* brach);
	bool inRange(float ox, float oy, float x, float y, float width, float height);
	void Pop(vector<LPGAMEOBJECT>& listObject, int CamX, int CamY);

};

class CPlayScene : public CScene
{
protected:
	SoPhia* player;				// A play scene has to have player, right? 
	Jason* player2;
	MiniJason* player3;
	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> secondLayer;
	int mapHeight;
	Map* map;
	CQuadTree* quadtree;
	vector<CEvenType1*> InterruptBulletMng ;
	vector<CEvenType1*> WormSpamMng;
	vector<CEvenType1*> KaboomMng;
	vector<CEvenType1*> BoomCarryMng;
	vector<CEvenType1*> CGXMng;
	vector<CEvenType1*> ItemsMng;
	vector<MapCamera*> MapCam;
	
	int filming_duration = 1000;
	DWORD filming_start = 0;

	int camState = 0;

	bool piloting = true;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_MAP(string line);
	void _ParseSection_QUADTREE(string line);
	void _ParseSection_SETTING(string line);
	void _ParseSection_MAPCAM(string line);
public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	bool IsInUseArea(float Ox, float Oy);
	bool IsInside(float Ox, float Oy, float xRange, float yRange, float tx, float ty);

	SoPhia* GetPlayer() { return player; }
	Jason* GetPlayer2() { return player2; }
	MiniJason* GetPlayer3() { return player3; }

	void setpiloting(int value)
	{
		piloting = value;
	}

	int getpiloting()
	{
		return piloting;
	}
	void StartFilming()
	{
		if (filming_start == 0)
			filming_start = (DWORD)GetTickCount64();
	}
	void setCamState(int value)
	{
		camState = value;
	}

	int getCamState()
	{
		return camState;
	}
	void setMapheight(int height)
	{
		mapHeight = height;
	}

	int getMapheight()
	{
		return mapHeight;
	}
	
	/////////////////ItemsMng
	void AddItemsMng(float x, float y, int num)
	{
		CEvenType1* obj = new CEvenType1(x, y, num);
		this->ItemsMng.push_back(obj);
	}
	CEvenType1* GetItemsMng()
	{
		return ItemsMng.at(0);
	}
	bool CheckItemsMng()
	{
		if (ItemsMng.size() != 0)
			return true;
		return false;
	}
	void DeleteItemsMng()
	{
		this->ItemsMng.erase(ItemsMng.begin());
	}
	/////////////////CGXMng
	void AddCGXMng(float x, float y, float vx, float vy)
	{
		CEvenType1* obj = new CEvenType1(x, y, 0, vx, vy);
		this->CGXMng.push_back(obj);
	}
	CEvenType1* GetCGXMng()
	{
		return CGXMng.at(0);
	}
	bool CheckCGXMng()
	{
		if (CGXMng.size() != 0)
			return true;
		return false;
	}
	void DeleteCGXMng()
	{
		this->CGXMng.erase(CGXMng.begin());
	}
	/////////////////BoomCarryMng
	void AddBoomCarryMng(float x, float y)
	{
		CEvenType1* obj = new CEvenType1(x, y);
		this->BoomCarryMng.push_back(obj);
	}
	void CheckStackBoomCarryMng()
	{
		if (BoomCarryMng.at(0)->getCEventStack() < 4)
		{
			BoomCarryMng.at(0)->setCEventStack(BoomCarryMng.at(0)->getCEventStack() + 1);
		}
		else 
		{
			DeleteBoomCarryMng();
		}
	}
	CEvenType1* GetBoomCarryMng()
	{
		return BoomCarryMng.at(0);
	}
	bool CheckBoomCarryMng()
	{
		if (BoomCarryMng.size() != 0)
			return true;
		return false;
	}
	void DeleteBoomCarryMng()
	{
		this->BoomCarryMng.erase(BoomCarryMng.begin());
	}
	/////////////////KaboomMng
	void AddKaboomMng(float x, float y)
	{
		CEvenType1* obj = new CEvenType1(x, y);
		this->KaboomMng.push_back(obj);
	}
	CEvenType1* GetKaboomMng()
	{
		return KaboomMng.at(0);
	}
	bool CheckKaboomMng()
	{
		if (KaboomMng.size() != 0)
			return true;
		return false;
	}
	void DeleteKaboomMng()
	{
		this->KaboomMng.erase(KaboomMng.begin());
	}
	/////////////////InterruptBulletMng
	void AddInterruptBulletMng(float x, float y)
	{
		CEvenType1* obj = new CEvenType1(x, y);
		this->InterruptBulletMng.push_back(obj);
	}
	CEvenType1* GetInterruptBulletMng()
	{
		return InterruptBulletMng.at(0);
	}
	bool CheckInterruptBulletMng()
	{
		if (InterruptBulletMng.size() != 0)
			return true;
		return false;
	}
	void DeleteInterruptBulletMng()
	{
		this->InterruptBulletMng.erase(InterruptBulletMng.begin());
	}
	//////////////////////////WormSpamMng
	void AddWormSpamMng(float x, float y)
	{
		CEvenType1* obj = new CEvenType1(x, y);
		this->WormSpamMng.push_back(obj);
	}
	CEvenType1* GetWormSpamMng()
	{
		return WormSpamMng.at(0);
	}
	bool CheckWormSpamMng()
	{
		if (WormSpamMng.size() != 0)
			return true;
		return false;
	}
	void DeleteWormSpamMng()
	{
		this->WormSpamMng.erase(WormSpamMng.begin());
	}
	///////////////////////////////
	//friend class CPlayScenceKeyHandler;
};

class CPlayScenceKeyHandler : public CScenceKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlayScenceKeyHandler(CScene* s) :CScenceKeyHandler(s) {};
};

