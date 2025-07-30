#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

/* 충돌체. */
/* 충돌체를 정의하는 기능 + 충돌체을 그리는 기능 + 충돌을 비교하는 기능. */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE		{ TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE			Get_ColliderType() const { return m_eType; }

	void			Set_UseType(COLL_TYPE eUseType) { m_eUseType = eUseType; }
	COLL_TYPE		Get_UseType() const { return m_eUseType; }

	const _uint& Get_ID() const { return m_iID; }

	class CBounding* Get_Bounding() { return m_pBounding; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);

public:
	void	Update(_fmatrix TransformMatrix);
	_bool	Collision(CCollider * pTargetCollider);
	_bool	Intersect_Ray(const RAY& In_Ray, float* Out_pDistance = nullptr) const;

public:
	_vector				Get_CurrentPosition();


public:
	void	OnCollision_Enter(CGameObject* pOther, CCollider* pOtherCol, CCollider* pThisCol);
	void	OnCollision_Stay(CGameObject* pOther, CCollider* pOtherCol, CCollider* pThisCol);
	void	OnCollision_Exit(CGameObject* pOther, CCollider* pOtherCol, CCollider* pThisCol);


#ifdef _DEBUG
public:
	HRESULT Render();
	void	Set_RenderColor(_fvector In_vColor);
#endif

private:
	TYPE					m_eType		= { TYPE_END };
	COLL_TYPE				m_eUseType	= { COLL_TYPE_END };

	class CBounding*		m_pBounding = { nullptr };
	_bool					m_isCollision = { false };
	_float4				    m_vCurrentPosition;

	_uint					m_iCollisonCount = { 0 };	// 현재 충돌중인 콜라이더 갯수
	_uint					m_iID = { 0 };				// 콜라이더 고유 아이디
	static _uint			g_iNextID;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };

	_float4									m_vColor = { 0.f, 0.f, 0.f, 1.f };
#endif


public:
	static CCollider* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END