#pragma once

/* 클라이언트 개발자가 만들고자하는 카메라들의 공통적인 부모가 되기위한 클래스다. */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tCameraShake
	{
		_bool		m_bShaking;		// 카메라 쉐이킹 여부
		_float		m_fAccTime;		// 누적시간

		_float		m_fAmplitude;	// 진폭
		_float		m_fDuration;	// 흔드는 시간
		_float		m_fSpeed;		// 흔드는 속도

		_float4		m_vOriginPos;	// 원래 있던 위치

	}CAMERASHAKE;

	typedef struct tagCameraDesc : public tagGameObjectDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;

public:
	/* For.CameraName */
	void				Set_CameraName(const wstring& strCameraName) { m_strCameraName = strCameraName; }
	const wstring&		Get_CameraName() const { return m_strCameraName; }


/* For.CameraShake */
public:
	void			Shake_Camera();
	void			Set_ShakeForce(_float _fAmplitude, _float _fDuration, _float _fSpeed);
	void			Reset_ShakeForce() { ZeroMemory(&m_tCameraShake, sizeof(CAMERASHAKE)); }

protected:
	CAMERASHAKE		m_tCameraShake;

protected:
	wstring			m_strCameraName = { };

	_float			m_fFovy = { 0.f };
	_float			m_fAspect = { 0.0f };
	_float			m_fNear = { 0.0f };
	_float			m_fFar = { 0.0f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END