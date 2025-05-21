#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4828)
#pragma warning (disable : 4251)
#pragma warning (disable : 4244)

#define _CRT_SECURE_NO_WARNINGS

#include <d3d11.h>

#define  DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#pragma region Imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"
#include <imfilebrowser.h>
#pragma	endregion

#pragma region assimp
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#pragma	endregion

#pragma region json
#include <json\json.hpp>
using json = nlohmann::json;
#pragma endregion

#include <Fx11\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

#include <directxtk\VertexTypes.h>
#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\Effects.h>


#include <DirectXTK\ScreenGrab.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace std;
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
#include <sstream>
#include <fstream>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <random>

#include "PxPhysicsApi.h"
#include "extensions/PxParticleClothCooker.h"
#include "cudamanager/PxCudaContext.h"
#include "PxConstraint.h"
using namespace physx;


#include "Engine_Macro.h"
#include "Engine_Enum.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "dxgidebug.h"
#include "DebugDraw.h"

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 

#endif // _DEBUG

#else // _DEBUG
#define DBG_NEW new

#endif // _DEBUG

#include "BoneData.h"
#include "MeshData.h"
#include "ModelData.h"

using namespace Engine;

