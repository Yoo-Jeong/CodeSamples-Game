#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CEasing_Utillity final
{
public:
	CEasing_Utillity();
	~CEasing_Utillity();

public:
	static _vector LerpToType(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime, EASING_TYPE eType);

	static _vector Linear(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  Linear(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector QuadIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector QuadIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuadIn_Float(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector QuadOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuadOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector QuadInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuadInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector CubicIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  CubicIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector CubicOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  CubicOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector CubicInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  CubicInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector QuartIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuartIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector QuartOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuartOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector QuartInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuartInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector QuintIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuintIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector QuintOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuintOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector QuintInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  QuintInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector SineIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  SineIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector SineOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  SineOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector SineInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  SineInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector ExpoIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  ExpoIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector ExpoOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  ExpoOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector ExpoInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  ExpoInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector CircIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  CircIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector CircOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  CircOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector CircInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  CircInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector ElasticIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  ElasticIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector ElasticOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  ElasticOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector ElasticInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  ElasticInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector BounceIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  BounceIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector BounceOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _float  BounceOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

	static _vector OutBack(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	static _vector InOutBack(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
};

END