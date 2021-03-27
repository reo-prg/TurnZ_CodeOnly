#pragma once
#include <memory>
#include <vector>
#include <d3dx12.h>

#define DrawerIns Drawer::GetInstance()

using namespace Microsoft::WRL;
enum class ChangeState
{
	Ready,
	Starting,
	Playing,
	Ending,
};

class DrawCharacter;
class ImageManager;
class Object2DDrawer;
class Object2D;
class Object2DContainer;
class Object3DDrawer;
class Object3D;
class Square2D;
class TransformCBV;
class Drawer
{
public:
	static Drawer& GetInstance(void);

	bool Initialize(unsigned int width, unsigned int height);
	void Update(void);
	void Draw(void);

	const std::unique_ptr<ImageManager>& GetImageManager(void);
	const std::unique_ptr<Object3DDrawer>& GetObjectDrawer(void);
	const std::unique_ptr<Object2DDrawer>& GetObject2DDrawer(void);
	[[nodiscard]]const std::unique_ptr<DrawCharacter>& GetCharacterDrawer(void);

	void RegisterObject(const std::shared_ptr<Object2D>& object);
	void RegisterObject(const std::shared_ptr<Object3D>& object);
	void UnregisterObject(const std::shared_ptr<Object2D>& object);
	void UnregisterObject(const std::shared_ptr<Object3D>& object);

	void changeEffect(void);
	ChangeState GetChangeState(void);
	void SetViewProjToEffekseer(void);
private:
	Drawer();
	~Drawer();
	Drawer(const Drawer&) = delete;
	Drawer operator=(const Drawer&) = delete;

	void ChStart(void);
	void ChIdle(void);
	void ChEnd(void);
	void(Drawer::*changeUpdater_)(void);

	// RenderTargets
	std::vector<ComPtr<ID3D12Resource>> baseBuffers_;
	ComPtr<ID3D12DescriptorHeap> baseHeap_;

	std::unique_ptr<ImageManager> imagemng_;
	std::unique_ptr<Object2DDrawer> object2D_;
	std::unique_ptr<Object3DDrawer> object3D_;
	std::unique_ptr<TransformCBV> wvp_;
	std::unique_ptr<DrawCharacter> chr_;

	void CreateBasicObjects(unsigned int width, unsigned int height);
	bool CreateSwapBuffer(void);
	void ExecuteCommand(void);

	void DrawChangeEffect_(void);

	float changeCoordX_;
	static constexpr float speed_ = 32.0f;
	static constexpr float plSize_ = 128.0f;
	static constexpr float scrdiff_ = 112.0f;
	float scrWidth_;
	float plScale_;
	ChangeState state_;
	bool change_;

	Square2D* overwrap_;
	Square2D* player_;
	bool drawChangeEfk_;
};

