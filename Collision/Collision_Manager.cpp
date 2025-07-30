#include "Collision_Manager.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Collider.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize()
{


	return S_OK;
}

void CCollision_Manager::Tick()
{

	Tick_Collision();

}

HRESULT CCollision_Manager::Add_CollisionGroup(const wstring& strDestLayerTag
	, const wstring& strSourceLayerTag
	, COLL_TYPE eDestUseType
	, COLL_TYPE eSourceUseType)
{

	m_mapCollGroup[strDestLayerTag].push_back(strSourceLayerTag);


	COLL_PAIRTYPE ePairType = {};
	ePairType.eDestUseType = eDestUseType;
	ePairType.eSourceUseType = eSourceUseType;

	m_mapPairType[strDestLayerTag].push_back(ePairType);


	return S_OK;
}

HRESULT CCollision_Manager::Clear_CollisionGroup()
{
	m_mapCollGroup.clear();
	m_mapPairType.clear();
	m_mapCollInfo.clear();

	return S_OK;
}


void CCollision_Manager::Tick_Collision()
{
	/* 현재 레벨 가져오기 */
	_uint iCurLevel = m_pGameInstance->Get_CurLevelIndex();

	for (auto& iterCollGroup : m_mapCollGroup)
	{
		list<CGameObject*>* pLayerA = m_pGameInstance->Get_ObjList(iCurLevel, iterCollGroup.first);

		_int iSize = iterCollGroup.second.size();
		for (_int i = 0; i < iSize; i++)
		{
			list<CGameObject*>* pLayerB = m_pGameInstance->Get_ObjList(iCurLevel, iterCollGroup.second[i]);

			if (nullptr == pLayerA || nullptr == pLayerB || pLayerA->empty() || pLayerB->empty()) return;

			map<_ulonglong, _bool>::iterator		iterCollInfo;

			/* Loop */
			for (auto& pObjA : *pLayerA)
			{
				for (auto& pObjB : *pLayerB)
				{
					if (nullptr == pObjA || nullptr == pObjB || pObjA == pObjB) continue;

					Loop_ForColliders(pObjA, pObjB, iterCollGroup.first, i);
				}
			}
		}
	}

}

void CCollision_Manager::Loop_ForColliders(CGameObject* pObjA, CGameObject* pObjB, wstring szDestGroupKey, _int iCollGroupIdx)
{
	CCollider* pColliderA = nullptr;
	CCollider* pColliderB = nullptr;
	CGameObject* pReturnObjA = Get_ReturnObject(m_mapPairType[szDestGroupKey][iCollGroupIdx].eDestUseType, pObjA);
	CGameObject* pReturnObjB = Get_ReturnObject(m_mapPairType[szDestGroupKey][iCollGroupIdx].eSourceUseType, pObjB);

	_int iCollSizeA = pReturnObjA->Get_ColliderComsCount();
	_int iCollSizeB = pReturnObjB->Get_ColliderComsCount();

	for (_int j = 0; j < iCollSizeA; j++)
	{
		pColliderA = dynamic_cast<CCollider*>(pReturnObjA->Find_Component(TEXT("Com_Collider"), j));

		for (_int k = 0; k < iCollSizeB; k++)
		{
			pColliderB = dynamic_cast<CCollider*>(pReturnObjB->Find_Component(TEXT("Com_Collider"), k));

			if (pColliderA == nullptr || pColliderB == nullptr || pColliderA == pColliderB
				/*|| !pColliderA->Is_Active() || !pColliderB->Is_Active()*/ ) continue;

			map<_ulonglong, _bool>::iterator iterCollInfo;
			Set_CollInfo(iterCollInfo, pColliderA, pColliderB);

			Check_Collision(pColliderA, pColliderB, pReturnObjA, pReturnObjB, iterCollInfo);
		}
	}

}

void CCollision_Manager::Check_Collision(CCollider* pColliderA, CCollider* pColliderB
										, CGameObject* pReturnObjA, CGameObject* pReturnObjB
										, map<_ulonglong, _bool>::iterator& iterCollInfo)
{
	if (pColliderA->Collision(pColliderB))
	{
		if (iterCollInfo->second) // 현재 충돌 중이다.
		{
			if (!pReturnObjA->Is_Active() || !pReturnObjB->Is_Active()
				|| !pColliderA->Is_Active() || !pColliderB->Is_Active()) // 둘 중 하나 삭제 예정.
			{
				pColliderA->OnCollision_Exit(pReturnObjB, pColliderB, pColliderA);
				pColliderB->OnCollision_Exit(pReturnObjA, pColliderA, pColliderB);

				iterCollInfo->second = FALSE;
			}
			else // 이전에도 충돌하고 있었다.(삭제 예정X)
			{
				pColliderA->OnCollision_Stay(pReturnObjB, pColliderB, pColliderA);
				pColliderB->OnCollision_Stay(pReturnObjA, pColliderA, pColliderB);
			}
		}
		else // 이전에는 충돌하지 않았다.
		{
			if (pReturnObjA->Is_Active() && pReturnObjB->Is_Active()
				&& pColliderA->Is_Active() && pColliderB->Is_Active()) // 둘 다 삭제예정이 아니면 충돌 체크
			{

				pColliderA->OnCollision_Enter(pReturnObjB, pColliderB, pColliderA);
				pColliderB->OnCollision_Enter(pReturnObjA, pColliderA, pColliderB);

				iterCollInfo->second = TRUE;
			}
		}
	}
	else
	{
		if (iterCollInfo->second) // 현재 충돌하고 있지 않다.
		{
			pColliderA->OnCollision_Exit(pReturnObjB, pColliderB, pColliderA);
			pColliderB->OnCollision_Exit(pReturnObjA, pColliderA, pColliderB);

			iterCollInfo->second = FALSE;
		}
	}
}

CGameObject* CCollision_Manager::Get_ReturnObject(COLL_TYPE eCollType, CGameObject* pOwnerObj)
{
	CGameObject* pReturnObj = { nullptr };

	switch (eCollType)
	{
	case COLL_TYPE::TYPE_BODY:
		pReturnObj = pOwnerObj;
		break;

	case COLL_TYPE::TYPE_WEAPON:
		pReturnObj = pOwnerObj->Find_PartObject(TEXT("Part_Weapon"));
		break;

	case COLL_TYPE::TYPE_BULLET:
		pReturnObj = pOwnerObj->Find_PartObject(TEXT("Part_Bullet"));
		break;

	case COLL_TYPE::TYPE_DETECT:
		pReturnObj = pOwnerObj->Find_PartObject(TEXT("Part_Detect"));
		break;

	default:
		pReturnObj = nullptr;
		break;
	}

	return pReturnObj;
}

void CCollision_Manager::Set_CollInfo(map<_ulonglong, _bool>::iterator& iter, CCollider* pColliderA, CCollider* pColliderB)
{
	// 두 충돌체 조합 아이디 생성
	COLLIDER_ID ID;
	ID.iLeft_ID = pColliderA->Get_ID();
	ID.iRight_ID = pColliderB->Get_ID();

	// 이전에 충돌 검사 여부 조사
	iter = m_mapCollInfo.find(ID.ID);

	// 충돌 정보가 미등록 상태인 경우 등록(충돌하지 않았다로)
	if (m_mapCollInfo.end() == iter)
	{
		m_mapCollInfo.insert({ ID.ID, FALSE });
		iter = m_mapCollInfo.find(ID.ID);
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollision_Manager::Free()
{

	Safe_Release(m_pGameInstance);
}
