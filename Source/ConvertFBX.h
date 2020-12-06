#pragma once

#include "vector.h"
#include <fbxsdk.h> 

// FbxDouble2 -> VECTOR2
inline VECTOR2 FbxDouble2ToFloat2(const FbxDouble2& fbx_value)
{
	return VECTOR2(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]));
}

// FbxDouble3 -> VECTOR3
inline VECTOR3 FbxDouble3ToFloat3(const FbxDouble3& fbx_value)
{
	return VECTOR3(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]));
}

// FbxDouble4 -> VECTOR3
inline VECTOR3 FbxDouble4ToFloat3(const FbxDouble4& fbx_value)
{
	return VECTOR3(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]));
}

// FbxDouble4 -> VECTOR4
inline VECTOR4 FbxDouble4ToFloat4(const FbxDouble4& fbx_value)
{
	return VECTOR4(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]),
		static_cast<float>(fbx_value[3]));
}

// FbxDouble4 -> FLOAT4X4
inline FLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbx_value)
{
	return FLOAT4X4(
		static_cast<float>(fbx_value[0][0]),
		static_cast<float>(fbx_value[0][1]),
		static_cast<float>(fbx_value[0][2]),
		static_cast<float>(fbx_value[0][3]),
		static_cast<float>(fbx_value[1][0]),
		static_cast<float>(fbx_value[1][1]),
		static_cast<float>(fbx_value[1][2]),
		static_cast<float>(fbx_value[1][3]),
		static_cast<float>(fbx_value[2][0]),
		static_cast<float>(fbx_value[2][1]),
		static_cast<float>(fbx_value[2][2]),
		static_cast<float>(fbx_value[2][3]),
		static_cast<float>(fbx_value[3][0]),
		static_cast<float>(fbx_value[3][1]),
		static_cast<float>(fbx_value[3][2]),
		static_cast<float>(fbx_value[3][3])
	);
}