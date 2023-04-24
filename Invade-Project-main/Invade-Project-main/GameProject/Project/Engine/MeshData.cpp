#include "pch.h"
#include "MeshData.h"

#include "PathMgr.h"
#include "Mesh.h"
#include "ResMgr.h"
#include "Material.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator3D.h"
#include "GameObject.h"

CMeshData::CMeshData()
	: CResource(RES_TYPE::MESHDATA)
{
}

CMeshData::~CMeshData()
{
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.Init();
	loader.LoadFbx(strFullPath);

	// �޽� ��������
	vector<Ptr<CMesh>> vecMesh;
	CMesh* pMesh{};
	wstring meshcnt = L"";
	for (int i = 0; i < loader.GetContainerCount(); ++i)
	{
		pMesh = CMesh::CreateFromContainer(loader, i);
		wstring strMeshName = L"Mesh\\";
		strMeshName += CPathMgr::GetFileName(strFullPath.c_str());
		meshcnt = L"_" + std::to_wstring(i);
		strMeshName += meshcnt;
		strMeshName += L".mesh";
		pMesh->SetName(strMeshName);
		pMesh->SetPath(strMeshName);

		vecMesh.push_back(pMesh);
		CResMgr::GetInst()->AddRes<CMesh>(pMesh->GetName(), pMesh);
	}
	


	vector<Ptr<CMaterial>> vecMtrl;

	// ���׸��� ��������

		for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
		{
			// ����ó�� (material �̸��� �Է� �ȵǾ����� ���� �ִ�.)
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
			vecMtrl.push_back(pMtrl);
		}

	CMeshData* pMeshData = new CMeshData;
	pMeshData->m_vecMesh = vecMesh;
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}

void CMeshData::Load(const wstring& _strFilePath, bool _bFBX, bool _bVecMesh)
{

	wstring meshname = CPathMgr::GetFileName(_strFilePath.c_str());
	if (_bVecMesh == false)
	{
		FILE* pFile = NULL;
		_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
		UINT iMtrlCount = 0;

		wstring strMeshKey, strMeshPath, strMeshSize;
		strMeshSize = LoadWString(pFile);
		strMeshKey = LoadWString(pFile);
		strMeshPath = LoadWString(pFile);
		m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);
		m_vecMesh.push_back(m_pMesh); // �޽� ���� �о������ ��.

		//UINT iMtrlCount = 0;
		fread(&iMtrlCount, sizeof(UINT), 1, pFile);
		m_vecMtrl.resize(iMtrlCount);

		for (UINT i = 0; i < iMtrlCount; ++i)
		{
			UINT idx = -1;
			fread(&idx, 4, 1, pFile);
			if (idx == -1)
				break;

			wstring strKey = LoadWString(pFile);
			wstring strPath = LoadWString(pFile);

			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey, strPath, true);
			m_vecMtrl[i] = pMtrl;
		}
		fclose(pFile);

	}
	else
	{
		FILE* pFile = NULL;
		_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
		UINT iMtrlCount = 0;

		// Mesh Load

		wstring strMeshKey, strMeshPath, strMeshSize;
		strMeshSize = LoadWString(pFile);
		strMeshKey = LoadWString(pFile);
		strMeshPath = LoadWString(pFile);
		m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);
		m_vecMesh.push_back(m_pMesh); // �޽� ���� �о������ ��.

		//UINT iMtrlCount = 0;
		fread(&iMtrlCount, sizeof(UINT), 1, pFile);
		m_vecMtrl.resize(iMtrlCount);

		for (UINT j = 0; j < iMtrlCount; ++j)
		{
			UINT idx = -1;
			fread(&idx, 4, 1, pFile);
			if (idx == -1)
				break;

			wstring strKey = LoadWString(pFile);
			wstring strPath = LoadWString(pFile);

			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey, strPath, true);
			m_vecMtrl[j] = pMtrl;
		}

		for (int i = 0; i < std::stoi(strMeshSize)-1; ++i)
		{
			wstring strMeshKey, strMeshPath;
			strMeshKey = LoadWString(pFile, i, meshname);
			strMeshPath = LoadWString(pFile, i, meshname);
			m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);
			m_vecMesh.push_back(m_pMesh); // �޽� ���� �о������ ��.
		}
		// material ���� �б�

		fclose(pFile);
	}
}

//void CMeshData::VecMeshLoad(const wstring& _strFilePath, bool _bFBX)
//{
//	FILE* pFile = NULL;
//	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
//	UINT iMtrlCount = 0;
//	wstring strFullPath = CPathMgr::GetResPath();
//	
//	 Mesh Load
//	for (int i=0; i < 25; ++i)
//	{
//		wstring strMeshKey, strMeshPath;
//		strMeshKey = LoadWString(pFile, i);
//		strMeshPath = LoadWString(pFile, i);
//		m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);
//		m_vecMesh.push_back(m_pMesh); // �޽� ���� �о������ ��.
//		if (i == 1)
//		{
//			wstring strMeshKey, strMeshPath;
//			strMeshKey = LoadWString(pFile);
//			strMeshPath = LoadWString(pFile);
//			m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);
//			m_vecMesh.push_back(m_pMesh); // �޽� ���� �о������ ��.
//
//			UINT iMtrlCount = 0;
//			fread(&iMtrlCount, sizeof(UINT), 1, pFile);
//			m_vecMtrl.resize(iMtrlCount);
//
//			for (UINT i = 0; i < iMtrlCount; ++i)
//			{
//				UINT idx = -1;
//				fread(&idx, 4, 1, pFile);
//				if (idx == -1)
//					break;
//
//				wstring strKey = LoadWString(pFile);
//				wstring strPath = LoadWString(pFile);
//
//				Ptr<CMaterial> pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey, strPath,true);
//				m_vecMtrl[i] = pMtrl;
//			}
//		}
//		else
//		{
//			wstring strMeshKey, strMeshPath;
//			strMeshKey = LoadWString(pFile, i);
//			strMeshPath = LoadWString(pFile, i);
//			m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);
//			m_vecMesh.push_back(m_pMesh); // �޽� ���� �о������ ��.
//		}
//	}
//	 material ���� �б�
//
//
//
//	fclose(pFile);
//}

void CMeshData::Save(const wstring& _strFilePath)
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += _strFilePath;
	SetPath(_strFilePath);

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	// Mesh �� ���Ϸ� ����
	for (int i = 0; i < m_vecMesh.size(); ++i)
	{

		m_vecMesh[i]->Save(m_vecMesh[i]->GetPath());

		// Mesh Key ����	
		// Mesh Data ����
		SaveWString(pFile, std::to_wstring(m_vecMesh.size()));
		SaveWString(pFile, m_vecMesh[i]->GetName().c_str());
		SaveWString(pFile, m_vecMesh[i]->GetPath().c_str());

		// material ���� ����
		UINT iMtrlCount = m_vecMesh[i]->GetSubsetCount();
		iMtrlCount = (UINT)m_vecMtrl.size();
		fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

		UINT j = 0;
		for (; j < iMtrlCount; ++j)
		{
			if (nullptr == m_vecMtrl[j])
				continue;

			// Material �� ���Ϸ� ����
			m_vecMtrl[j]->Save(m_vecMtrl[j]->GetPath());

			// Material �ε���, Key, Path ����
			fwrite(&j, sizeof(UINT), 1, pFile);
			SaveWString(pFile, m_vecMtrl[j]->GetName().c_str());
			SaveWString(pFile, m_vecMtrl[j]->GetPath().c_str());
		}

		j = -1; // ���� ��
		fwrite(&j, sizeof(UINT), 1, pFile);
	}

	fclose(pFile);
}

CGameObject* CMeshData::Instantiate()
{
	// Mesh
	// Material

	CGameObject* pNewObj = new CGameObject;
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	//pNewObj->MeshRender()->SetMesh(m_pMesh);
	for (UINT i = 0; i < m_vecMesh.size(); ++i)
	{
		pNewObj->MeshRender()->SetMesh(m_vecMesh[i], i);

		//if (false == m_vecMesh[i]->IsAnimMesh())
		//	return pNewObj;
		if (false == m_vecMesh[i]->IsAnimMesh())
			continue;

		CAnimator3D* pAnimator = new CAnimator3D;
		pNewObj->AddComponent(pAnimator);

		pAnimator->SetBones(m_vecMesh[i]->GetBones());
		pAnimator->SetAnimClip(m_vecMesh[i]->GetAnimClip());

	}


	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

	//if (false == m_pMesh->IsAnimMesh())
	//	return pNewObj;

	//CAnimator3D* pAnimator = new CAnimator3D;
	//pNewObj->AddComponent(pAnimator);

	//pAnimator->SetBones(m_pMesh->GetBones());
	//pAnimator->SetAnimClip(m_pMesh->GetAnimClip());
	

	return pNewObj;
}

