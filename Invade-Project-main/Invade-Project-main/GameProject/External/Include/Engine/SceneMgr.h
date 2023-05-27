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
	vector<Tile> tiles;
public:
	void LoadMapInfoFromFile(const wstring& FileName, vector<Tile>& tiles);
	void InitMainScene();
	void InitStartScene();
	void Update();
	void Update_Tool();

	CScene* GetCurScene();
	void ChangeScene(CScene* _pNextScene);
	void ChangeScene();

	void FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	CGameObject* AddNetworkGameObject(bool isPlayer, Vec3 pos);
	void RemoveNetworkGameObject(CGameObject* obj);
};

