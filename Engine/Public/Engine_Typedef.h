#pragma once

namespace Engine
{	
	typedef unsigned char			_ubyte;
	typedef signed char				_byte;
	typedef char					_char;
	typedef wchar_t					_tchar;
	typedef wstring					_wstring;

	typedef bool					_bool;

	typedef unsigned short			_ushort;
	typedef signed short			_short;

	typedef unsigned int			_uint;
	typedef signed int				_int;

	typedef unsigned long			_ulong;
	typedef unsigned long long		_ull;
	typedef signed long				_long;

	typedef float					_float;
	typedef double					_double;

	typedef XMFLOAT2				_float2;
	typedef XMFLOAT3				_float3;
	typedef XMFLOAT4				_float4;
	typedef XMVECTOR				_vector; 
	typedef FXMVECTOR				_fvector;
	typedef GXMVECTOR				_gvector;
	typedef HXMVECTOR				_hvector;
	typedef CXMVECTOR				_cvector;


	typedef XMFLOAT4X4				_float4x4;
	typedef XMMATRIX				_matrix;
	typedef FXMMATRIX				_fmatrix;
	typedef CXMMATRIX				_cmatrix;

	typedef XMUINT3					_uint3;
	typedef XMUINT2					_uint2;

	enum class DELAYED_TYPE
	{
		CREATE_OBJECT,//do not use
		DELETE_OBJECT,
		CHANGE_LEVEL_DELAY,
		DELAYED_END,
	};

	enum MOUSEKEYSTATE
	{
		DIMK_LBUTTON,
		DIMK_RBUTTON,
		DIMK_WHEEL,
		DIMK_END,
	};

	enum MOUSEMOVESTATE
	{
		DIMM_X,
		DIMM_Y,
		DIMM_WHEEL,
		DIMM_END,
	};

	//enum class COLLIDER_TYPE
	//{
	//	SPHERE,
	//	AABB,
	//	OBB
	//};

}

