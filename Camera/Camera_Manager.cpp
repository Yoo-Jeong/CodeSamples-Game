#include "..\Public\Camera_Manager.h"
#include "Camera.h"

#include "GameInstance.h"
#include "MyMath.h"
#include "Transform.h"

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CCamera_Manager::Add_Prototype_Camera(_uint iLevelIndex, const wstring& strPrototypeTag, CCamera* pPrototype)
{
	if (nullptr == pPrototype ||
		iLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	/* 이름 설정. */
	pPrototype->Set_Name(strPrototypeTag);

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CCamera_Manager::Add_Clone_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, const wstring& strCameraTag, void* pArg)
{
	/* 원형을 찾고. */
	CCamera* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);

	if (nullptr == pPrototype)
		return E_FAIL;

	/* 원형을 복제하여 실제 게임내에 사용할 사본 객체를 생성해낸다.  */
	CCamera* pCamera = dynamic_cast<CCamera*>(pPrototype->Clone(pArg));
	if (nullptr == pCamera)
		return E_FAIL;

	pCamera->Set_LayerTag(strLayerTag);
	//pCamera->Set_Name(Get_CloneNameWithPin(pCamera->Get_Name()));
	pCamera->Set_CameraName(strCameraTag);
	m_pCameras.emplace(strCameraTag, pCamera);

	return S_OK;
}

void CCamera_Manager::Priority_Tick(_float fTimeDelta)
{
	_int a = 0;
	m_pCurCamera;

	if(nullptr != m_pCurCamera)
		m_pCurCamera->Priority_Tick(fTimeDelta);
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurCamera)
	{
		if (m_bIsPlay)  // 컷신 재생이면
		{
			if (m_pCurCutScene) // 현재 컷신이 있으면
			{
				m_pCurCutScene->Update_CutScene(fTimeDelta);        // 컷신 업데이트
			}
		}
		m_pCurCamera->Tick(fTimeDelta);
	}
	
}

void CCamera_Manager::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurCamera)
	{
		if (m_bIsPlay)	// 컷신 재생이면 컷신 컷신 레이트 업데이트
		{
			if (m_pCurCutScene)
			{
				m_pCurCutScene->LateUpdate_CutScene(fTimeDelta);

				if (m_pCurCutScene->Get_End())  // 컷신 재생이 종료됐다면 false
				{
					m_bIsPlay = false;
				}
			}
		}
		m_pCurCamera->Late_Tick(fTimeDelta);
	}

}

void CCamera_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;


	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);
	m_pPrototypes[iLevelIndex].clear();
}

void CCamera_Manager::Clear_Cameras()
{
	for (auto& Pair : m_pCameras)
		Safe_Release(Pair.second);
	m_pCameras.clear();

	m_pCurCamera = nullptr;
	Safe_Release(m_pCurCamera);
}


CCamera* CCamera_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}


CCamera* CCamera_Manager::Get_CurCamera()
{
	return dynamic_cast<CCamera*>(m_pCurCamera);
}

HRESULT CCamera_Manager::Set_CurCamera(const wstring& strCameraTag)
{
	CCamera* pCamera = m_pCameras.find(strCameraTag)->second;

	if (nullptr == pCamera)
		return E_FAIL;

	m_pCurCamera = pCamera;

	return S_OK;
}


const wstring CCamera_Manager::Get_CloneNameWithPin(const wstring& strName)
{
	_int CloneMaxNum = -1;
	wstring strPin = TEXT("");

	for (auto& iter : m_pCameras)
	{
		if (nullptr == iter.second) continue;

		/* 클론 고유 번호를 자른 이름과 비교한다. */
		if (strName == MyMath::Remove_LastNumChar(iter.second->Get_Name(), CLONE_PIN_MAX_DIGIT))
		{
			/* 이름이 같다면 고유번호만 갖고와서 대소비교를 통해 최댓값에 저장할지를 결정한다. */
			/* 언더바를 제외하기 위해 - 1을 한다. */
			_int iPinNum = stoi(MyMath::Get_LastNumChar(iter.second->Get_Name(), CLONE_PIN_MAX_DIGIT - 1));

			if (CloneMaxNum < iPinNum)
				CloneMaxNum = iPinNum;
		}
	}

	/* 최댓값이 -1이라는 것은 해당 이름과 같은 클론이 없으므로 고유번호를 000으로 세팅한다. */
	if (-1 == CloneMaxNum)
		return strName + L"_000";
	else if (0 == CloneMaxNum)		/* 0이라는 것은 클론이 딱 하나 존재한다는 것*/
		return strName + L"_001";
	else /* 아니라면 최댓값에 + 1을 하여 고유 번호로 세팅한다. */
	{
		_int iPinNum = CloneMaxNum + 1;

		if (0 == iPinNum / 10)
			strPin = L"_00" + to_wstring(iPinNum);
		else if (0 == iPinNum / 100)
			strPin = L"_0" + to_wstring(iPinNum);
		else
			strPin = L"_" + to_wstring(iPinNum);

		return strName + strPin;
	}
}



void CCamera_Manager::Initialize_CutScene()
{


}

void CCamera_Manager::Clear_CutScene()
{
	Safe_Delete_Array(m_pCurCutSceneTag);

	for (auto& Pair : m_mapCutScene)
		Safe_Release(Pair.second);
	m_mapCutScene.clear();
}

HRESULT CCamera_Manager::Add_CutScene(_tchar* pTag, CCutScene* _pCutScene)
{
	// 같은 키의 키프레임이 있으면 안됨! (중복 키 불가)
	CCutScene* pCutScene = Find_CutScene(pTag);

	if (nullptr != pCutScene)
		return E_FAIL;
	else
	{
		_pCutScene->Set_CutSceneTag(pTag);
		m_mapCutScene.insert({ pTag, _pCutScene });
	}

	return S_OK;
}

CCutScene* CCamera_Manager::Find_CutScene(const _tchar* pTag)
{
	auto	iter = find_if(m_mapCutScene.begin(), m_mapCutScene.end(), CTag_Finder(pTag));

	if (iter == m_mapCutScene.end())
		return nullptr;

	return iter->second;
}

void CCamera_Manager::Play_CutScene(const _tchar* pTag)
{
	m_pCurCutScene = Find_CutScene(pTag);

	if (m_pCurCutScene != nullptr)
	{
		m_bIsPlay = TRUE;
		m_pCurCutScene->Ready_CutScene();
	}
}

void CCamera_Manager::Play_CutScene_FromPlayer(const _tchar* pTag)
{
	m_pCurCutScene = Find_CutScene(pTag);

	if (m_pCurCutScene != nullptr)
	{
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CurLevelIndex(), TEXT("Layer_Player"), g_pTransformTag));

		CTransform* pTransform = m_pCurCamera->Get_TransformCom();
		_float4 vEyePos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);
		//_float4 vLookAt = pTransform->Get_State_Float4(CTransform::STATE_LOOK);
		_float4 vLookAt = pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION);
		_float3 vRot = pTransform->Get_Rotated_Quaternion();

		// 플레이어의 위치에서부터 컷씬이 시작된다.
		m_pCurCutScene->Set_KeyFrame_EyePos(0, vEyePos);
		m_pCurCutScene->Set_KeyFrame_LookAt(0, vLookAt);
		m_pCurCutScene->Set_KeyFrame_Rot(0, vRot);

		// 마지막 프레임은 플레이어의 위치로 돌아와서 종료된다.
		_int MaxFrmae = m_pCurCutScene->Get_MaxFrameIdx();
		m_pCurCutScene->Set_KeyFrame_EyePos(MaxFrmae, vEyePos);
		m_pCurCutScene->Set_KeyFrame_LookAt(MaxFrmae, vLookAt);
		m_pCurCutScene->Set_KeyFrame_Rot(MaxFrmae, vRot);

		m_bIsPlay = TRUE;
		m_pCurCutScene->Ready_CutScene();
	}
}

void CCamera_Manager::Set_FromPlayer(const _tchar* pTag)
{
	CCutScene* pCutScene = Find_CutScene(pTag);

	if (nullptr != pCutScene)
	{
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CurLevelIndex(), TEXT("Layer_Player"), g_pTransformTag));

		CTransform* pTransform = m_pCurCamera->Get_TransformCom();
		_float4 vEyePos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);
		//_float4 vLookAt = pTransform->Get_State_Float4(CTransform::STATE_LOOK);
		_float4 vLookAt = pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION);
		_float3 vRot = pTransform->Get_Rotated_Quaternion();

		// 플레이어의 위치에서부터 컷씬이 시작된다.
		pCutScene->Set_KeyFrame_EyePos(0, vEyePos);
		pCutScene->Set_KeyFrame_LookAt(0, vLookAt);
		m_pCurCutScene->Set_KeyFrame_Rot(0, vRot);

		// 마지막 프레임은 플레이어의 위치로 돌아와서 종료된다.
		_int MaxFrmae = m_pCurCutScene->Get_MaxFrameIdx();
		pCutScene->Set_KeyFrame_EyePos(MaxFrmae, vEyePos);
		pCutScene->Set_KeyFrame_LookAt(MaxFrmae, vLookAt);
		m_pCurCutScene->Set_KeyFrame_Rot(MaxFrmae, vRot);
	}
}

_bool CCamera_Manager::Is_ExistCutScene()
{
	if (!m_mapCutScene.empty())
		return true;
	else
		return false;
}

void CCamera_Manager::Set_CurCutScene(_tchar* pTag)
{
	m_pCurCutScene = Find_CutScene(pTag);
}

_int CCamera_Manager::Get_CurFrame()
{
	return -1;
}


CCamera_Manager* CCamera_Manager::Create(_uint iNumLevels)
{
	CCamera_Manager* pInstance = new CCamera_Manager;

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CCamera_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Manager::Free()
{
	Safe_Delete_Array(m_pCurCutSceneTag);

	for (auto& Pair : m_mapCutScene)
		Safe_Release(Pair.second);
	m_mapCutScene.clear();


	for (auto& Pair : m_pCameras)
		Safe_Release(Pair.second);
	m_pCameras.clear();

	Safe_Release(m_pCurCamera);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
		{
			Safe_Release(Pair.second);
		}
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);

	Safe_Release(m_pGameInstance);
}