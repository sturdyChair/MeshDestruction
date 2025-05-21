#include "Imgui_Manager.h"
#include "Light.h"
#include "GameInstance.h"
#include "Effect_Base.h"
#include "Load_Include.h"

#include "MapObject_Static.h"

#include "GameManager.h"

#include "Model.h"
#include "Animation.h"

#include "Free_SK_Object.h"

#include "UnionEffectData.h"

#include "GeometryBrush.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

extern IMGUI_IMPL_API bool ImGui_ImplDX11_Init(ID3D11Device* device,
	ID3D11DeviceContext* device_context);

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{

}

CImgui_Manager::~CImgui_Manager()
{
	Free();
}


void CImgui_Manager::Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Device, pContext);
	m_pDevice = Device;
	m_pContext = pContext;
	ImGui::StyleColorsDark();

	m_vecObjectNames.emplace_back("Light");
	m_vecObjectNames.emplace_back("TestObject");
	m_vecObjectNames.emplace_back("StaticObject");

	m_vecObjectTags.emplace_back("CTrigger");

		m_vecPtEffectStyleNames.emplace_back("STYLE_NONE");
		m_vecPtEffectStyleNames.emplace_back("STYLE_SPREAD");
		m_vecPtEffectStyleNames.emplace_back("STYLE_DROP");
		m_vecPtEffectStyleNames.emplace_back("STYLE_FOUNTAIN");
		m_vecPtEffectStyleNames.emplace_back("STYLE_RISING");
		m_vecPtEffectStyleNames.emplace_back("STYLE_CYCLE");
		m_vecPtEffectStyleNames.emplace_back("STYLE_ORBITAL");
		//m_vecPtEffectStyleNames.emplace_back("STYLE_END");

	fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	fileDialog2 = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
}

HRESULT CImgui_Manager::Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	return S_OK;
}

HRESULT CImgui_Manager::Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ImGui_ImplDX11_Init(pDevice, pContext);

	return S_OK;
}

void CImgui_Manager::PreTick()
{
	if (GAMEINSTANCE->Get_KeyDown(DIK_P)) {

		if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::TRANSLATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::ROTATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::SCALE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
}

void CImgui_Manager::Tick()
{
	Tool_Change();

	//if (m_eToolType != eToolType::TOOL_EFFECT){
	//	if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
	//		Pick_Object();
	//}

	if (m_bIsUnionLink && m_bUnionPlay && m_bUnionPause == false) {
		m_fUnionLinkTime += GAMEINSTANCE->Get_TimeDelta(TEXT("Timer_60"));
		if (m_fUnionLinkTime > m_fUnionLinkTimeMax) {
			m_bUnionPlay = false;
			m_bUnionPause = false;
			m_fUnionLinkTime = 0.f;
			UnionEnd();
		}
	}
}

void CImgui_Manager::LateTick()
{
}

void CImgui_Manager::Tick_Engine(_float fTimeDelta)
{
	if (Imgui_GetState() == eImgui_State::IMGUI_STATE_SHUTDOWN) return;

	PreTick();
	Tick();
	LateTick();
}

void CImgui_Manager::Imgui_Render()
{
	if (m_eState != eImgui_State::IMGUI_STATE_RENDER) return;

	switch (m_eToolType)
	{
	case eToolType::TOOL_MAP:
		Imgui_MapTool();
		break;
	case eToolType::TOOL_EFFECT:
		Imgui_LinkTool();
		break;
	case eToolType::TOOL_SOUND:
		Imgui_SoundTool();
		break;
	case eToolType::TOOL_ANIMATION:
		Imgui_AnimationTool();
		break;
	case eToolType::TOOL_FRACTURE:
		Imgui_FractureTool();
		break;
	case eToolType::TOOL_WOUND:
		Imgui_WoundTool();
		break;
	case eToolType::TOOL_BREAKABLE:
		Imgui_BreakableObjectTool();
		break;
	case eToolType::TOOL_GEOBRUSH:
		Imgui_GeometryBrushTool();
		break;
	case eToolType::TOOL_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Imgui_MapTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("MapData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_MenuBar();

		ImGui::InputText("Data File Name", m_strMapName.data(), sizeof(_tchar) * MAX_PATH);

		if (ImGui::BeginTabBar("##", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Static"))
			{
				m_eObjectType = MAPOBJ_STATIC;

				if (ImGui::BeginListBox("Model List"))
				{
					for (_uint i = 0; i < m_vecModelTags.size(); i++)
					{
						_bool bIsSelected = (m_iCurrModelIndex == i);
						if (ImGui::Selectable(m_vecModelTags[i].c_str(), bIsSelected))
							m_iCurrModelIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(7, 35.5));
				if (ImGui::Button("Create", ImVec2(107, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[MAPOBJ_STATIC].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[MAPOBJ_STATIC][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrCloneIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex]);
						}
					}

					ImGui::EndListBox();
				}

				if (m_vecCollision.size() > 0)
				{
					_bool bCollision = m_vecCollision[m_iCurrCloneIndex];
					ImGui::Checkbox("Collision", &bCollision);
					m_vecCollision[m_iCurrCloneIndex] = bCollision;
				}

				//ImGui::SetCursorPos(ImVec2(227, 35.5));
				if (ImGui::Button("Delete", ImVec2(107, 19))) {
					if (m_pSelectObject != nullptr) {
						m_pSelectObject->Dead();

						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[MAPOBJ_STATIC].erase(m_vecCloneNames[MAPOBJ_STATIC].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
					}
				}

				if (ImGui::Button("Clear", ImVec2(107, 19)))
				{
					Clear_Objects();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Animated"))
			{
				m_eObjectType = MAPOBJ_ANIM;

				if (ImGui::BeginListBox("Model List"))
				{
					for (_uint i = 0; i < m_vecModelTags.size(); i++)
					{
						_bool bIsSelected = (m_iCurrModelIndex == i);
						if (ImGui::Selectable(m_vecModelTags[i].c_str(), bIsSelected))
							m_iCurrModelIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(7, 35.5));
				if (ImGui::Button("Create", ImVec2(107, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[MAPOBJ_ANIM].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrCloneIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[MAPOBJ_ANIM][m_iCurrCloneIndex]);
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(227, 35.5));
				if (ImGui::Button("Delete", ImVec2(107, 19))) {
					if (m_pSelectObject != nullptr) {
						m_pSelectObject->Dead();

						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Interactive"))
			{
				m_eObjectType = MAPOBJ_INTERACTIVE;

				if (m_strSelectObjectTag.size() == 0)
					m_strSelectObjectTag = m_vecObjectTags[0];

				if (ImGui::BeginCombo("##", m_vecObjectTags[0].data(), ImGuiComboFlags_None))
				{
					for (int n = 0; n < m_vecObjectTags.size(); n++)
					{
						if (ImGui::Selectable(m_vecObjectTags[n].data(), n == m_eObjectType))
							m_strSelectObjectTag = m_vecObjectTags[n];
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Create", ImVec2(214, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[MAPOBJ_INTERACTIVE].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrInteractiveIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[MAPOBJ_INTERACTIVE][m_iCurrInteractiveIndex]);
						}
					}

					ImGui::EndListBox();
				}

				if (ImGui::Button("Delete", ImVec2(214, 19))) {
					if (m_pSelectObject != nullptr) {
						m_pSelectObject->Dead();

						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
					}

				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		if (m_eMapLoadType == eMapLoadType::MODELNAME_LOAD)
			Load_ModelNames(fileDialog.GetSelected().wstring() + L"\\");
		else
			Imgui_MapDataControl();

		m_eMapLoadType = eMapLoadType::MAP_END;
		fileDialog.ClearSelected();
	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_EffectTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("EffectData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_EffectMenuBar();

		ImGui::InputText("Effect Key Input", m_strUnionEffectKey.data(), sizeof(_tchar) * MAX_PATH);

		ImGui::PushItemWidth(200.f);
 		if (ImGui::Checkbox("Effect Union Link", &m_bIsUnionLink)) {
			if (m_bIsUnionLink == false) {
				EffectUnLink();
			}
			else {
				EffectLink();
			}
		}
		if (m_bIsUnionLink) {
			ImGui::SameLine();
			if (!m_bUnionPlay) {
				if (ImGui::Button("Effect Union Play")) {
					UnionStart();
					m_bUnionPlay = true;
					m_fUnionLinkTime = 0;
				}
			}
			else {
				if (!m_bUnionPause) {
					if (ImGui::Button("Effect Union Pause")) {
						m_bUnionPause = true;
						EffectPause();
					}
				}
				else {
					if (ImGui::Button("Effect Union Resume")) {
						m_bUnionPause = false;
						EffectPause();
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Effect Union Stop")) {
				m_bUnionPlay = false;
				m_bUnionPause = false;
				m_fUnionLinkTime = 0;
				UnionEnd();
			}
		}

		ImGui::BeginChild(13, ImVec2(460, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecEffectNames[m_strSelectEffectNum].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecEffectNames.size(); n++) {
				if (ImGui::Selectable(m_vecEffectNames[n].data(), n == m_strSelectEffectNum)) {
					m_strSelectEffectNum = n;
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(214, 19))) {
			Imgui_EffectCreate();
		}

		ImGui::SetCursorPos(ImVec2(227, 35.5));
		if (ImGui::Button("Delete", ImVec2(214, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Dead();

				for (_uint i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
					if (m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][i] == m_pSelectObject) {
						m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].erase(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].begin() + i);
						break;
					}
				}

				m_pSelectObject = m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].empty() ? nullptr : static_pointer_cast<CEffect_Base>(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
			}
		}

		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;

		if (ImGui::BeginListBox("##", ImVec2(428, 150))) {
			for (int i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
				string str = m_vecEffectNames[m_strSelectEffectNum];
				if (ImGui::Selectable((str += to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_EffectOptionRender();
		}

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_EffectDataControl();
		m_eEffectLoadType = eEffectLoadType::EFFECT_END;
		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_SoundTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("SoundData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_MenuBar();

		ImGui::SetCursorPos(ImVec2(19, 34));
		ImGui::Button("Object", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(74.5, 34));
		ImGui::Button("Light", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(122.5, 34));
		ImGui::Button("Model", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(12, 63));
		ImGui::BeginChild(13, ImVec2(448, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecObjectNames[m_eObjectType].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecObjectNames.size(); n++) {
				if (ImGui::Selectable(m_vecObjectNames[n].data(), n == m_eObjectType)) {
					m_eObjectType = static_cast<eObjectType>(n);
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(214, 19))) {
			Imgui_ObjCreate();
		}

		ImGui::SetCursorPos(ImVec2(227, 35.5));
		if (ImGui::Button("Delete", ImVec2(214, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Dead();

				for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
					if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
						m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
						break;
					}
				}
				m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
			}

		}

		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;


		if (ImGui::BeginListBox("##", ImVec2(428, 300))) {
			for (int i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
				if (ImGui::Selectable((m_vecObjectNames[i] += to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_vecObjects[m_eObjectType][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_MapDataControl();
		m_eMapLoadType = eMapLoadType::MAP_END;
		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_AnimationTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("Animation Setting");
	{
		// Resource List
		if (ImGui::Button("Load Model"))
		{
			fileDialog = ImGui::FileBrowser(0, "../Bin/Resources/Models/");
			fileDialog.SetTypeFilters({ ".fbx" });
			fileDialog.Open();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Model"))
		{
			if (m_pFreeObj != nullptr)
			{
				m_pFreeObj->Dead();
				m_pFreeObj = nullptr;

				m_iAnim_Select = -1;
				m_AnimList.clear();
			}
		}

		if (ImGui::BeginListBox("##Anim List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_int iSelectIndex = -1;
			for (auto& strAnim : m_AnimList)
			{
				++iSelectIndex;
				const bool is_selected = (m_iAnim_Select == iSelectIndex);
				if (ImGui::Selectable(strAnim.c_str(), is_selected))
				{
					if (m_iAnim_Select == iSelectIndex)
					{
						m_iAnim_Select = -1;
					}
					else
					{
						m_iAnim_Select = iSelectIndex;

						m_pFreeObj->SwitchAnim(m_AnimList[m_iAnim_Select]);
					}
				}
			}
			ImGui::EndListBox();
		}

		if (nullptr != m_pFreeObj && -1 != m_iAnim_Select)
		{
			shared_ptr<CAnimation> pAnimSelected = nullptr;
			m_pFreeObj->GetAnimInfo(pAnimSelected, m_iAnim_Select);
			ImGui::Checkbox("RootAnim", &pAnimSelected->m_isRootAnim);
			ImGui::SameLine();
			ImGui::Checkbox("ForcedNonRoot", &pAnimSelected->m_isForcedNonRootAnim);

			static _float fFramePos = 0.f;
			ImGui::SliderFloat("Frame", &fFramePos, 0.f, pAnimSelected->Get_Duration(), "%.1f");
			m_pFreeObj->SetAnimFrame(fFramePos);

			if (ImGui::Button("Save CallBack"))
			{
				pAnimSelected->m_FrameCallbackList.push_back(fFramePos);
				sort(pAnimSelected->m_FrameCallbackList.begin(), pAnimSelected->m_FrameCallbackList.end());
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete CallBack"))
			{
				if (pAnimSelected->m_FrameCallbackList.size() > 0)
					pAnimSelected->m_FrameCallbackList.pop_back();
			}

			if (ImGui::BeginListBox("##AnimationCallbackList"))
			{
				for (auto fFrame : pAnimSelected->m_FrameCallbackList)
				{
					_bool test = false;
					if (ImGui::Selectable(to_string(fFrame).c_str(), test))
						fFramePos = fFrame;
				}
				ImGui::EndListBox();
			}

			// Save animation set info
			if (ImGui::Button("Save Custom AnimInfo"))
			{
				string strFolderPath = m_strAnimFilePath;
				string subStr = ".fbx";
				size_t pos = strFolderPath.find(subStr);
				while (pos != string::npos) {
					strFolderPath.erase(pos, subStr.length());
					pos = strFolderPath.find(subStr);
				}
				strFolderPath += "/";

				if (FAILED(m_pFreeObj->ExportAnimInfo(strFolderPath)))
					MSG_BOX(TEXT("Failed to save"));
				else
					MSG_BOX(TEXT("Success to save"));
			}
		}
	}
	ImGui::End();

	fileDialog.Display();
	if (fileDialog.HasSelected())
	{
		m_strAnimFilePath = fileDialog.GetSelected().string();
		auto iCurLevel = CGame_Manager::Get_Instance()->Get_Current_Level();

		CFree_SK_Object::DESC desc{};
		desc.strModelPath = m_strAnimFilePath;
		m_pFreeObj = dynamic_pointer_cast<CFree_SK_Object>(GAMEINSTANCE->Add_Clone_Return(iCurLevel, TEXT("TEST_LAYER"), CFree_SK_Object::ObjID, &desc));
		m_pFreeObj->GetAnimStrList(m_AnimList);

		fileDialog.ClearSelected();
	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_LinkTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("EffectData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_EffectMenuBar();

		ImGui::InputText("Effect Key Input", m_strUnionEffectKey.data(), sizeof(_tchar) * MAX_PATH);

		ImGui::PushItemWidth(200.f);
		if (ImGui::Checkbox("Effect Union Link", &m_bIsUnionLink)) {
			if (m_bIsUnionLink == false) {
				EffectUnLink();
			}
			else {
				EffectLink();
			}
		}
		if (m_bIsUnionLink) {
			ImGui::SameLine();
			if (!m_bUnionPlay) {
				if (ImGui::Button("Effect Union Play")) {
					UnionStart();
					m_bUnionPlay = true;
					m_fUnionLinkTime = 0;
				}
			}
			else {
				if (!m_bUnionPause) {
					if (ImGui::Button("Effect Union Pause")) {
						m_bUnionPause = true;
						EffectPause();
					}
				}
				else {
					if (ImGui::Button("Effect Union Resume")) {
						m_bUnionPause = false;
						EffectPause();
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Effect Union Stop")) {
				m_bUnionPlay = false;
				m_bUnionPause = false;
				m_fUnionLinkTime = 0;
				UnionEnd();
			}
		}

		ImGui::BeginChild(13, ImVec2(460, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecEffectNames[m_strSelectEffectNum].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecEffectNames.size(); n++) {
				if (ImGui::Selectable(m_vecEffectNames[n].data(), n == m_strSelectEffectNum)) {
					m_strSelectEffectNum = n;
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(214, 19))) {
			Imgui_EffectCreate();
		}

		ImGui::SetCursorPos(ImVec2(227, 35.5));
		if (ImGui::Button("Delete", ImVec2(214, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Dead();

				for (_uint i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
					if (m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][i] == m_pSelectObject) {
						m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].erase(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].begin() + i);
						break;
					}
				}

				m_pSelectObject = m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].empty() ? nullptr : static_pointer_cast<CEffect_Base>(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
			}
		}

		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;

		if (ImGui::BeginListBox("##", ImVec2(428, 150))) {
			for (int i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
				string str = m_vecEffectNames[m_strSelectEffectNum];
				if (ImGui::Selectable((str += to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_EffectOptionRender();
		}

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_EffectDataControl();
		m_eEffectLoadType = eEffectLoadType::EFFECT_END;
		fileDialog.ClearSelected();
	}


	//Animation Tool
	ImGui::Begin("Animation Setting");
	{
		// Resource List
		if (ImGui::Button("Load Model"))
		{
			fileDialog2 = ImGui::FileBrowser(0, "../Bin/Resources/Models/");
			fileDialog2.SetTypeFilters({ ".fbx" });
			fileDialog2.Open();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Model"))
		{
			if (m_pFreeObj != nullptr)
			{
				m_pFreeObj->Dead();
				m_pFreeObj = nullptr;

				m_iAnim_Select = -1;
				m_AnimList.clear();
			}
		}

		if (ImGui::BeginListBox("##Anim List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_int iSelectIndex = -1;
			for (auto& strAnim : m_AnimList)
			{
				++iSelectIndex;
				const bool is_selected = (m_iAnim_Select == iSelectIndex);
				if (ImGui::Selectable(strAnim.c_str(), is_selected))
				{
					if (m_iAnim_Select == iSelectIndex)
					{
						m_iAnim_Select = -1;
					}
					else
					{
						m_iAnim_Select = iSelectIndex;

						m_pFreeObj->SwitchAnim(m_AnimList[m_iAnim_Select]);
					}
				}
			}
			ImGui::EndListBox();
		}

		if (nullptr != m_pFreeObj && -1 != m_iAnim_Select)
		{
			shared_ptr<CAnimation> pAnimSelected = nullptr;
			m_pFreeObj->GetAnimInfo(pAnimSelected, m_iAnim_Select);
			ImGui::Checkbox("RootAnim", &pAnimSelected->m_isRootAnim);
			ImGui::SameLine();
			ImGui::Checkbox("ForcedNonRoot", &pAnimSelected->m_isForcedNonRootAnim);

			static _float fFramePos = 0.f;
			ImGui::SliderFloat("Frame", &fFramePos, 0.f, pAnimSelected->Get_Duration(), "%.1f");
			m_pFreeObj->SetAnimFrame(fFramePos);

			static string strPlay = "Play";
			if (ImGui::Button(strPlay.c_str()))
				strPlay = m_pFreeObj->SwitchPlayMode();
			ImGui::SameLine();
			ImGui::ProgressBar(pAnimSelected->Get_Current_Track_Posiiton() / pAnimSelected->Get_Duration());


			if (ImGui::Button("Save CallBack"))
			{
				pAnimSelected->m_FrameCallbackList.push_back(fFramePos);
				sort(pAnimSelected->m_FrameCallbackList.begin(), pAnimSelected->m_FrameCallbackList.end());
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete CallBack"))
			{
				if (pAnimSelected->m_FrameCallbackList.size() > 0)
					pAnimSelected->m_FrameCallbackList.pop_back();
			}

			if (ImGui::BeginListBox("##AnimationCallbackList"))
			{
				for (auto fFrame : pAnimSelected->m_FrameCallbackList)
				{
					_bool test = false;
					if (ImGui::Selectable(to_string(fFrame).c_str(), test))
						fFramePos = fFrame;
				}
				ImGui::EndListBox();
			}

			// Save animation set info
			if (ImGui::Button("Save Custom AnimInfo"))
			{
				string strFolderPath = m_strAnimFilePath;
				string subStr = ".fbx";
				size_t pos = strFolderPath.find(subStr);
				while (pos != string::npos) {
					strFolderPath.erase(pos, subStr.length());
					pos = strFolderPath.find(subStr);
				}
				strFolderPath += "/";

				if (FAILED(m_pFreeObj->ExportAnimInfo(strFolderPath)))
					MSG_BOX(TEXT("Failed to save"));
				else
					MSG_BOX(TEXT("Success to save"));
			}
		}
	}
	ImGui::End();

	fileDialog2.Display();
	if (fileDialog2.HasSelected())
	{
		m_strAnimFilePath = fileDialog2.GetSelected().string();
		auto iCurLevel = CGame_Manager::Get_Instance()->Get_Current_Level();

		CFree_SK_Object::DESC desc{};
		desc.strModelPath = m_strAnimFilePath;
		m_pFreeObj = dynamic_pointer_cast<CFree_SK_Object>(GAMEINSTANCE->Add_Clone_Return(iCurLevel, TEXT("TEST_LAYER"), CFree_SK_Object::ObjID, &desc));
		m_pFreeObj->GetAnimStrList(m_AnimList);

		fileDialog2.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_FractureTool()
{
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("Fracture Tool");

	ImGui::Text("Available Meshes:");
	if (ImGui::BeginCombo("##MeshList", meshNames[selectedMeshIndex].c_str())) {
		for (int i = 0; i < meshNames.size(); ++i) {
			bool isSelected = (i == selectedMeshIndex);
			if (ImGui::Selectable(meshNames[i].c_str(), isSelected)) {
				selectedMeshIndex = i;
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// 2. 객체 생성
	if (ImGui::Button("Create Object")) {
		MeshObject obj;
		obj.meshName = meshNames[selectedMeshIndex];
		obj.name = obj.meshName + "_Instance_" + std::to_string(objects.size());
		objects.push_back(obj);

		CCSG_Tester::FDO_DESC CSGDesc{};
		CSGDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		CSGDesc.strModelTag = wstring(obj.meshName.begin(), obj.meshName.end());
		CSGDesc.vScale = { 100.f,100.f,100.f };
		FractureObjects.push_back(static_pointer_cast<CCSG_Tester>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Fracture", CCSG_Tester::ObjID, &CSGDesc)));

		

	}

	ImGui::Separator();

	// 3. 객체 리스트 및 선택
	ImGui::Text("Objects:");
	for (int i = 0; i < objects.size(); ++i) {
		bool isSelected = (i == selectedObjectIndex);
		if (ImGui::Selectable(objects[i].name.c_str(), isSelected)) {
			selectedObjectIndex = i;
		}
	}

	// 4. 선택한 객체 정보 및 옵션 표시
	if (selectedObjectIndex >= 0 && selectedObjectIndex < objects.size()) {
		MeshObject& obj = objects[selectedObjectIndex];
		m_pSelectObject = FractureObjects[selectedObjectIndex];
		ImGui::Separator();
		ImGui::Text("Selected Object: %s", obj.name.c_str());

		// 5. 객체의 Fracture 옵션
		ImGui::Text("Fracture Options:");
		ImGui::InputInt("Fracture Count", &obj.options.fractureCount, 1, 10);
		ImGui::InputFloat3("Fracture Scale", &obj.options.vScale.x);
		ImGui::InputInt("Cluster Size",		&obj.options.ClusterSize, 1, 10);
		ImGui::Checkbox("Cluster Destruction", &obj.options.bCrusterFracture);
		ImGui::SliderInt("Fracture Mode", &obj.options.iMode, 0, CModel::FRACTURE_MODE::FRACTURE_END - 1);
		
		obj.options.mode = (CModel::FRACTURE_MODE)obj.options.iMode;
		if (obj.options.mode == CModel::SPIDER_WEB)
		{
			ImGui::InputFloat3("Fracture Center", &obj.options.vCenter.x);
			ImGui::InputInt("Web Num Angle", &obj.options.WebNumAngle);
			ImGui::InputInt("Web Num Layer", &obj.options.WebNumLayer);
		}
		if (obj.options.mode == CModel::HONEYCOMB)
		{
			ImGui::InputInt("Comb Num Y-Axis", &obj.options.WebNumAngle);
			ImGui::InputInt("Comb Num Z-Axis", &obj.options.WebNumLayer);
		}
		if (obj.options.mode == CModel::VOXEL)
		{
			ImGui::InputInt("Voxel Num Y-Axis", &obj.options.WebNumAngle);
			ImGui::InputInt("Voxel Num Z-Axis", &obj.options.WebNumLayer);
		}
		if (ImGui::Button("Fracture!")) {
			FractureObjects[selectedObjectIndex]->Dispatch_Fracture(obj.options);
		}
	}

	if (ImGui::Button("Load Textures"))
	{
		fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory, "../Bin/Resources/Models/");
		fileDialog.Open();
	}

	ImGui::Separator();

	ImGui::Text("Materials:");
	for (int i = 0; i < FractureMaterials.size(); ++i) 
	{
		bool isSelected = (i == selectedMaterialIndex);
		if (ImGui::Selectable(FractureMaterials[i].first.c_str(), isSelected)) 
		{
			selectedMaterialIndex = i;
		}
	}
	if(FractureMaterials.size())
	{
		ImGui::Begin("Material");
		auto srv = FractureMaterials[selectedMaterialIndex].second->Get_SRV(0);

		ImGui::Image((ImTextureID)(intptr_t)srv, ImVec2(100, 100));
		ImGui::End();

		if (ImGui::Button("Diffuse") && FractureObjects.size() > selectedObjectIndex)
		{
			FractureObjects[selectedObjectIndex]->Apply_Inner_Texture(FractureMaterials[selectedMaterialIndex].first, FractureMaterials[selectedMaterialIndex].second, aiTextureType_DIFFUSE);
		}

		if (ImGui::Button("Normal") && FractureObjects.size() > selectedObjectIndex)
		{
			FractureObjects[selectedObjectIndex]->Apply_Inner_Texture(FractureMaterials[selectedMaterialIndex].first, FractureMaterials[selectedMaterialIndex].second, aiTextureType_NORMALS);
		}
	}


	Imgui_GizmoRender();
	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_LoadFractureTexture();
		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_BreakableObjectTool()

{
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("Breakable Tool");

	ImGui::Text("Available Meshes:");
	if (ImGui::BeginCombo("##MeshList", meshNames[selectedMeshIndex].c_str())) {
		for (int i = 0; i < meshNames.size(); ++i) {
			bool isSelected = (i == selectedMeshIndex);
			if (ImGui::Selectable(meshNames[i].c_str(), isSelected)) {
				selectedMeshIndex = i;
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	static _float3 vScale = {1.f,1.f,1.f};
	static _int nbFracture = 100;
	static _float fThreshold = 0.f;
	ImGui::InputFloat3("Scale", &vScale.x);
	ImGui::InputFloat("Force Threshold", &fThreshold);
	ImGui::InputInt("Nb Fracture", &nbFracture);

	// 2. 객체 생성
	if (ImGui::Button("Create Object")) {
		BreakableObject obj;
		obj.meshName = meshNames[selectedMeshIndex];
		obj.name = obj.meshName + "_Instance_" + std::to_string(breakableObjects.size());
		breakableObjects.push_back(obj);

		CBreakable::FDO_DESC CSGDesc{};
		CSGDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		CSGDesc.strModelTag = wstring(obj.meshName.begin(), obj.meshName.end());
		CSGDesc.vScale = vScale;
		CSGDesc.iNumCell = nbFracture;
		CSGDesc.fForceThreshold = fThreshold;
		CSGDesc.strInD = L"DirtRockInDiff";
		CSGDesc.strInN = L"DirtRockInNorm";
		BreakableObjects.push_back(static_pointer_cast<CBreakable>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Fracture", CBreakable::ObjID, &CSGDesc)));
	}

	ImGui::Separator();

	// 3. 객체 리스트 및 선택
	ImGui::Text("Objects:");
	if(BreakableObjects.size())
	{
		if(!m_pDamageApplier)
		{
			m_pDamageApplier = GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"Layer_DA", CDamageApplier::ObjID);
		}
		m_pSelectObject = m_pDamageApplier;
		static _float3 vDAScale = { 5.f,5.f,5.f };
		ImGui::InputFloat3("ForceField : ", &vDAScale.x);
		m_pDamageApplier->Get_Transform()->Set_Scale(vDAScale);
	}
	

	ImGui::Separator();

	//Imgui_GizmoRender();
	ImGui::End();

	//fileDialog.Display();

	//if (fileDialog.HasSelected())
	//{
	//	Imgui_LoadFractureTexture();
	//	fileDialog.ClearSelected();
	//}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_GeometryBrushTool()
{
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("GeoBrush Tool");

	ImGui::Text("Available Meshes:");
	if (ImGui::BeginCombo("##MeshList", meshNames[selectedMeshIndex].c_str())) {
		for (int i = 0; i < meshNames.size(); ++i) {
			bool isSelected = (i == selectedMeshIndex);
			if (ImGui::Selectable(meshNames[i].c_str(), isSelected)) {
				selectedMeshIndex = i;
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	static _float3 vScale = { 100.f,100.f,100.f };
	ImGui::InputFloat3("Scale", &vScale.x);

	// 2. 객체 생성
	if (ImGui::Button("Create Object")) {
		GeometryObject obj;
		obj.meshName = meshNames[selectedMeshIndex];
		static int instancecount = 0;
		obj.name = obj.meshName + "_Instance_" + std::to_string(++instancecount);
		GeoObjects.push_back(obj);

		CGeometryBrush::FDO_DESC CSGDesc{};
		CSGDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		CSGDesc.strModelTag = wstring(obj.meshName.begin(), obj.meshName.end());
		CSGDesc.vScale = vScale;
		GeoBrushObjects.push_back(static_pointer_cast<CGeometryBrush>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"Layer_Geo", CGeometryBrush::ObjID, &CSGDesc)));
	}

	ImGui::Separator();

	// 3. 객체 리스트 및 선택
	ImGui::Text("Objects:");
	for (int i = 0; i < GeoObjects.size(); ++i) {
		bool isSelected = (i == selectedGeoObjectIndex);
		if (ImGui::Selectable(GeoObjects[i].name.c_str(), isSelected)) {
			selectedGeoObjectIndex = i;
		}
	}

	if (selectedGeoObjectIndex >= 0 && selectedGeoObjectIndex < GeoObjects.size()) {
		GeometryObject& obj = GeoObjects[selectedGeoObjectIndex];
		m_pSelectObject = GeoBrushObjects[selectedGeoObjectIndex];
		ImGui::Separator();
		ImGui::Text("Selected Object: %s", obj.name.c_str());
		if (ImGui::Button("A"))
			selectedGeoA = selectedGeoObjectIndex;
		if (ImGui::Button("B"))
			selectedGeoB = selectedGeoObjectIndex;
	}
	if(selectedGeoA != -1 && selectedGeoB != -1 && selectedGeoA != selectedGeoB)
	{
		if (ImGui::Button("Union"))
		{
			GeoBrushObjects[selectedGeoA]->Operation(GeoBrushObjects[selectedGeoB], CGeometryBrush::CSG_OP::UNION);
			GeoBrushObjects[selectedGeoB]->Dead();
			swap(GeoBrushObjects[selectedGeoB], GeoBrushObjects.back());
			GeoBrushObjects.pop_back();
			swap(GeoObjects[selectedGeoB], GeoObjects.back());
			GeoObjects.pop_back();
			selectedGeoA = -1;
			selectedGeoB = -1;
			selectedGeoObjectIndex = -1;
			m_pSelectObject = nullptr;
		}
		if (ImGui::Button("Intersect"))
		{
			GeoBrushObjects[selectedGeoA]->Operation(GeoBrushObjects[selectedGeoB], CGeometryBrush::CSG_OP::INTERSECT);
			GeoBrushObjects[selectedGeoB]->Dead();
			swap(GeoBrushObjects[selectedGeoB], GeoBrushObjects.back());
			GeoBrushObjects.pop_back();
			swap(GeoObjects[selectedGeoB], GeoObjects.back());
			GeoObjects.pop_back();
			selectedGeoA = -1;
			selectedGeoB = -1;
			selectedGeoObjectIndex = -1;
			m_pSelectObject = nullptr;
		}
		if (ImGui::Button("Subtract"))
		{
			GeoBrushObjects[selectedGeoA]->Operation(GeoBrushObjects[selectedGeoB], CGeometryBrush::CSG_OP::DIFF);
			GeoBrushObjects[selectedGeoB]->Dead();
			swap(GeoBrushObjects[selectedGeoB], GeoBrushObjects.back());
			GeoBrushObjects.pop_back();
			swap(GeoObjects[selectedGeoB], GeoObjects.back());
			GeoObjects.pop_back();
			selectedGeoA = -1;
			selectedGeoB = -1;
			selectedGeoObjectIndex = -1;
			m_pSelectObject = nullptr;
		}
	}

	ImGui::Separator();

	Imgui_GizmoRender();
	ImGui::End();

	//fileDialog.Display();

	//if (fileDialog.HasSelected())
	//{
	//	Imgui_LoadFractureTexture();
	//	fileDialog.ClearSelected();
	//}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_Main()
{
}

void CImgui_Manager::Imgui_GizmoRender()
{
	if (m_pSelectObject == nullptr)
		return;

	shared_ptr<CTransform> pTerrainTransform = static_pointer_cast<CTransform>(m_pSelectObject->Find_Component(L"Com_Transform"));
	_float4x4 WorldMat, ViewMat, ProjMat;
	_float4  vPos;
	static _float3 vRot = { 0.f,0.f,0.f };
	_float3 vScale;

	if (pTerrainTransform == nullptr)
		return;

	WorldMat = pTerrainTransform->Get_WorldMatrix();
	ViewMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	ProjMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_bool flag = false;

	ImGui::Text("");
	ImGui::Text("Transform Data");

	ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
	ImGui::PushItemWidth(200.f);
	flag = ImGui::InputFloat3("Trans", &vPos.x);
	if (flag) {
		pTerrainTransform->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));
	}
	ImGui::PushItemWidth(200.f);

	_float3 vRadianRot = {};
	
	flag = ImGui::DragFloat3("Rotat", &vRot.x);
	if (flag) {
		vRadianRot.x = XMConvertToRadians(vRot.x);
		vRadianRot.y = XMConvertToRadians(vRot.y);
		vRadianRot.z = XMConvertToRadians(vRot.z);
		pTerrainTransform->Rotation_Quaternion(XMLoadFloat3(&vRadianRot));
	}

	ImGui::PushItemWidth(200.f);

	flag = ImGui::InputFloat3("Scale", &vScale.x);
	if (flag) {
		if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f)
			pTerrainTransform->Set_Scale(vScale);
	}

	ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);

	if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11	
		, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
		, m_tGizmoDesc.CurrentGizmoMode				// WORLD, LOCAL
		, (_float*)&WorldMat					
		, NULL					
		, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL	
		, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		m_pSelectObject->Get_Transform()->Set_WorldMatrix(WorldMat);
		if (eToolType::TOOL_EFFECT == m_eToolType)
			static_pointer_cast<CEffect_Base>(m_pSelectObject)->SetRootMatrix(WorldMat);
	}

	if (m_eObjectType == LIGHT) {
		shared_ptr<CLight> pLight = static_pointer_cast<CLight>(m_pSelectObject);

		ImGui::PushItemWidth(200.f);
		ImGui::InputFloat4("Diffuse", pLight->Get_LightDiffusePtr());

		ImGui::InputFloat4("Ambient", pLight->Get_LightAmbientPtr());

		ImGui::InputFloat4("Specular", pLight->Get_LightSpecularPtr());

		ImGui::InputFloat("Range", pLight->Get_LightRangePtr());
	}


}

void CImgui_Manager::Imgui_EffectOptionRender()
{
	if (m_pSelectObject == nullptr)
		return;

	_bool flag = false;

	shared_ptr<CEffect_Base> pEffect = static_pointer_cast<CEffect_Base>(m_pSelectObject);
	
	ImGui::Text("");
	ImGui::Text("TimeData");

	ImGui::PushItemWidth(200.f);
	_float fEffectTimeAcc = pEffect->GetEffectTimeAcc();
	flag = ImGui::InputFloat("TimeAcc", &fEffectTimeAcc);
	if (flag) {
		pEffect->SetEffectTimeAcc(fEffectTimeAcc);
	}
	ImGui::SameLine();
	flag = ImGui::Button("+");
	if (flag) {
		pEffect->SetEffectTimeAcc(pEffect->GetEffectTimeAcc() + 0.1f);
		pEffect->ToolTurning(0.1f);
	}
	ImGui::SameLine();
	flag = ImGui::Button("-");
	if (flag) {
		pEffect->SetEffectTimeAcc(pEffect->GetEffectTimeAcc() - 0.1f);
		pEffect->ToolTurning(-0.1f);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimeMin = pEffect->GetEffectTimeMin();
	flag = ImGui::InputFloat("TimeAccMin", &fEffectTimeMin);
	if (flag) {
		pEffect->SetEffectTimeMin(fEffectTimeMin);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimeMax = pEffect->GetEffectTimeMax();
	flag = ImGui::InputFloat("TimeAccMax", &fEffectTimeMax);
	if (flag) {
		pEffect->SetEffectTimeMax(fEffectTimeMax);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimeMultiplier = pEffect->GetEffectTimeAccMultiplier();
	flag = ImGui::InputFloat("TimeAccMulti", &fEffectTimeMultiplier);
	if (flag) {
		pEffect->SetEffectTimeAccMultiplier(fEffectTimeMultiplier);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimePercent = pEffect->GetEffectTimePercent();
	//flag = ImGui::InputFloat("TimePersect", &fEffectTimePercent);
	if (ImGui::SliderFloat("TimePersect", &fEffectTimePercent, 0.f, 1.f)) {
		pEffect->Set_ProgressBar_Inversion(fEffectTimePercent);
	}
	ImGui::ProgressBar(fEffectTimePercent);

	ImGui::PushItemWidth(200.f);

	_bool fEffectTimeLoop = pEffect->GetEffectLoop();
	flag = ImGui::Checkbox("TimeAccLoop", &fEffectTimeLoop);
	if (flag) {
		pEffect->SetEffectLoop(fEffectTimeLoop);
	}
	ImGui::SameLine();
	_bool fEffectTimeFreeze = pEffect->GetEffectTimeFreeze();
	flag = ImGui::Checkbox("TimeAccFreeze", &fEffectTimeFreeze);
	if (flag) {
		pEffect->SetEffectTimeFreeze(fEffectTimeFreeze);
	}
	ImGui::SameLine();
	_bool fEffectView = pEffect->GetEffectIsView();
	flag = ImGui::Checkbox("EffectIsView", &fEffectView);
	if (flag) {
		pEffect->SetEffectIsView(fEffectView);
	}

	ImGui::Text("");
	ImGui::Text("BaseColorData");

	ImGui::PushItemWidth(200.f);

	_float4 fEffectBaseColor = pEffect->GetEffectBaseColor();
	flag = ImGui::InputFloat4("BaseColor", &fEffectBaseColor.x);
	if (flag) {
		pEffect->SetEffectBaseColorToColor(fEffectBaseColor);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectBaseColorBlendRate = pEffect->GetEffectColorBlendRate();
	flag = ImGui::InputFloat("BaseColorBlendRate", &fEffectBaseColorBlendRate);
	if (flag) {
		pEffect->SetEffectColorBlendRate(fEffectBaseColorBlendRate);
	}

	ImGui::Text("");
	ImGui::Text("MaskColorData");

	ImGui::PushItemWidth(200.f);
	_float4 fEffectColorPresetR = pEffect->GetColorPresetR();
	flag = ImGui::InputFloat4("ColorPresetR", &fEffectColorPresetR.x);
	if (flag) {
		pEffect->SetColorPresetR(fEffectColorPresetR);
	}

	ImGui::PushItemWidth(200.f);
	_float4 fEffectColorPresetG = pEffect->GetColorPresetG();
	flag = ImGui::InputFloat4("ColorPresetG", &fEffectColorPresetG.x);
	if (flag) {
		pEffect->SetColorPresetG(fEffectColorPresetG);
	}

	ImGui::PushItemWidth(200.f);
	_float4 fEffectColorPresetB = pEffect->GetColorPresetB();
	flag = ImGui::InputFloat4("ColorPresetB", &fEffectColorPresetB.x);
	if (flag) {
		pEffect->SetColorPresetB(fEffectColorPresetB);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectColorAlpha = pEffect->GetEffectAlpha();
	flag = ImGui::InputFloat("ColorAlpha", &fEffectColorAlpha);
	if (flag) {
		pEffect->SetEffectAlpha(fEffectColorAlpha);
	}

	ImGui::SameLine();

	_bool fEffectAlphaLink = pEffect->GetEffectAlphaLinkTime();
	flag = ImGui::Checkbox("Color Time-Alpha Link", &fEffectAlphaLink);
	if (flag) {
		pEffect->SetEffectAlphaLinkTime(fEffectAlphaLink);
	}

	ImGui::PushItemWidth(200.f);

	ImGui::Text("");
	ImGui::Text("UvFlowData");

	_bool fEffectUvFlowX = pEffect->GetUseEffectUvFlowX();
	flag = ImGui::Checkbox("Effect Uv Flow X", &fEffectUvFlowX);
	if (flag) {
		pEffect->SetUseEffectUvFlowX(fEffectUvFlowX);
	}
	ImGui::SameLine();
	_bool fEffectUvFlowY = pEffect->GetUseEffectUvFlowY();
	flag = ImGui::Checkbox("Effect Uv Flow Y", &fEffectUvFlowY);
	if (flag) {
		pEffect->SetUseEffectUvFlowY(fEffectUvFlowY);
	}

	ImGui::PushItemWidth(200.f);

	_float2 fEffectUvFlow = pEffect->GetEffectUvFlow();
	flag = ImGui::InputFloat2("UvFlow XY", &fEffectUvFlow.x);
	if (flag) {
		pEffect->SetEffectUvFlow(fEffectUvFlow.x, fEffectUvFlow.y);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSpeedX = pEffect->GetEffectUvFlowXSpeed();
	flag = ImGui::InputFloat("UvFlowSpeed X", &fEffectUvFlowSpeedX);
	if (flag) {
		pEffect->SetEffectUvFlowXSpeed(fEffectUvFlowSpeedX);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSpeedY = pEffect->GetEffectUvFlowYSpeed();
	flag = ImGui::InputFloat("UvFlowSpeed Y", &fEffectUvFlowSpeedY);
	if (flag) {
		pEffect->SetEffectUvFlowYSpeed(fEffectUvFlowSpeedY);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSizeMultiX = pEffect->GetEffectUvSizeXMultiplier();
	flag = ImGui::InputFloat("UvMultiple X", &fEffectUvFlowSizeMultiX);
	if (flag) {
		pEffect->SetEffectUvSizeXMultiplier(fEffectUvFlowSizeMultiX);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSizeMultiY = pEffect->GetEffectUvSizeYMultiplier();
	flag = ImGui::InputFloat("UvMultiple Y", &fEffectUvFlowSizeMultiY);
	if (flag) {
		pEffect->SetEffectUvSizeYMultiplier(fEffectUvFlowSizeMultiY);
	}

	ImGui::Text("");
	ImGui::Text("ETC Option");

	_bool fEffectGlow = pEffect->GetUseGlow();
	flag = ImGui::Checkbox("Effect Glow", &fEffectGlow);
	if (flag) {
		pEffect->SetUseGlow(fEffectGlow);
	}
	ImGui::SameLine();
	_bool fEffectDistortion = pEffect->GetUseDistortion();
	flag = ImGui::Checkbox("Effect Distortion", &fEffectDistortion);
	if (flag) {
		pEffect->SetUseDistortion(fEffectDistortion);
	}
	ImGui::SameLine();
	_bool fEffectTurn = pEffect->GetEffectIsTurn();
	flag = ImGui::Checkbox("Effect Turn", &fEffectTurn);
	if (flag) {
		pEffect->SetEffectIsTurn(fEffectTurn);
	}
	ImGui::PushItemWidth(200.f);
	_float fEffectTurnSpeed = pEffect->GetTurnSpeed();
	flag = ImGui::InputFloat("TurnSpeed", &fEffectTurnSpeed);
	if (flag) {
		pEffect->SetTurnSpeed(fEffectTurnSpeed);
	}

	if (static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectType() == CEffect_Base::EFFECT_PARTICLE) {
		ImGui::Text("");
		ImGui::Text("Particle Option");
		
		shared_ptr<CParticle_Effect> pParticleEffect = static_pointer_cast<CParticle_Effect>(m_pSelectObject);
		
		ImGui::PushItemWidth(200.f);
		_float3 fEffectPivot = pParticleEffect->Get_Pivot();
		flag = ImGui::InputFloat3("Pivot", &fEffectPivot.x);
		if (flag) {
			pParticleEffect->Set_Pivot(fEffectPivot);
		}

		ImGui::PushItemWidth(60.f);
		_float2 fEffectSpeed = pParticleEffect->Get_Speed();
		flag = ImGui::InputFloat("SpeedMin", &fEffectSpeed.x);
		if (flag) {
			if (fEffectSpeed.y > fEffectSpeed.x)
			pParticleEffect->Set_Speed(_float2(fEffectSpeed.x, fEffectSpeed.y));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("SpeedMax", &fEffectSpeed.y);
		if (flag) {
			if (fEffectSpeed.y > fEffectSpeed.x)
			pParticleEffect->Set_Speed(_float2(fEffectSpeed.x, fEffectSpeed.y));
		}

		ImGui::Text("");
		ImGui::Text("Particle Range");

		ImGui::PushItemWidth(40.f);
		_float3 fEffectRange = pParticleEffect->Get_Range();
		flag = ImGui::InputFloat("RangeX", &fEffectRange.x);
		if (flag) {
			pParticleEffect->Set_Range(_float3(fEffectRange.x, fEffectRange.y, fEffectRange.z));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("RangeY", &fEffectRange.y);
		if (flag) {
			pParticleEffect->Set_Range(_float3(fEffectRange.x, fEffectRange.y, fEffectRange.z));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("RangeZ", &fEffectRange.z);
		if (flag) {
			pParticleEffect->Set_Range(_float3(fEffectRange.x, fEffectRange.y, fEffectRange.z));
		}

		ImGui::PushItemWidth(60.f);
		_float2 fEffectScale = pParticleEffect->Get_Scale();
		flag = ImGui::InputFloat("ScaleMin", &fEffectScale.x);
		if (flag) {
			if (fEffectScale.y > fEffectScale.x)
			pParticleEffect->Set_Scale(_float2(fEffectScale.x, fEffectScale.y));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("ScaleMax", &fEffectScale.y);
		if (flag) {
			if (fEffectScale.y > fEffectScale.x)
			pParticleEffect->Set_Scale(_float2(fEffectScale.x, fEffectScale.y));
		}



		ImGui::PushItemWidth(200.f);
		_bool fEffectRandomize = pParticleEffect->Get_IsRandomize();
		flag = ImGui::Checkbox("Effect Randomize", &fEffectRandomize);
		if (flag) {
			pParticleEffect->Set_IsRandomize(fEffectRandomize);
		}

		ImGui::Text("");
		ImGui::Text("Particle Style");

		ImGui::PushItemWidth(200.f);
		_int iStyleNum = static_cast<_int>(pParticleEffect->Get_Style());
		if (ImGui::BeginCombo("####", m_vecPtEffectStyleNames[iStyleNum].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecPtEffectStyleNames.size(); n++) {
				if (ImGui::Selectable(m_vecPtEffectStyleNames[n].data(), n == iStyleNum)) {
					pParticleEffect->Set_Style(static_cast<CVIBuffer_Instance::INSTANCE_STYLE>(n));
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

	}

	if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE)) {
		if (GAMEINSTANCE->Get_KeyPressing(DIK_LCONTROL)) {
			for (_int i = 0; i < m_vecEffectNames.size(); ++i) {
				for (_int j = 0; j < m_mEffects[m_vecEffectNames[i]].size(); ++j)
					m_mEffects[m_vecEffectNames[i]][j]->SetEffectTimeFreeze(!(m_mEffects[m_vecEffectNames[i]][j]->GetEffectTimeFreeze()));
			}
				
		}
		else {
			pEffect->SetEffectTimeFreeze(!(pEffect->GetEffectTimeFreeze()));
		}//pEffect->SetEffectIsTurn(!(pEffect->GetEffectIsTurn()));
	}




}

void CImgui_Manager::Imgui_MenuBar()
{

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Map"))
			{
				fileDialog = ImGui::FileBrowser();
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MAP_LOAD;
			}

			if (ImGui::MenuItem("Save Map"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MAP_SAVE;
				//Make_Map();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Load ModelNames"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MODELNAME_LOAD;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Load FModel Data"))
			{
				fileDialog = ImGui::FileBrowser(0);
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::JSON_LOAD;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void CImgui_Manager::Imgui_EffectMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Effect"))
			{
				fileDialog = ImGui::FileBrowser();
				fileDialog.Open();
				m_eEffectLoadType = eEffectLoadType::EFFECT_LOAD;
			}

			if (ImGui::MenuItem("Save Effect"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
				fileDialog.Open();
				m_eEffectLoadType = eEffectLoadType::EFFECT_SAVE;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void CImgui_Manager::Imgui_ObjCreate()
{
	CTransform::TRANSFORM_DESC tDesc{};
	XMStoreFloat4(&tDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());

	/*LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	XMStoreFloat4(&LightDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());
	LightDesc.fRange = 20.f;
	strcpy_s(LightDesc.szName, "Light_Point");
	LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;*/

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	XMStoreFloat4(&LightDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());
	LightDesc.fRange = 20.f;
	strcpy_s(LightDesc.szName, "Light_Point");
	LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.g_fSpotInnerConeAngle = 0.f;
	LightDesc.g_fSpotOuterConeAngle = 0.7f;
	LightDesc.g_fLightFalloff = 0.01f;

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	staticDesc.fSpeedPerSec = 0.f;
	staticDesc.fRotationPerSec = 0.f;
	staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };

	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	animDesc.fSpeedPerSec = 0.f;
	animDesc.fRotationPerSec = 0.f;
	animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };

	_tchar strWcharBuffer[MAX_PATH] = {};

	switch (m_eObjectType)
	{
	case LIGHT: {
		m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&LightDesc));
		Imgui_SetSelectObject(m_vecObjects[LIGHT].back());
		break;
	}
	case TESTOBJECT: {
		m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), TEXT("Prototype_GameObject_TestObject"), &tDesc));
		Imgui_SetSelectObject(m_vecObjects[TESTOBJECT].back());
		break;
	}
	case MAPOBJ_STATIC: {
		m_vecObjects[MAPOBJ_STATIC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Static::ObjID, &staticDesc));
		static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC].back())->Set_ModelComponent(m_vecModelTags[m_iCurrModelIndex]);
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_STATIC].back());
		m_vecCloneNames[MAPOBJ_STATIC].emplace_back(m_vecModelTags[m_iCurrModelIndex]);
		m_vecCollision.emplace_back(true);
		break;
	}
	case MAPOBJ_ANIM: {
		m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Anim::ObjID, &animDesc));
		static_pointer_cast<CMapObject_Anim>(m_vecObjects[MAPOBJ_ANIM].back())->Set_ModelComponent(m_vecModelTags[m_iCurrModelIndex]);
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_ANIM].back());
		m_vecCloneNames[MAPOBJ_ANIM].emplace_back(m_vecModelTags[m_iCurrModelIndex]);
		break;
	}
	case MAPOBJ_INTERACTIVE: {
		MultiByteToWideChar(CP_ACP, 0, m_strSelectObjectTag.c_str(), (_int)m_strSelectObjectTag.length(), strWcharBuffer, MAX_PATH);
		m_vecObjects[MAPOBJ_INTERACTIVE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), wstring(strWcharBuffer), &tDesc));
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_INTERACTIVE].back());
		m_vecCloneNames[MAPOBJ_INTERACTIVE].emplace_back(m_strSelectObjectTag);
		break;
	}
	default:
		break;

	}
}

void CImgui_Manager::Imgui_EffectCreate()
{
	m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].push_back(static_pointer_cast<CEffect_Base>(GAMEINSTANCE->Add_Clone_Return(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), wstring().assign(m_vecEffectNames[m_strSelectEffectNum].begin(), m_vecEffectNames[m_strSelectEffectNum].end()))));
	Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
}

void CImgui_Manager::Imgui_MapDataControl()
{
	switch (m_eMapLoadType)
	{
	case eMapLoadType::MAP_LOAD:
		Load_Map();
		break;
	case eMapLoadType::MAP_SAVE:
		Save_Map();
		break;
	case eMapLoadType::JSON_LOAD:
		Load_FModel();
		break;
	case eMapLoadType::MAP_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Imgui_EffectDataControl()
{
	switch (m_eEffectLoadType)
	{
	case eEffectLoadType::EFFECT_LOAD:
		Load_Effect();
		break;
	case eEffectLoadType::EFFECT_SAVE:
		Save_Effect();
		break;
	case eEffectLoadType::EFFECT_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Load_ModelNames(wstring strFilePath)
{
	//m_vecModelTags.clear();
	m_iCurrModelIndex = 0;

	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	wstring strTargetPath = strFilePath + L"*.*";

	WIN32_FIND_DATA	fd;
	HANDLE	hFind = FindFirstFile(strTargetPath.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bContinue = true;

		do
		{
			if (!lstrcmp(L".", fd.cFileName) || !lstrcmp(L"..", fd.cFileName))
				continue;

			DWORD dwFileAttribute = fd.dwFileAttributes;

			if (dwFileAttribute == FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring strChildPath = strFilePath + fd.cFileName + L"\\";

				Load_ModelNames(strChildPath);
			}
			else if (dwFileAttribute == FILE_ATTRIBUTE_ARCHIVE)
			{
				wstring strFileName = fd.cFileName;

				_tchar szFullPath[MAX_PATH] = {};

				_tchar szFileName[MAX_PATH] = {};
				_tchar szExt[MAX_PATH] = {};

				_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				if (lstrcmp(L".FBX", szExt) && lstrcmp(L".fbx", szExt))
					continue;

				_char szStringPath[MAX_PATH] = {};
				WideCharToMultiByte(CP_ACP, 0, szFileName, MAX_PATH, szStringPath, MAX_PATH, NULL, NULL);

				m_vecModelTags.emplace_back(szStringPath);
				//wcscpy_s(szFullPath, strFilePath.c_str());
				//wcscat_s(szFullPath, strFileName.c_str());

				//size_t index = strFilePath.find_last_of(L'/');
				//wstring strSectionName = strFilePath.substr(0, index);
				//index = strSectionName.find_last_of(L'/');
				//strSectionName = strSectionName.substr(index + 1, strSectionName.length());
				//
				//wstring strPrototypeTag = L"Prototype_Component_Model_" + strSectionName;

				//m_List_ResourceInfo.push_back(
				//	make_shared<RES_MODEL>(strFileName, wstring(szFullPath), MODEL_TYPE::NONANIM, FMat)
				//);

			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}

void CImgui_Manager::Load_FModel()
{
	//Clear_Objects();
	m_vecMapObjData.clear();

	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();
	//szBinFilePath += "\\";
	//szBinFilePath += m_strMapName;
	//szBinFilePath += ".json";

	ifstream MapData(szBinFilePath);
	json importer = json::parse(MapData);
	MapData.close();

	for (auto& object : importer)
	{
		if (!object.contains("Properties"))
			continue;

		MapObjData tObjData;
		string strObjName;


		if (object["Type"] == "SceneComponent")
		{
			string strBPName = object["Outer"];

			if (strBPName.find("bg") == strBPName.npos && strBPName.find("SM_") == strBPName.npos)
				continue;

			auto index = strBPName.find("_BP");
			if (index != strBPName.npos)
				strBPName.erase(strBPName.begin() + index, strBPName.end());
			else
				continue;

			if (find(m_vecModelTags.begin(), m_vecModelTags.end(), strBPName) == m_vecModelTags.end())
				continue;

			tObjData.iObjType = (_uint)MAPOBJ_STATIC;
			strObjName = strBPName;
		}

		else if (object["Properties"].contains("StaticMesh"))
		{
			tObjData.iObjType = (_uint)MAPOBJ_STATIC;

			strObjName = object["Properties"]["StaticMesh"]["ObjectName"];
			auto index = strObjName.find("StaticMesh");
			if (index != strObjName.npos)
				strObjName.erase(index, strlen("StaticMesh"));
			index = strObjName.find("\'");
			while (index != strObjName.npos)
			{
				strObjName.erase(index, 1);
				index = strObjName.find("\'");
			}

			if (find(m_vecModelTags.begin(), m_vecModelTags.end(), strObjName) == m_vecModelTags.end())
				continue;
		}

		else
			continue;

		tObjData.tLightDesc = {};
		strcpy_s(tObjData.szPrototypeTag, strObjName.c_str());
		_float3 vPosition{ 0.f, 0.f, 0.f }, vRotation{ 0.f, 0.f, 0.f }, vScale{ 1.f, 1.f, 1.f };

		if (object["Properties"].contains("RelativeLocation"))
		{
			auto vLocation = object["Properties"]["RelativeLocation"];
			vPosition.x = vLocation["X"] * +0.01f;
			vPosition.y = vLocation["Z"] * +0.01f;
			vPosition.z = vLocation["Y"] * -0.01f;
		}
		else
			vPosition = { 0.f, 0.f, 0.f };

		if (object["Properties"].contains("RelativeRotation"))
		{
			auto vRot = object["Properties"]["RelativeRotation"];
			vRotation.x = XMConvertToRadians(vRot["Roll"]) * -1.f;
			vRotation.y = XMConvertToRadians(vRot["Yaw"]);
			vRotation.z = XMConvertToRadians(vRot["Pitch"]);
		}
		else
			vRotation = { 0.f, 0.f, 0.f };

		if (object["Properties"].contains("RelativeScale3D"))
		{
			auto vScl = object["Properties"]["RelativeScale3D"];
			vScale.x = vScl["X"] * 0.01f;
			vScale.y = vScl["Z"] * 0.01f;
			vScale.z = vScl["Y"] * 0.01f;
		}
		else
			vScale = { 0.01f, 0.01f, 0.01f };

		_vector vQuatRot = XMVectorZero();
		vQuatRot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&vRotation));
		_matrix MatWorld = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuatRot, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

		XMStoreFloat4x4(&tObjData.matWorld, MatWorld);

		m_vecMapObjData.emplace_back(tObjData);
	}

	Make_Map();

	//ofstream Output;
	//Output.open("../bin/DataFiles/Map/Output.json");
	//
	//for (auto ObjData : m_vecMapObjData)
	//{
	//	json j;
	//	j["ModelName"] = ObjData.szPrototypeTag;
	//
	//	Output << j << endl;
	//}
	//
	//int a = 0;
}

void CImgui_Manager::Pick_Object()
{
	if (Imgui_GetState() == eImgui_State::IMGUI_STATE_WAIT) return;

	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;

	auto MousePos = io.MousePos;

	shared_ptr<CGameObject> pPicked = GAMEINSTANCE->Pick_Object((_uint)MousePos.x, (_uint)MousePos.y);

	if (pPicked != nullptr)
	{
		m_pSelectObject = pPicked;

		_bool bPicked = false;
		_uint iObjectType = (_uint)MAPOBJ_STATIC;

		while (bPicked == false)
		{
			for (_uint i = 0; i < m_vecObjects[iObjectType].size(); ++i)
			{
				if (m_vecObjects[iObjectType][i].get()->Get_ObjectID() == m_pSelectObject.get()->Get_ObjectID())
				{
					m_iCurrCloneIndex = i;
					bPicked = true;
					break;
				}
			}

			++iObjectType;
		}
	}
}

void CImgui_Manager::Clear_Objects()
{
	for (_uint i = 0; i < OBJTYPE_END; ++i) {
		for (_uint j = 0; j < m_vecObjects[i].size(); ++j)
			m_vecObjects[i][j]->Dead();

		m_vecObjects[i].clear();
	}

	for (_uint i = 0; i < OBJTYPE_END; ++i) {
		m_vecCloneNames[i].clear();
	}
}

void CImgui_Manager::Tool_Change()
{
	if (!m_bCanChange)
		return;

	if (GAMEINSTANCE->Get_KeyDown(DIK_F1)) {
		if (m_eToolType == eToolType::TOOL_END) {
			m_eToolType = eToolType::TOOL_MAP;
			return;
		}
		m_eToolType = static_cast<eToolType>(static_cast<int>(m_eToolType) + 1);
	}

	if (GAMEINSTANCE->Get_KeyDown(DIK_1)) m_eToolType = eToolType::TOOL_MAP;
	if (GAMEINSTANCE->Get_KeyDown(DIK_2)) m_eToolType = eToolType::TOOL_EFFECT;
	if (GAMEINSTANCE->Get_KeyDown(DIK_3)) m_eToolType = eToolType::TOOL_SOUND;
	if (GAMEINSTANCE->Get_KeyDown(DIK_4)) m_eToolType = eToolType::TOOL_ANIMATION;
	if (GAMEINSTANCE->Get_KeyDown(DIK_5)) m_eToolType = eToolType::TOOL_FRACTURE;
	if (GAMEINSTANCE->Get_KeyDown(DIK_6)) m_eToolType = eToolType::TOOL_WOUND;
	if (GAMEINSTANCE->Get_KeyDown(DIK_7)) m_eToolType = eToolType::TOOL_BREAKABLE;
	if (GAMEINSTANCE->Get_KeyDown(DIK_8)) m_eToolType = eToolType::TOOL_GEOBRUSH;

}

void CImgui_Manager::Make_Map()
{
	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	CMapObject_Anim::ANIMOBJ_DESC animDesc{};

	_tchar strTextBuffer[MAX_PATH] = {};

	for (_uint i = 0; i < m_vecMapObjData.size(); i++) {
		ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));

		switch (m_vecMapObjData[i].iObjType)
		{
		case LIGHT:
			m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&m_vecMapObjData[i].tLightDesc));
			m_vecObjects[LIGHT].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		case TESTOBJECT:
			m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), TEXT("Prototype_GameObject_TestObject")));
			m_vecObjects[TESTOBJECT].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		case MAPOBJ_STATIC:
			staticDesc.fSpeedPerSec = 0.f;
			staticDesc.fRotationPerSec = 0.f;
			staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			staticDesc.strModelTag = wstring(strTextBuffer);
			staticDesc.bCollision = m_vecMapObjData[i].bCollision;
			m_vecObjects[MAPOBJ_STATIC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Static::ObjID, &staticDesc));
			m_vecObjects[MAPOBJ_STATIC].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			m_vecCloneNames[MAPOBJ_STATIC].emplace_back(m_vecMapObjData[i].szPrototypeTag);
			m_vecCollision.emplace_back(m_vecMapObjData[i].bCollision);
			break;
		case MAPOBJ_ANIM:
			animDesc.fSpeedPerSec = 0.f;
			animDesc.fRotationPerSec = 0.f;
			animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			animDesc.strModelTag = wstring(strTextBuffer);
			m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Anim::ObjID, &animDesc));
			m_vecObjects[MAPOBJ_ANIM].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			m_vecCloneNames[MAPOBJ_ANIM].emplace_back(m_vecMapObjData[i].szPrototypeTag);
			break;

		case MAPOBJ_INTERACTIVE:
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			m_vecObjects[MAPOBJ_INTERACTIVE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), wstring(strTextBuffer), &staticDesc));
			m_vecObjects[MAPOBJ_INTERACTIVE].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			m_vecCloneNames[MAPOBJ_INTERACTIVE].emplace_back(m_vecMapObjData[i].szPrototypeTag);
			break;
		default:
			break;
		}
	}

	m_vecMapObjData.clear();
}

void CImgui_Manager::Save_Map()
{
	for (_uint i = 0; i < OBJTYPE_END; ++i)
	{
		for (_uint j = 0; j < m_vecObjects[i].size(); ++j)
		{
			if (i == MAPOBJ_STATIC) {
				MapObjData tData;
				tData.iObjType = i;
				tData.bCollision = m_vecCollision[j];
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_STATIC][j].c_str());

				m_vecMapObjData.push_back(tData);
			}
			else if (i == MAPOBJ_ANIM) {
				MapObjData tData;
				tData.iObjType = i;
				tData.bCollision = true;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_ANIM][j].c_str());

				m_vecMapObjData.push_back(tData);
			}
			else if (i == MAPOBJ_INTERACTIVE) {
				MapObjData tData;
				tData.iObjType = i;
				tData.bCollision = true;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_INTERACTIVE][j].c_str());

				m_vecMapObjData.push_back(tData);
			}
			else {
				MapObjData tData;
				tData.iObjType = i;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = *(static_pointer_cast<CLight>(m_vecObjects[i][j])->Get_LightDesc());

				m_vecMapObjData.push_back(tData);
			}
		}
	}

	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();
	szBinFilePath += "\\";
	szBinFilePath += m_strMapName;
	szBinFilePath += ".bin";


	std::ofstream os(szBinFilePath, std::ios::binary);

#ifdef _DEBUG

	assert(os.is_open());
#endif // _DEBUG

	for (_uint i = 0; i < m_vecMapObjData.size(); ++i)
	{
		write_typed_data(os, m_vecMapObjData[i]);
	}

	os.close();

	m_vecMapObjData.clear();
}

void CImgui_Manager::Load_Map()
{
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();

	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	
	assert(is.is_open());
#endif // _DEBUG

	while (is.peek() != EOF) {
		MapObjData data;
		read_typed_data(is, data);
		m_vecMapObjData.push_back(data);
	}

	Make_Map();

	//m_vecMapObjData.clear();

	is.close();
}

void CImgui_Manager::Level_MapMake(_uint iMapNum)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	_tchar strTextBuffer[MAX_PATH] = {};

	for (_uint i = 0; i < m_vecMapObjData.size(); i++) {
		switch (m_vecMapObjData[i].iObjType)
		{
		case LIGHT: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = GAMEINSTANCE->Add_Light_Return(&m_vecMapObjData[i].tLightDesc);
			obj->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		case TESTOBJECT: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_TestObject"), &TransformDesc);
			obj->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		case MAPOBJ_STATIC: {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			staticDesc.fSpeedPerSec = 0.f;
			staticDesc.fRotationPerSec = 0.f;
			staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			staticDesc.strModelTag = wstring(strTextBuffer);
			staticDesc.bCollision = m_vecMapObjData[i].bCollision;
			m_vecObjects[MAPOBJ_STATIC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_MapObject"), CMapObject_Static::ObjID, &staticDesc));
			m_vecObjects[MAPOBJ_STATIC].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		case MAPOBJ_ANIM: {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			animDesc.fSpeedPerSec = 0.f;
			animDesc.fRotationPerSec = 0.f;
			animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			animDesc.strModelTag = wstring(strTextBuffer);
			m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_MapObject"), CMapObject_Anim::ObjID, &animDesc));
			m_vecObjects[MAPOBJ_ANIM].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		case MAPOBJ_INTERACTIVE: {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			m_vecObjects[MAPOBJ_INTERACTIVE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_MapObject"), wstring(strTextBuffer), &staticDesc));
			m_vecObjects[MAPOBJ_INTERACTIVE].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		default:
			break;
		}
	}

	m_vecMapObjData.clear();
}

void CImgui_Manager::Level_MapLoad(_uint _iMapNum, const string& strFilePath)
{
	string szBinFilePath;
	_uint iMapNum = _iMapNum;

	//szBinFilePath += "E:\\Github\\Dx11_Framework\\Binary\\Map\\";
	//if (iMapNum == 3) szBinFilePath += "MapData01.bin";
	//if (iMapNum == 5) szBinFilePath += "MapData02.bin";
	//if (iMapNum == 52) {
	//	szBinFilePath += "MapData02_2.bin";
	//	iMapNum = 5;
	//}
	//if (iMapNum == 6) szBinFilePath += "MapData03.bin";

	szBinFilePath = strFilePath;

	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	assert(is.is_open());
#endif // _DEBUG

	while (is.peek() != EOF) {
		MapObjData data;
		read_typed_data(is, data);
		m_vecMapObjData.push_back(data);
	}

	Level_MapMake(iMapNum);

	is.close();

}

void CImgui_Manager::Make_EffectData(UNION_EFFECT_DATA _sUnionData)
{
	m_strUnionEffectKey = _sUnionData.szUnionEffectName;
	

	EffectUnLink();
	for (auto& Effect : m_mEffects)
	{
		Effect.second.clear();
	}

	for (_uint i = 0; i < _sUnionData.iNumEffectDatas; ++i)
	{
		shared_ptr<CEffect_Base> pEffect = static_pointer_cast<CEffect_Base>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), TEXT("Layer_Effect"), wstring().assign(_sUnionData.vEffectDatas[i]->szPrototypeTag.begin(), _sUnionData.vEffectDatas[i]->szPrototypeTag.end())));
		_sUnionData.Make_UnionEffectData(pEffect,i);

		m_mEffects[_sUnionData.vEffectDatas[i]->szPrototypeTag].push_back(pEffect);
		pEffect->EffectEnd();
	}

	EffectLink();



}

void CImgui_Manager::Save_EffectData()
{
	UNION_EFFECT_DATA sUnionData;
	
	_char szUnionEffectName[MAX_PATH];
	strcpy(szUnionEffectName,m_strUnionEffectKey.c_str());
	sUnionData.szUnionEffectName = szUnionEffectName;

	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			shared_ptr<EFFECT_DATA> sEffectData = make_shared<EFFECT_DATA>();
			sEffectData->Make_EffectDatas(pEffect, Effect.first);

			(sUnionData.vEffectDatas).push_back(sEffectData);
		}
	}

	sUnionData.iNumEffectDatas = static_cast<_uint>((sUnionData.vEffectDatas).size());
	
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();
	szBinFilePath += "\\";
	szBinFilePath += sUnionData.szUnionEffectName;
	szBinFilePath += ".bin";

	std::ofstream os(szBinFilePath, std::ios::binary);

#ifdef _DEBUG

	assert(os.is_open());
#endif // _DEBUG

	sUnionData.Bake_Binary(os);
}

void CImgui_Manager::Load_EffectData()
{
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();

	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	//Failed To File Open
	assert(is.is_open());
#endif // _DEBUG

	UNION_EFFECT_DATA sUnionData;
	sUnionData.Load_FromBinary(is);
	
	is.close();

	Make_EffectData(sUnionData);
}

void CImgui_Manager::Init_Effect_Prototype()
{
	auto& Tags = CEffect_Base::GetProtoTag();
	for (auto& Tag : Tags)
	{
		m_vecEffectNames.push_back(string(Tag.begin(), Tag.end()));
	}

	for (auto& Tag : m_vecEffectNames)
	{
		m_mEffects.insert(make_pair(Tag, vector<shared_ptr<CEffect_Base>>()));
	}
}

void CImgui_Manager::Make_Effect()
{
}

void CImgui_Manager::Save_Effect()
{
	Save_EffectData();
}

void CImgui_Manager::Load_Effect()
{
	Load_EffectData();
}

void CImgui_Manager::EffectLink()
{
	_float fTimeMaxCheck = 0.f;
	m_bIsUnionLink = true;
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_EffectLink(true);
			pEffect->Set_EffectLinkTime(&m_fUnionLinkTime);
			pEffect->EffectEnd();
			pEffect->Set_PlayOnce(false);
			if (pEffect->GetEffectTimeMax() > fTimeMaxCheck) {
				fTimeMaxCheck = pEffect->GetEffectTimeMax();
				m_fUnionLinkTimeMax = fTimeMaxCheck;
			}
		}
	}
}

void CImgui_Manager::EffectUnLink()
{
	m_bIsUnionLink = false;
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_EffectLink(false);
			pEffect->Set_EffectLinkTime(nullptr);
			m_fUnionLinkTimeMax = 0.f;
		}
	}
}

void CImgui_Manager::EffectPause()
{
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_Pause(!pEffect->Get_Pause());
		}
	}
}

void CImgui_Manager::UnionStart()
{
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->SetEffectTimeAcc(pEffect->GetEffectTimeMin());
		}
	}
}

void CImgui_Manager::UnionEnd()
{
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->EffectEnd();
			pEffect->Set_PlayOnce(false);
		}
	}
}

void CImgui_Manager::Imgui_LoadFractureTexture()
{
	auto path = fileDialog.GetSelected();
	if (filesystem::is_directory(path))
	{
		for (auto const& entry : filesystem::recursive_directory_iterator(path))
		{
			if (filesystem::is_regular_file(entry) && (entry.path().extension() == ".png" || entry.path().extension() == ".dds"))
			{
				FractureMaterials.push_back({ entry.path().filename().string(), CTexture::Create(m_pDevice, m_pContext, entry.path())});
			}
		}
	}

}

void CImgui_Manager::Imgui_WoundTool()
{
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("Wound Tool");

	if (ImGui::Button("Create Object")) {
		if (!WoundableObject)
		{
			CWoundable::FDO_DESC Desc{};
			Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
			Desc.strModelTag = L"Prototype_Component_Model_Nathan";
			Desc.vScale = { 10.f,10.f,10.f };
			WoundableObject = static_pointer_cast<CWoundable>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"Woundable", CWoundable::ObjID, &Desc));
		}

	}
	if (WoundableObject)
	{
		m_pSelectObject = WoundableObject;
		ImGui::SliderInt("ellp", &ellipsIdx, 0, 1);

		Guizmo(ellipsIdx);
	}

	
	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_LoadFractureTexture();
		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Guizmo(_uint idx)
{
	if (WoundableObject == nullptr)
		return;

	shared_ptr<CTransform> pTerrainTransform = (WoundableObject->Get_WoundTransform(idx));
	_float4x4 WorldMat, ViewMat, ProjMat;
	_float4  vPos;
	static _float3 vRot = { 0.f,0.f,0.f };
	_float3 vScale;

	if (pTerrainTransform == nullptr)
		return;

	WorldMat = pTerrainTransform->Get_WorldMatrix();
	ViewMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	ProjMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_bool flag = false;

	ImGui::Text("");
	ImGui::Text("Transform Data");

	ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
	ImGui::PushItemWidth(200.f);
	flag = ImGui::InputFloat3("Trans", &vPos.x);
	if (flag) {
		pTerrainTransform->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));
	}
	ImGui::PushItemWidth(200.f);

	_float3 vRadianRot = {};

	flag = ImGui::DragFloat3("Rotat", &vRot.x);
	if (flag) {
		vRadianRot.x = XMConvertToRadians(vRot.x);
		vRadianRot.y = XMConvertToRadians(vRot.y);
		vRadianRot.z = XMConvertToRadians(vRot.z);
		pTerrainTransform->Rotation_Quaternion(XMLoadFloat3(&vRadianRot));
	}

	ImGui::PushItemWidth(200.f);

	flag = ImGui::InputFloat3("Scale", &vScale.x);
	if (flag) {
		if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f)
			pTerrainTransform->Set_Scale(vScale);
	}

	ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);

	if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
		, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
		, m_tGizmoDesc.CurrentGizmoMode				// WORLD, LOCAL
		, (_float*)&WorldMat
		, NULL
		, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL
		, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		pTerrainTransform->Set_WorldMatrix(WorldMat);
		if (eToolType::TOOL_EFFECT == m_eToolType)
			static_pointer_cast<CEffect_Base>(m_pSelectObject)->SetRootMatrix(WorldMat);
	}

	if (m_eObjectType == LIGHT) {
		shared_ptr<CLight> pLight = static_pointer_cast<CLight>(m_pSelectObject);

		ImGui::PushItemWidth(200.f);
		ImGui::InputFloat4("Diffuse", pLight->Get_LightDiffusePtr());

		ImGui::InputFloat4("Ambient", pLight->Get_LightAmbientPtr());

		ImGui::InputFloat4("Specular", pLight->Get_LightSpecularPtr());

		ImGui::InputFloat("Range", pLight->Get_LightRangePtr());
	}
}

void CImgui_Manager::Free()
{

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
