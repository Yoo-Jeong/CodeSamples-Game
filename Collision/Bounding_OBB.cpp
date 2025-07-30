#include "..\Public\Bounding_OBB.h"
#include "DebugDraw.h"
#include "Collider.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{

}

HRESULT CBounding_OBB::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_OBB_DESC* pDesc = (BOUNDING_OBB_DESC*)pBoundingDesc;

	_float4		vQuaternion = {};

	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginalOBB = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
	m_pOBB = new BoundingOrientedBox(*m_pOriginalOBB);


	return S_OK;
}
void CBounding_OBB::Update(_fmatrix TransformMatrix)
{
	m_pOriginalOBB->Transform(*m_pOBB, TransformMatrix);
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _vector vColor)
{
	if (nullptr == pBatch ||
		nullptr == m_pOBB)
		return E_FAIL;

	/* m_pOBB가 월드스페이스 상의 정보다. */
	DX::Draw(pBatch, *m_pOBB, vColor);

	return S_OK;
}
#endif

_bool CBounding_OBB::Collision(CCollider* pTargetCollider, _bool* pisCollision)
{
	CBounding* pTargetBounding = pTargetCollider->Get_Bounding();

	*pisCollision = false;

	switch (pTargetCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		*pisCollision = m_pOBB->Intersects(*((CBounding_AABB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		*pisCollision = m_pOBB->Intersects(*((CBounding_OBB*)pTargetBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_SPHERE:
		*pisCollision = m_pOBB->Intersects(*((CBounding_Sphere*)pTargetBounding)->Get_Bounding());
		break;
	}
	return *pisCollision;
}

_bool CBounding_OBB::Intersect_Ray(const RAY& In_Ray, float* Out_pDistance)
{
	_bool bIntersection = false;

	_vector vOrigin = XMLoadFloat4(&In_Ray.vOriginPos);
	vOrigin.m128_f32[3] = 1.f;
	_vector vDirection = XMLoadFloat3(&In_Ray.vDirection);

	_float fDistance = 0.f;

	if (m_pOBB->Intersects(vOrigin, vDirection, fDistance))
	{
		if (Out_pDistance)
		{
			*Out_pDistance = fDistance;
		}

		bIntersection = In_Ray.fLength >= fDistance;
	}

	else
	{
		if (Out_pDistance)
		{
			*Out_pDistance = -1.f;
		}
	}

	return bIntersection;
}

_vector CBounding_OBB::Get_CurrentPosition()
{
	_vector vResult = XMVectorSetW(XMLoadFloat3(&m_pOBB->Center), 1.f);
	return vResult;
}


CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalOBB);
	Safe_Delete(m_pOBB);
}
