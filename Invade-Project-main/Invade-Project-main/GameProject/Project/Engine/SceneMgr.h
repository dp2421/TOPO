#pragma once
class CScene;
class CCamera;
class CGameObject;
class CSceneMgr
{
	SINGLE(CSceneMgr)
private:
	CScene* m_pCurScene;
	CScene* m_pStartScene;
public:
	void Init();
	void Update();
	void Update_Tool();

	CScene* GetCurScene();
	void ChangeScene(CScene* _pNextScene);
	void FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	CGameObject* AddNetworkGameObject(bool isPlayer, Vec3 pos);
	void RemoveNetworkGameObject(CGameObject* obj);
};

