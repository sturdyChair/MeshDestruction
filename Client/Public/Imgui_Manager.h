#pragma once

#include "Client_Defines.h"
#include "Effect_Base.h"
#include "ObjEnum.h"
#include "UnionEffectData.h"
#include "CSG_Tester.h"
#include "Woundable.h"
#include "Breakable.h"

BEGIN(Engine)
class CGameObject;
class CShader;
class CModel;
END

BEGIN(Client)

class CGeometryBrush;

enum class eImgui_State
{
	IMGUI_STATE_WAIT,
	IMGUI_STATE_RENDER,
	IMGUI_STATE_SHUTDOWN,
	IMGUI_STATE_END
};

enum class eMapLoadType
{
	MAP_LOAD,
	MAP_SAVE,
	MODELNAME_LOAD,
	JSON_LOAD,
	MAP_END
};

enum class eEffectLoadType
{
	EFFECT_LOAD,
	EFFECT_SAVE,
	EFFECT_END
};

enum class eToolType
{
	TOOL_MAP,
	TOOL_EFFECT,
	TOOL_SOUND,
	TOOL_ANIMATION,
	TOOL_FRACTURE,
	TOOL_WOUND,
	TOOL_BREAKABLE,
	TOOL_GEOBRUSH,
	TOOL_END
};

using GIZMODESC = struct tagGizmoDesc
{
	ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool bUseSnap = false;
	bool boundSizing = false;
	bool boundSizingSnap = false;
	float snap[3] = { 0.1f, 0.1f, 0.1f };
	float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
};

class CImgui_Manager
{

	DECLARE_SINGLETON(CImgui_Manager)

private:
	CImgui_Manager();

public:
	~CImgui_Manager();

public:
	void	Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext);
	HRESULT Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void    PreTick();
	void    Tick();
	void	LateTick();
	void	Tick_Engine(_float fTimeDelta);

	void	Imgui_Render();

	void	Imgui_MapTool();
	void	Imgui_EffectTool();
	void	Imgui_SoundTool();
	void	Imgui_AnimationTool();
	void	Imgui_LinkTool();
	void	Imgui_FractureTool();
	void	Imgui_BreakableObjectTool();

	void	Imgui_GeometryBrushTool();

	void    Imgui_Main();

	void	Imgui_GizmoRender();
	void	Imgui_EffectOptionRender();
	void	Imgui_MenuBar();
	void	Imgui_EffectMenuBar();

	void	Imgui_ObjCreate();
	void	Imgui_EffectCreate();
private:
	void    Imgui_MapDataControl();
	void    Imgui_EffectDataControl();

	void	Load_ModelNames(wstring strFilePath);
	void	Load_FModel();

	void	Pick_Object();

	//Imgui Control Function
public:
	void			Imgui_SetState(eImgui_State eState) { m_eState = eState; }
	eImgui_State	Imgui_GetState() { return m_eState; }

	void 			Imgui_SetSelectObject(shared_ptr< CGameObject> pObject) { m_pSelectObject = pObject; }
	vector<shared_ptr< CGameObject>>* Get_Objects() { return m_vecObjects; }

	void			Set_ToolType(eToolType eToolType, _bool bCanChange) { m_eToolType = eToolType; m_bCanChange = bCanChange; }

	void			Clear_Objects();

	void			Tool_Change();

public:
	void			Make_Map();
	void			Save_Map();
	void			Load_Map();

	void			Level_MapMake(_uint iMapNum);
	void	        Level_MapLoad(_uint _iMapNum, const string& strFilePath);

	void			Make_EffectData(UNION_EFFECT_DATA _sUnionData);
	void			Save_EffectData();
	void			Load_EffectData();



	void			Init_Effect_Prototype();

	void			Make_Effect();
	void			Save_Effect();
	void			Load_Effect();

private:
	_bool m_bRotCheck = false;
	_float3 m_vRot = { 0.f,0.f,0.f };

private:
	eImgui_State m_eState = eImgui_State::IMGUI_STATE_WAIT;

	eMapLoadType	m_eMapLoadType		= eMapLoadType::MAP_END;
	eEffectLoadType m_eEffectLoadType	= eEffectLoadType::EFFECT_END;

	eToolType		m_eToolType		= eToolType::TOOL_END;
	
	shared_ptr<CGameObject> m_pSelectObject = nullptr;

	tagGizmoDesc m_tGizmoDesc;

	eObjectType m_eObjectType = TESTOBJECT;

	vector<shared_ptr<CGameObject>> m_vecObjects[OBJTYPE_END];

	vector<MapObjData> m_vecMapObjData;

	string m_strMapName = "MapData01";

	vector<string> m_vecObjectNames;

private:
	map<string,vector<shared_ptr<CEffect_Base>>> m_mEffects;
	vector<string> m_vecEffectNames;
	vector<string> m_vecPtEffectStyleNames;
	int m_strSelectEffectNum = 0;
	
	CEffect_Base::EFFECT_TYPE m_eEffectType = CEffect_Base::EFFECT_TEXTURE;
	_bool m_bCanChange = true;

	string m_strUnionEffectKey = "";

	/* Map Tool */
	vector<string> m_vecModelTags;
	vector<string> m_vecCloneNames[OBJTYPE_END];
	vector<string> m_vecObjectTags;
	string m_strSelectObjectTag;

	_int m_iCurrModelIndex = 0;
	_int m_iCurrCloneIndex = 0;
	_int m_iCurrInteractiveIndex = 0;

	vector<_bool> m_vecCollision;

private:
	_bool m_bIsUnionLink = false;
	_bool m_bUnionPlay = false;
	_bool m_bUnionPause = false;

	_float m_fUnionLinkTime = 0.f;
	_float m_fUnionLinkTimeMax = 0.f;
	_float m_fUnionLinkTimePercent = 0.f;

	void EffectLink();
	void EffectUnLink();
	void EffectPause();
	void UnionEnd();
	void UnionStart();

private:
	ImGui::FileBrowser fileDialog;
	ImGui::FileBrowser fileDialog2;

/* For Anim Tool */
private:
	vector<string> m_AnimList;
	_int m_iAnim_Select = -1;

	string m_strAnimFilePath = "";

	shared_ptr<class CFree_SK_Object> m_pFreeObj = nullptr;

#pragma region FRACTURE_TOOL
public:
	void Add_Mesh(const string& szModelComTag)
	{
		meshNames.push_back(szModelComTag);
	}
	void Add_Mesh(const wstring& szModelComTag)
	{
		Add_Mesh(string(szModelComTag.begin(), szModelComTag.end()));
	}
private:
	void Imgui_LoadFractureTexture();

private:
	struct MeshObject {
		string name;
		string meshName;
		CCSG_Tester::FractureOptions options;
	};

	vector<string> meshNames = { };
	vector<MeshObject> objects;
	vector<shared_ptr<CCSG_Tester>> FractureObjects;
	vector<pair<string, shared_ptr<CTexture>>> FractureMaterials;
	int selectedMeshIndex = 0;
	int selectedObjectIndex = -1;
	int selectedMaterialIndex = 0;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
#pragma endregion

#pragma region BREAK_TOOL
private:
	struct BreakableObject {
		string name;
		string meshName;
	};
	int selectedBreakableObjectIndex = -1;
	vector<BreakableObject> breakableObjects;
	vector<shared_ptr<CBreakable>> BreakableObjects;
	shared_ptr<CGameObject> m_pDamageApplier;
#pragma endregion

#pragma region GEO_TOOL
private:
	struct GeometryObject {
		string name;
		string meshName;
	};
	int selectedGeoObjectIndex = -1;
	vector<GeometryObject> GeoObjects;
	vector<shared_ptr<CGeometryBrush>> GeoBrushObjects;
	int selectedGeoA = -1, selectedGeoB = -1;

#pragma endregion

#pragma region WOUND_TOOL
public:
	void	Imgui_WoundTool();

private:
	void Guizmo(_uint idx);
private:
	shared_ptr<CWoundable> WoundableObject;
	int ellipsIdx = 0;

#pragma endregion
public:
	void Free();
};

END
