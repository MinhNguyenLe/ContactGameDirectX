#pragma once
#include "GameObject.h"
#include "DF.h"

class PlayerTankturret : public CGameObject
{
	int part = 0;
	int pre_ani = 0;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);

public:
	PlayerTankturret();
	virtual void SetState(int state);
};

