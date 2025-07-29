#include "stdafx.h"
#include "..\Public\Camera_Player.h"

#include "GameInstance.h"
#include "CutScene.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	PLAYER_CAMERA_DESC* pDesc = (PLAYER_CAMERA_DESC*)pArg;
	m_fMouseSensor = pDesc->fMouseSensor;
	m_fRotSpeed = pDesc->fRotSpeed;
	m_fOffsetY = pDesc->fOffsetY;
	m_fOffsetZ = pDesc->fOffsetZ;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Find_Player(pDesc->iPlayerLevelIndex, pDesc->strPlayerLayerTag)))
		return E_FAIL;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_vTargetFollowLerpPosition, vPlayerPos);
	XMStoreFloat4(&m_vPreTargetPos, vPlayerPos);


	m_pTransformCom->Rotation_Add(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fSpawnRot));


	return S_OK;
}

void CCamera_Player::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_Player::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Get_IsPlay())
	{
		_vector vPos, vLook;

		if (m_tCameraShake.m_bShaking)
		{
			m_tCameraShake.m_fAccTime += fTimeDelta;

			if (m_tCameraShake.m_fAccTime < m_tCameraShake.m_fDuration)
			{			
				_float fX = m_tCameraShake.m_fAmplitude * cosf(m_tCameraShake.m_fAccTime * m_tCameraShake.m_fSpeed + (((_float)rand() / (_float)RAND_MAX) * XM_PI));
				_float fY = m_tCameraShake.m_fAmplitude * -sinf(m_tCameraShake.m_fAccTime * m_tCameraShake.m_fSpeed + (((_float)rand() / (_float)RAND_MAX) * XM_PI));

				// 쉐이킹 적용
				_vector vShakePos = { fX, fY, 0.f, 0.f };


				vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_vector vNewPos = vPos + vLook + vShakePos;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
			}
			else
			{
				m_tCameraShake.m_bShaking = FALSE;
			}
		}
		else
		{
	
			vPos = XMLoadFloat4(&m_pGameInstance->Get_CurCutScene()->Get_CurEye());
			vPos.m128_f32[3] = 1.f;

			vLook = XMLoadFloat4(&m_pGameInstance->Get_CurCutScene()->Get_CurAt());	
			vLook.m128_f32[3] = 1.f;

			_float3 vRot = m_pGameInstance->Get_CurCutScene()->Get_CurRot();

			//m_pTransformCom->Rotation_Quaternion(_float3(vRot.x, vRot.y, 0.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pTransformCom->Look_At(vLook);

		}
	}
	//else if (m_pGameInstance->Key_Hold(KEY::LEFT))
	//{
	//	m_pTransformCom->Rotation_Quaternion_Add(_float3(0.f, -1.f, 0.f));
	//}
	//else if (m_pGameInstance->Key_Hold(KEY::RIGHT))
	//{
	//	m_pTransformCom->Rotation_Quaternion_Add(_float3(0.f, 1.f, 0.f));
	//}
	else
	{
		Mouse_Rotate(fTimeDelta);
		Mouse_Zoom(fTimeDelta);
		Follow_Target(fTimeDelta);
	}

	__super::Tick(fTimeDelta);
}

void CCamera_Player::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	//_float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	//cout << "Camera Pos  : " << vPos.x << " / " << vPos.y << " / " << vPos.z << " / " << vPos.w << endl;

	//_float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	//cout << "Camera Look : " << vLook.x << " / " << vLook.y << " / " << vLook.z << " / " << vLook.w << endl;
	//cout << m_pTransformCom->GetHeight_OnNavMesh(m_pNavigationCom) << endl;

	//_float3 vRot = m_pTransformCom->Get_Rotated_Quaternion();
	//cout << "Camera Rot : " << vRot.x << " / " << vRot.y << " / " << vRot.z << endl;
#endif	
}


HRESULT CCamera_Player::Find_Player(_uint iLevelIndex, wstring& strLayerTag)
{
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(iLevelIndex, strLayerTag, TEXT("Com_Transform")));

	return S_OK;
}

void CCamera_Player::Mouse_Fix()
{
	POINT	pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CCamera_Player::Follow_Target(_float fTimeDelta)
{
	/* 플레이어 따라가는 카메라 */
	//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_fOffsetZ;
	//_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + vLook + XMVectorSet(0.f, m_fOffsetY, 0.f, 0.f);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	/* 부드러운 카메라 */
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPrePlayerPos = XMLoadFloat4(&m_vPreTargetPos);
	XMStoreFloat4(&m_vPreTargetPos, vPlayerPos);

	//_float fDistance = XMVector3Length(vPlayerPos - vPrePlayerPos).m128_f32[0];

	//if (fTimeDelta < fDistance)
	//{
	//	_vector  vPlayerFollowLerpPostiion = XMLoadFloat4(&m_vTargetFollowLerpPosition);

	//	m_fSpeed += m_fAccel * fTimeDelta;

	//	if (5.f + fDistance < m_fSpeed)
	//		m_fSpeed = 5.f + fDistance;

	//	vPlayerFollowLerpPostiion = XMVectorLerp(vPlayerFollowLerpPostiion, vPrePlayerPos, m_fSpeed * fTimeDelta);
	//	XMStoreFloat4(&m_vTargetFollowLerpPosition, vPlayerFollowLerpPostiion);
	//}
	//else
	{
		m_fSpeed = 5.f;

		_vector vPlayerFollowLerpPos = XMLoadFloat4(&m_vTargetFollowLerpPosition);
		vPlayerFollowLerpPos.m128_f32[3] = 1.f;

		vPlayerFollowLerpPos = XMVectorLerp(vPlayerFollowLerpPos, vPrePlayerPos, m_fSpeed * fTimeDelta);
		XMStoreFloat4(&m_vTargetFollowLerpPosition, vPlayerFollowLerpPos);
	}

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_fOffsetZ;
	_vector vPos = XMLoadFloat4(&m_vTargetFollowLerpPosition) + vLook /*vZoomDir * m_fZoom*/ + XMVectorSet(0.f, m_fOffsetY, 0.f, 0.f) /* + XMLoadFloat3(&m_vShaking) */;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

}

void CCamera_Player::Mouse_Rotate(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Hold(KEY::RBUTTON))
	{
		Mouse_Fix();
		_long	lMouseMove = 0;

		if (lMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_lMouseMoveX += lMouseMove;
		}

		if (lMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_lMouseMoveY += lMouseMove;
		}
	}

	if (fabs(m_lMouseMoveX) > DBL_EPSILON) // 마우스 이동량이 있으면 회전
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * m_lMouseMoveX * fTimeDelta);
	m_lMouseMoveX = static_cast<_long>(m_lMouseMoveX * m_fRotSpeed);	// 정지속도(회전속도) 곱해주기

	
	_matrix RotMat = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * m_lMouseMoveY * fTimeDelta);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3TransformNormal(vLook, RotMat);

	_vector vPlayerUp = m_pPlayerTransform->Get_State(CTransform::STATE_UP);

	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vPlayerUp));


	if (0.3f > fDot && -0.99f < fDot) // 회전 제한(3인칭)
	{
		if (fabs(m_lMouseMoveY) > DBL_EPSILON)
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * m_lMouseMoveY * fTimeDelta);
		m_lMouseMoveY = static_cast<_long>(m_lMouseMoveY * m_fRotSpeed);
	}
	else
	{
		m_lMouseMoveY = 0;
	}

}

void CCamera_Player::Mouse_Zoom(_float fTimeDelta)
{
	_float fZoomSpeed = 30.f;

	m_lMouseMoveZ = m_pGameInstance->Get_DIMouseMove(DIMS_Z);

	_float fAccOffset = m_fOffsetZ + (m_lMouseMoveZ * 0.05f);

	if (5 > fAccOffset && -30.f < fAccOffset)
	{
		if (0 < m_pGameInstance->Get_DIMouseMove(DIMS_Z))
		{
			m_fOffsetZ += fZoomSpeed * fTimeDelta;
			//m_fOffsetY += 0.4f;
		}
		else if (0 > m_pGameInstance->Get_DIMouseMove(DIMS_Z))
		{
			m_fOffsetZ -= fZoomSpeed * fTimeDelta;

			//if(1.1f < m_fOffsetY)
			//	m_fOffsetY -= 0.4f;
		}			
	}

	if (m_pGameInstance->Key_Hold(KEY::CTRL) && m_pGameInstance->Key_Hold(KEY::UP))
	{
		m_fOffsetY += 1.f * fTimeDelta;
	}
	else if (m_pGameInstance->Key_Hold(KEY::CTRL) && m_pGameInstance->Key_Hold(KEY::DOWN))
	{
		m_fOffsetY -= 1.f * fTimeDelta;
	}
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

	/* 원형객체를 초기화한다. */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();

}
