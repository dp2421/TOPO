#pragma once
#define _XM_NO_INTRINSICS_

#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <d2d1_3.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <d3d11on12.h>
#include <d2d1_1helper.h>

#include <d2d1effects.h>
#include <wincodec.h>

#include <algorithm>
#include "SimpleMath.h"
#include "../../../../../Protocol.h"

#pragma comment(lib,"d3d12")
#pragma comment(lib,"dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "WS2_32.LIB")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <string>
#include <vector>
#include <list>
#include <map>

#include <fbxsdk.h>
#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

using std::vector;
using std::list;
using std::map;
using std::string;
using std::wstring;
using std::make_pair;

#include "d3dx12.h"
#include "define.h"
#include "struct.h"
#include "func.h"
#include "extern.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"