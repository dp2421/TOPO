#pragma once
class CScene;
class CCamera;
class CGameObject;
class Tile;
class CSceneMgr
{
	SINGLE(CSceneMgr)
private:
	CScene* m_pCurScene;
	CScene* m_pStartScene;
	CScene* m_pRacingScene;
	vector<Tile> tiles;
	SCENE_TYPE m_pSceneType;
public:
	void LoadMapInfoFromFile(const wstring& FileName, vector<Tile>& tiles);
	void InitMainScene();
	void InitStartScene();
	void InitUI();

	void InitScene() { m_pCurScene = m_pStartScene; }
	void Update();
	void Update_Tool();

	CScene* GetCurScene();
	SCENE_TYPE GetSceneType() { return m_pSceneType; }
	void ChangeScene(CScene* _pNextScene);
	void ChangeScene();

	void FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	CGameObject* AddNetworkGameObject(bool isPlayer, Vec3 pos);
	void RemoveNetworkGameObject(CGameObject* obj);
};

