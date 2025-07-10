#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include <wrl/client.h>
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"

#include "Model.h"
#include "Object3dCommon.h"

class Object3dCommon;

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};
struct KeyframeVector3 {
	Vector3 value;
	float time;
};
struct KeyframeQuaternion {
	Quaternion value;
	float time;
};
struct NodeAnimation {
	std::vector<KeyframeVector3> translate;
	std::vector<KeyframeQuaternion> rotate;
	std::vector<KeyframeVector3> scale;
};
struct Animation {
	float duration;
	std::map<std::string, NodeAnimation> nodeAnimations;
};

//3Dオブジェクト
class Object3d
{
public:
	//初期化
	void Initialize(Object3dCommon* object3dCommon,Model* model);

	void Update();

	void Draw();

	void CreateMaterialData();

	void CreateTransformationMatrixData();

	void CreateDirectionLightData();

	void SetModel(Model* model) { this->model = model; }

	void SetModel(const std::string& filePath);

	void SetCamera(Camera* camera) { camera_ = camera; }

	EulerTransform& GetTransform() { return transform_; }

	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	//Vector3用
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	//Quaternion用
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

private:
	Object3dCommon* object3dCommon_ = nullptr;

	Model* model = nullptr;

	Camera* camera_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr < ID3D12Resource> transformationMatrixResource = nullptr;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr < ID3D12Resource> shaderResource = nullptr;
	//バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightData = nullptr;

	EulerTransform transform_;

	float animationTime_ = 0.0f;

	Animation animation = LoadAnimationFile("./resources", "walk.gltf");
};

