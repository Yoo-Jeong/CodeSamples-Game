#include "..\Public\Collider.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "GameInstance.h"

#include "GameObject.h"

_uint CCollider::g_iNextID = 0;

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_eUseType(rhs.m_eUseType)
	, m_isCollision(rhs.m_isCollision)
	, m_iID(rhs.m_iID)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	CBounding::BOUNDING_DESC* pBoundingDesc = (CBounding::BOUNDING_DESC*)pArg;

	m_eUseType = pBoundingDesc->eUseType;
	m_iID = g_iNextID++;

	switch (m_eType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	m_pBounding->Update(TransformMatrix);
	Get_CurrentPosition();
}

_bool CCollider::Collision(CCollider* pTargetCollider)
{
	return m_pBounding->Collision(pTargetCollider, &m_isCollision);
}

_vector CCollider::Get_CurrentPosition() 
{
	XMStoreFloat4(&m_vCurrentPosition, m_pBounding->Get_CurrentPosition());
	_vector vResult = XMLoadFloat4(&m_vCurrentPosition);
	
	return vResult;
}

_bool CCollider::Intersect_Ray(const RAY& In_Ray, float* Out_pDistance) const
{
	return m_pBounding->Intersect_Ray(In_Ray, Out_pDistance);
}

void CCollider::OnCollision_Enter(CGameObject* pOther, CCollider* pOtherCol, CCollider* pThisCol)
{
	if (m_bActive)
	{
		++m_iCollisonCount;
		m_pOwner->OnCollision_Enter(pOther, pOtherCol, pThisCol);
	}
}

void CCollider::OnCollision_Stay(CGameObject* pOther, CCollider* pOtherCol, CCollider* pThisCol)
{
	if (m_bActive)
	{
		m_pOwner->OnCollision_Stay(pOther, pOtherCol, pThisCol);
	}
}

void CCollider::OnCollision_Exit(CGameObject* pOther, CCollider* pOtherCol, CCollider* pThisCol)
{	
	//if (m_bActive)
	{
		--m_iCollisonCount;
		m_pOwner->OnCollision_Exit(pOther, pOtherCol, pThisCol);
	}
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	if (m_bActive)
	{
		m_pBatch->Begin();

		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		m_pContext->IASetInputLayout(m_pInputLayout);

		m_pEffect->Apply(m_pContext);

		{
			if (0 < m_iCollisonCount)
				m_vColor = { 1.f, 0.f, 0.f, 1.f };
			else
				m_vColor = { 0.f, 1.f, 0.f, 1.f };
		}
		m_pBounding->Render(m_pBatch, XMLoadFloat4(&m_vColor));

		//m_pBounding->Render(m_pBatch, m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));


		m_pBatch->End();
	}

	return S_OK;
}

void CCollider::Set_RenderColor(_fvector In_vColor)
{
	XMStoreFloat4(&m_vColor, In_vColor);
}


#endif 

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	/* 원형객체를 초기화한다. */
	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif

	Safe_Release(m_pBounding);

}
