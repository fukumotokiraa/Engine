#pragma once
#include <random>
#include <list>
#include <unordered_map>
#include <numbers>

#include <d3d12.h>
#include <wrl.h>
#include "Calculation.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "TextureManager.h"

struct Particle {
	EulerTransform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 world;
	Vector4 color;
};
struct ParticleGroup {
	// マテリアルデータ(テクスチャファイルパスとテクスチャ用SRVインデックス)
	MaterialData materialData;

	// パーティクルのリスト(std::list<Particle>型)
	std::list<Particle> particles;

	// インスタンシングデータ用SRVインデックス
	uint32_t instanceSrvIndex;

	// インスタンシングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceResource;

	// インスタンス数
	uint32_t instanceCount;

	// インスタンシングデータを書き込むためのポインタ
	ParticleForGPU* instanceData;
};
struct AccelerationField {
	Vector3 acceleration;//!<加速度
	AABB area;//!<範囲
};

class DirectXCommon;
class SrvManager;
class object3dCommon;

class ParticleManager
{
private:
	static ParticleManager* instance;
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();

	bool isCollision(const AABB& aabb, const Vector3& point) {
		// 各軸において点がAABBの範囲内にあるかを確認
		if (point.x < aabb.min.x || point.x > aabb.max.x) return false;
		if (point.y < aabb.min.y || point.y > aabb.max.y) return false;
		if (point.z < aabb.min.z || point.z > aabb.max.z) return false;

		// すべての軸で範囲内にある場合、衝突している
		return true;
	}

public:
	static ParticleManager* GetInstance();

	void Finalize();

	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager,Object3dCommon*object3dCommon);

	void Update();

	void Draw();

	void CreateParticleGroup(const std::string& name, const std::string& textureFilePath, const std::string& modelFilePath);

	void Emit(const std::string& name, const Vector3& position, uint32_t count);

	Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);

	bool GetUseBillBoard() { return useBillBoard; }

	void SetUseBillBoard(bool useBillBoard) { this->useBillBoard = useBillBoard; }

	bool GetApplyField() { return applyField; }

	void SetApplyField(bool applyField) { this->applyField = applyField; }

private:
	DirectXCommon* dxCommon_;
	SrvManager* srvManager_;
	Object3dCommon* object3dCommon_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;

	const float kDeltaTime_ = 1.0f / 60.0f;

	std::unordered_map < std::string, ParticleGroup > particleGroups_;

	D3D12_VERTEX_BUFFER_VIEW* vertexBufferView;

	EulerTransform cameraTransform_;

	std::mt19937 randomEngine_;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};

	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob;

	D3D12_BLEND_DESC blendDesc{};

	D3D12_RASTERIZER_DESC rasterizerDesc{};

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	Microsoft::WRL::ComPtr < ID3D12Resource> materialResource_ = nullptr;
	Material* materialData_ = nullptr;

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	ParticleForGPU particleForGPU;

	bool isFirstInstancingData = false;

	bool useBillBoard = true;

	AccelerationField accelerationField;

	bool applyField = false;

};

