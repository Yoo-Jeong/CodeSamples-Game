#include "..\Public\Camera.h"
#include "Transform.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

	/* 항등상태의 트랜스폼을 내가 셋팅하고 싶은 카메라의 상태로 동기화한다. */
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->Look_At(XMLoadFloat4(&pCameraDesc->vAt));

	m_fFovy		= pCameraDesc->fFovy;
	m_fAspect	= pCameraDesc->fAspect;
	m_fNear		= pCameraDesc->fNear;
	m_fFar		= pCameraDesc->fFar;

	return S_OK;
}

void CCamera::Priority_Tick(_float fTimeDelta)
{
}

void CCamera::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 로컬 -> 월드 -> 뷰 -> 투영
	// 카메라의 월드의 역행렬이 곧 뷰스페이스로 가는 행렬이다
	m_pGameInstance->Set_CamWorld(m_pTransformCom->Get_WorldFloat4x4());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

void CCamera::Shake_Camera()
{
	m_tCameraShake.m_fAccTime = 0.f;		// 누적시간 초기화
	m_tCameraShake.m_vOriginPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	m_tCameraShake.m_bShaking = TRUE;
}

void CCamera::Set_ShakeForce(_float _fAmplitude, _float _fDuration, _float _fSpeed)
{
	m_tCameraShake.m_fAmplitude = _fAmplitude;
	m_tCameraShake.m_fDuration = _fDuration;
	m_tCameraShake.m_fSpeed = _fSpeed;
}

void CCamera::Free()
{
	__super::Free();

}