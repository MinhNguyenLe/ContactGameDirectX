#include "StatBar.h"
#include"Game.h"
#include"Scene.h"

StatBar::StatBar(int type)
{
	this->type = type;
}

void StatBar::Render()
{
	CGame* game = CGame::GetInstance();
	int stat = game->Getheath();
	if (type == 1)
		stat = game->Getattack();
	int ani = 8 - stat / 100;
	if (ani > 8)
		ani = 8;
	if (ani < 0)
		ani = 0;
	animation_set->at(ani)->Render(x, y, 255, true);
	//RenderBoundingBox();
}
void StatBar::GetBoundingBox(float& l, float& t, float& r, float& b)
{
}