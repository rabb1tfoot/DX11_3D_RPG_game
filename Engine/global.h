#pragma once

#define _XM_NO_INTRIINSICS_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace DirectX::TriangleTests;

#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "winmm.lib")

//FBX Loader
#include <fbxsdk.h>

#ifdef _DEBUG

#ifdef _WIN32
#pragma comment(lib, "FBXLoader/x86/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader\x64\debug\libfbxsdk-md.lib")
#endif

#else
#ifdef _WIN32
#pragma comment(lib, "FBXLoader\x86\release\libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader\x64\release\libfbxsdk-md.lib")
#endif

#endif

#include <vector>
#include <list>
#include <map>

using namespace std;

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

#include "define.h"
#include "struct.h"
#include "func.h"

extern tTransform g_transform;
extern tGlobalValue g_global;