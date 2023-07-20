#pragma once

class CScene;
class CCamera;
class CGameObject;
class Tile;
#include "Texture.h"
#include "Ptr.h"

class CSceneMgr
{
	SINGLE(CSceneMgr)
private:
	CScene* m_pCurScene;
	CScene* m_pStartScene;
	CScene* m_pRacingScene;
	CScene* m_pMetorScene;
	CScene* m_pJumpingScene;
	CScene* m_pAwardScene;
	vector<Tile> tiles;
	vector<Tile> awardTiles;
	SCENE_TYPE m_pSceneType;
	Ptr<CTexture> m_pDaySkyBox;
	Ptr<CTexture> m_pNightSkyBox;


public:
	void LoadMapInfoFromFile(const wstring& FileName, vector<Tile>& tiles);
	void LoadMetorMapInfoFromFile(const wstring& FileName, vector<Tile>& tiles);

	void InitMainScene();
	void InitStartScene();
	void InitMetorScene();
	void InitJumpingScene();
	void InitAwardScene();
	void InitUI();

	void InitScene();
	void Update();
	void Update_Tool();

	CScene* GetCurScene();
	SCENE_TYPE GetSceneType() { return m_pSceneType; }
	void ChangeScene(CScene* _pNextScene);
	void ChangeScene(SCENE_TYPE _type);
	void ChangeScene();

	void FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	CGameObject* AddNetworkGameObject(bool isPlayer, Vec3 pos, CScene* curscene = nullptr);
	void RemoveNetworkGameObject(CGameObject* obj);

	Ptr<CTexture> GetNightSky() { return m_pNightSkyBox; }
	Ptr<CTexture> GetDaySky() { return m_pDaySkyBox; }

};

