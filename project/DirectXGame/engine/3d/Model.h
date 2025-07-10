#pragma once
#include <map>

#include "ModelCommon.h"
#include "TextureManager.h"
#include "Calculation.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};
struct Material {
	Vector4 color;
	bool enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};
struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
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


class Model
{
public:
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename);

	void Update(Skeleton& skeleton);

	void Draw();

	//.mtlファイルの読み込み
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み込み
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	ModelData* GetModelData() {return &modelData_;}

	Skeleton* GetSkeleton() { return &skeleton_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView; }

	static Node ReadNode(aiNode* node);

	Skeleton CreateSkeleton(const Node& rootNode);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

	//Vector3用
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	//Quaternion用
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

private:
	//ModelCommonのポインタ
	ModelCommon* modelCommon_;
	//Objファイルのデータ
	ModelData modelData_;
	Skeleton skeleton_;
	//VertexResource
	Microsoft::WRL::ComPtr< ID3D12Resource> vertexResource = nullptr;
	//VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	//VertexResourceにデータを書き込むためのポインタ
	VertexData* vertexData = nullptr;
	//マテリアルリソース
	Microsoft::WRL::ComPtr < ID3D12Resource> materialResource = nullptr;
	//マテリアルリソースにデータを書き込むためのポインタ
	Material* materialData = nullptr;

	uint32_t instanceCount = 10;

	ModelData model = LoadModelFile("./resources/", "AnimatedCube.gltf");

};

