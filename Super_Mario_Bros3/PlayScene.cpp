#include <iostream>
#include <fstream>

#include "PlayScene.h"


using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	key_handler = new CPlayScenceKeyHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/


void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	
		if (line == "[MAP]") { section = SCENE_SECTION_MAP; continue; }
		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[MAP]") { section = SCENE_SECTION_MAP; continue; }
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line == "[QUADTREE]") {
			section = SCENE_SECTION_QUADTREE; continue;
		}
		if (line == "[SETTING]") {
			section = SCENE_SECTION_SETTING; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_MAP: _ParseSection_MAP(line); break;
		case SCENE_SECTION_SETTING: _ParseSection_SETTING(line); break;
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_QUADTREE: _ParseSection_QUADTREE(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void CPlayScene::_ParseSection_QUADTREE(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring file_path = ToWSTR(tokens[0]);
	if (quadtree == NULL)
		quadtree = new CQuadTree(file_path.c_str());
}

void CPlayScene::_ParseSection_SETTING(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	setMapheight(int(atoi(tokens[0].c_str())));

	CGame::GetInstance()->GetCurrentScene()->setMapheight(int(atoi(tokens[0].c_str())));

	DebugOut(L"Y: la %d  \n", getMapheight());

}

void CPlayScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS]
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_SOPHIA:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] SOPHIA object was created before!\n");
			return;
		}
		obj = new CSOPHIA(x, getMapheight() - y);

		player = (CSOPHIA*)obj;

		DebugOut(L"[INFO] Player object created!\n");

		break;
	case OBJECT_TYPE_JASON:
		if (player2 != NULL)
		{
			DebugOut(L"[ERROR] JASON object was created before!\n");
			return;
		}
		obj = new JASON(x, getMapheight() - y);

		player2 = (JASON*)obj;

		DebugOut(L"[INFO] Player object created!\n");

		break;
	case OBJECT_TYPE_BRICK: obj = new CBrick(); break;
	case OBJECT_TYPE_CBOOM: obj = new CBOOM(); break;
	case OBJECT_TYPE_CTANKBULLET: obj = new CTANKBULLET(); break;
	case OBJECT_TYPE_NoCollisionObject: obj = new CNoCollisionObject(); break;
		
	case OBJECT_TYPE_TANK_WHEEL:
	{
		float part = atof(tokens[4].c_str());
		obj = new TANKWHEEL(part);
	}
	break;
	case OBJECT_TYPE_CLASER_BULLET:
	{
		obj = new CLASER_BULLET();
	}
	break;
	case OBJECT_TYPE_TANK_BODY:
	{
		obj = new TANKBODY();
	}
	break;
	case OBJECT_TYPE_JASON_BULLET_1:
	{
		obj = new CWAVE_BULLET();
	}
	break;

	case OBJECT_TYPE_TANK_TURRET:
	{
		obj = new TANKTURRET();
	}
	break;

	case OBJECT_TYPE_EFFECT:
	{
		float time = atof(tokens[4].c_str());
		obj = new EFFECT(time);
	}
	break;
	case OBJECT_TYPE_JASON_GRENADE:
	{
		obj = new CGRENADE();
	}
	break;
	case OBJECT_TYPE_CGX_BULLET:
	{
		obj = new CGX_BULLET();
	}
	break;
	case OBJECT_TYPE_PORTAL:
	{
		float r = atof(tokens[4].c_str());
		float b = atof(tokens[5].c_str());
		int scene_id = atoi(tokens[6].c_str());
		obj = new CPortal(x, y, r, b, scene_id);
	}
	case OBJECT_TYPE_CINTERCRUPT_BULLET: obj = new CINTERRUPT_BULLET(); break;
	case OBJECT_TYPE_RED_WORM: obj = new CREDWORM(); break;
		
	break;
	
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup


	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	if (obj != NULL )
	{
		if(object_type != OBJECT_TYPE_SOPHIA)
		obj->SetPosition(x, getMapheight() - y);
		obj->SetAnimationSet(ani_set);
		obj->SetOrigin(x, y, obj->GetState());
		obj->SetisOriginObj(true);
		objects.push_back(obj);
	}
}

void CPlayScene::_ParseSection_MAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 7) return; // skip invalid lines

	int idTileSet = atoi(tokens[0].c_str());
	int totalRowsTileSet = atoi(tokens[1].c_str());
	int totalColumnsTileSet = atoi(tokens[2].c_str());
	int totalRowsMap = atoi(tokens[3].c_str());
	int totalColumnsMap = atoi(tokens[4].c_str());
	int totalTiles = atoi(tokens[5].c_str());

	setMapheight(int(atoi(tokens[3].c_str())) * 32);

	CGame::GetInstance()->GetCurrentScene()->setMapheight(int(atoi(tokens[3].c_str())) * 32);

	wstring file_path = ToWSTR(tokens[6]);

	map = new Map(idTileSet, totalRowsTileSet, totalColumnsTileSet, totalRowsMap, totalColumnsMap, totalTiles);
	map->LoadMap(file_path.c_str());
	map->ExtractTileFromTileSet();
}

bool CPlayScene::IsInUseArea(float Ox, float Oy)
{
	float CamX, CamY;

	CamX = (float)CGame::GetInstance()->GetCam().GetCamX();

	CamY = (float)CGame::GetInstance()->GetCam().GetCamY();

	if (((CamX < Ox) && (Ox < CamX + IN_USE_WIDTH + CAM_X_BONUS)) && ((CamY < Oy) && (Oy < CamY + IN_USE_HEIGHT)))
		return true;
	return false;
}

bool CPlayScene::IsInside(float Ox, float Oy, float xRange, float yRange, float tx, float ty)
{
	if (Ox <= tx && tx <= xRange && Oy <= ty && ty <= yRange)
		return true;
	return false;
}

void CPlayScene::Update(DWORD dt)
{
	// We know that SOPHIA is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	// skip the rest if scene was already unloaded (SOPHIA::Update might trigger PlayScene::Unload)
	if (player == NULL && player2 == NULL) return;

	// Update camera to follow mario
	float cx, cy;

	if(player)
		player->GetPosition(cx, cy);
	else
		player2->GetPosition(cx, cy);

	cy = cy;

	/*DebugOut(L"Y: la %d %f  \n", CGame::GetInstance()->GetCurrentScene()->getMapheight(), cy);*/
	
	CGame* game = CGame::GetInstance();

	cx -= game->GetScreenWidth() / 2;
	cy -= game->GetScreenHeight() / 2;

	if (cx < 0)
	{
		cx = 0;
	}

	CGame::GetInstance()->SetCamPos(cx, cy);

	vector<LPGAMEOBJECT> coObjects;

	quadtree->GetObjects(objects, (int)cx, (int)cy);

	for (size_t i = 0; i < objects.size(); i++)
	{
		float Ox, Oy;
		objects[i]->GetPosition(Ox, Oy);
		if (!IsInUseArea(Ox, Oy) && !objects[i]->GetisOriginObj())
		{
			objects[i]->SetActive(false);
			objects.erase(objects.begin() + i);
			i--;
		}
	}

	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}
}

void CPlayScene::Render()
{
	CGame* game = CGame::GetInstance();

	if (map)
	{
		this->map->Render();
	}
	for (int i = 0; i < objects.size(); i++)
		objects[i]->Render();
}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	objects.clear();

	player = NULL;

	player2 = NULL;

	delete map;

	map = nullptr;

	quadtree->Unload();

	quadtree = nullptr;

	delete quadtree;

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	if (((CPlayScene*)scence)->GetPlayer())
	{
		CSOPHIA* player = ((CPlayScene*)scence)->GetPlayer();
		switch (KeyCode)
		{
		case DIK_SPACE:
			player->SetState(SOPHIA_STATE_JUMP);
			break;
		case DIK_B:
			player->Reset();
			break;
		case DIK_A:
			player->SetisFiring(true);
			break;
		}
	}

	else {
		JASON* player = ((CPlayScene*)scence)->GetPlayer2();
		switch (KeyCode)
		{
		case DIK_SPACE:
			player->SetState(SOPHIA_STATE_JUMP);
			break;
		case DIK_B:
			player->Reset();
			break;
		case DIK_A:
			player->SetisFiring(true);
			break;
		}
	}
}

void CPlayScenceKeyHandler::OnKeyUp(int KeyCode)
{
	if (((CPlayScene*)scence)->GetPlayer())
	{
		CSOPHIA* player = ((CPlayScene*)scence)->GetPlayer();
		switch (KeyCode)
		{
		case DIK_A:
			player->SetisFiring(false);
			break;
		case DIK_R:
			CGame::GetInstance()->SwitchScene(2);
			break;
		case DIK_H:
			CGame::GetInstance()->SwitchScene(1);
			break;
		}
	}
		
	else {
		JASON* player = ((CPlayScene*)scence)->GetPlayer2();
		switch (KeyCode)
		{
		case DIK_A:
			player->SetisFiring(false);
			break;
		case DIK_R:
			CGame::GetInstance()->SwitchScene(2);
			break;
		case DIK_H:
			CGame::GetInstance()->SwitchScene(1);
			break;
		}
	}
		
}

void CPlayScenceKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();

	if (((CPlayScene*)scence)->GetPlayer())
	{
		CSOPHIA* player = ((CPlayScene*)scence)->GetPlayer();
		// disable control key when SOPHIA die 
		if (player->GetState() == SOPHIA_STATE_DIE) return;
		if (game->IsKeyDown(DIK_RIGHT))
			player->SetState(SOPHIA_STATE_WALKING_RIGHT);
		else if (game->IsKeyDown(DIK_LEFT))
			player->SetState(SOPHIA_STATE_WALKING_LEFT);
		else if (game->IsKeyDown(DIK_DOWN))
			player->SetState(SOPHIA_STATE_WALKING_DOWN);
		else if (game->IsKeyDown(DIK_UP))
			player->SetState(SOPHIA_STATE_WALKING_UP);
		else
			player->SetState(SOPHIA_STATE_IDLE);
	}

	else {
		JASON* player = ((CPlayScene*)scence)->GetPlayer2();
		// disable control key when SOPHIA die 
		if (player->GetState() == SOPHIA_STATE_DIE) return;
		if (game->IsKeyDown(DIK_RIGHT))
			player->SetState(SOPHIA_STATE_WALKING_RIGHT);
		else if (game->IsKeyDown(DIK_LEFT))
			player->SetState(SOPHIA_STATE_WALKING_LEFT);
		else if (game->IsKeyDown(DIK_DOWN))
			player->SetState(SOPHIA_STATE_WALKING_DOWN);
		else if (game->IsKeyDown(DIK_UP))
			player->SetState(SOPHIA_STATE_WALKING_UP);
		else if (game->IsKeyDown(DIK_Q))
			player->SwitchWeapon();
		else
			player->SetState(SOPHIA_STATE_IDLE);
	}


}