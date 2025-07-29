#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Player final : public CCamera
{
public:
	typedef struct tagPlayerCameraDesc : public tagCameraDesc
	{
		_float		fMouseSensor = 0.f;
		_float		fRotSpeed;

		_float		fOffsetY;
		_float		fOffsetZ;

		_uint		iPlayerLevelIndex;
		wstring		strPlayerLayerTag;
	} PLAYER_CAMERA_DESC;

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;

public:
	HRESULT			Find_Player(_uint iLevelIndex, wstring& strLayerTag);
	void			Mouse_Fix();

public:
	void			Follow_Target(_float fTimeDelta);
	void			Mouse_Rotate(_float fTimeDelta);
	void			Mouse_Zoom(_float fTimeDelta);

private:
	CTransform*		m_pPlayerTransform = { nullptr };

	_float			m_fMouseSensor = { 0.f };

	_float			m_fOffsetY = { 0.f };
	_float			m_fOffsetZ = { 0.f };

	_long			m_lMouseMoveX = { 0 };
	_long			m_lMouseMoveY = { 0 };
	_long           m_lMouseMoveZ = { 0 };
	_float			m_fRotSpeed = { 0.f };

/* 부드러운 카메라 */
private:
	_float4			m_vPreTargetPos = {};
	_float4			m_vTargetFollowLerpPosition = {};
	_float			m_fAccel = { 10.f };
	_float			m_fSpeed = { 0.f };

public:
	static CCamera_Player*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END