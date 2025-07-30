#pragma once
#include "Base.h"

union COLLIDER_ID
{
	struct
	{
		_uint iLeft_ID;
		_uint iRight_ID;
	};
	_ulonglong ID;
};

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
public:
	typedef struct tagCollPairType
	{
		COLL_TYPE eDestUseType;
		COLL_TYPE eSourceUseType;

	}COLL_PAIRTYPE;

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize();
	void	Tick();
	void	Tick_Collision();

public: 
	HRESULT				Add_CollisionGroup(const wstring& strDestLayerTag
											, const wstring& strSourceLayerTag
											, COLL_TYPE eDestUseType
											, COLL_TYPE eSourceUseType);

	HRESULT				Clear_CollisionGroup();

private:
	void Loop_ForColliders(CGameObject* pObjA, CGameObject* pObjB, wstring szDestGroupKey, _int iCollGroupIdx);

	void Check_Collision(CCollider* pColliderA, CCollider* pColliderB
					   , CGameObject* pReturnObjA, CGameObject* pReturnObjB
					   , map<_ulonglong, _bool>::iterator& iterCollInfo);


private:
	class CGameObject*  Get_ReturnObject(COLL_TYPE eCollType, CGameObject* pOwnerObj);
	void				Set_CollInfo(map<_ulonglong, _bool>::iterator& iter, class CCollider* pCollider1, class CCollider* pCollider2);

private:
	map<const wstring, vector<wstring>>				m_mapCollGroup;		// 누가 누구와 충돌체크 할건지
	map<const wstring, vector<COLL_PAIRTYPE>>		m_mapPairType;		// 나의 어느부위와, 상대의 어느부위와 충돌할건지

	map<_ulonglong, _bool>							m_mapCollInfo;		// 충돌체 간 이전 프레임 충돌 정보



private:
	class CGameInstance* m_pGameInstance = { nullptr };

public: 
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END

