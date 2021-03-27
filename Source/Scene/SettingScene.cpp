#include "SettingScene.h"
#include <DirectXMath.h>
#include "../Audio/AudioManager.h"
#include "../Actor/Note.h"
#include "../Graphic/Drawer.h"
#include "../Graphic/DrawCharacter.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Object/2D/Square2D.h"
#include "../Input/InputIntegrator.h"
#include "../Score/ScoreLoader.h"
#include "SceneManager.h"
#include "SelectScene.h"
#include "../Application.h"

float SettingScene::musicVolume_;
float SettingScene::soundVolume_;
unsigned char SettingScene::noteType_;
unsigned int SettingScene::key_[8];
short SettingScene::judge_;

using namespace DirectX;
namespace
{
	constexpr XMFLOAT2 mVolumeBarCoord = { 640.0f, 160.0f };
	constexpr XMFLOAT2 mVolumeBarSize = { 450.0f, 75.0f };

	constexpr XMFLOAT2 eVolumeBarCoord = { 640.0f, 280.0f };
	constexpr XMFLOAT2 eVolumeBarSize = { 450.0f, 75.0f };

	constexpr XMFLOAT2 volumeFrameSize = { 550.0f, 100.0f };

	constexpr XMFLOAT2 volumeFrameCoord = { 550.0f, 190.0f };
	constexpr XMFLOAT2 volumeOverwrapSize = { 760.0f, 300.0f };

	constexpr XMFLOAT2 barSize = { 15.0f, 75.0f };

	constexpr XMFLOAT2 noteCoord = { 465.0f, 420.0f };
	constexpr float noteSize = 64.0f;
	constexpr float noteDur = 90.0f;

	constexpr XMFLOAT3 noteFrameColorNormal = { 0.4f, 0.4f, 0.4f };
	constexpr XMFLOAT3 noteFrameColorActive = { 1.0f, 0.75f, 0.25f };
	constexpr float noteFrameSize = 80.0f;
	constexpr XMFLOAT2 noteBigFrameCoord = { 640.0f, noteCoord.y };
	constexpr XMFLOAT2 noteBigFrameSize = { 450.0f, 100.0f };

	constexpr XMFLOAT2 judgeFrameCoord = { 640.0f, 530.0f };
	constexpr XMFLOAT2 judgeFrameSize = { 250.0f, 100.0f };
	constexpr XMFLOAT2 judgeNumFrameSize = { 100.0f, 80.0f };
	constexpr XMFLOAT2 judgeStrFrameSize = { 595.0f, judgeFrameSize.y };
	constexpr XMFLOAT2 judgeStrFrameCoord = { judgeFrameCoord.x + judgeFrameSize.x / 2.0f - judgeStrFrameSize.x / 2.0f, judgeFrameCoord.y };

	constexpr XMFLOAT3 frameColorNormal = { 0.8f, 0.8f, 0.8f };
	constexpr XMFLOAT3 frameColorActive = { 0.2f, 0.8f, 0.3f };

	constexpr XMFLOAT2 resetFrameCoord = { 490.0f, 640.0f };
	constexpr XMFLOAT2 resetFrameSize = { 200.0f, 80.0f };

	constexpr XMFLOAT2 backFrameCoord = { 790.0f, 640.0f };
	constexpr XMFLOAT2 backFrameSize = { 200.0f, 80.0f };

	constexpr XMFLOAT2 shadowDiff = { -4.0f, 4.0f };

	constexpr float noteStrFrameDiff = -245.0f;

	XMFLOAT2 operator+(const XMFLOAT2& val1, const XMFLOAT2& val2)
	{
		return XMFLOAT2(val1.x + val2.x, val1.y + val2.y);
	}
}

SettingScene::SettingScene(int cursor, unsigned int dif)
{
	obj_.reset(new Object2DContainer());

	unsigned int x, y;
	Application::GetInstance().GetWindowSize(x, y);
	scrWidth_ = static_cast<float>(x);
	scrHeight_ = static_cast<float>(y);

	selectCursor_ = cursor;
	selectDifficulty_ = dif;
	cursor_ = 0;
	Initialize();
	LoadImages();

	SetJudgeStr();
	updater_ = &SettingScene::WaitEffect;
	AudioIns.LoadWAVFile("Resource/Sound/Tutorial.wav", "test");
	AudioIns.LoadWAVFile("Resource/Sound/clap.wav", "clap");
	AudioIns.LoadWAVFile("Resource/Sound/correct.wav", "correct");
	AudioIns.PlayWAVFileLoop("test", 0.0f, 0.0f, XAUDIO2_LOOP_INFINITE, VoiceType::Music);
	DrawerIns.changeEffect();
}

SettingScene::~SettingScene()
{
}

void SettingScene::Update(void)
{
	DrawStr();
	(this->*updater_)();
}

void SettingScene::LoadAndApplySetting(void)
{
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, "Resource/data/settings.dat", "rb");

	// ファイルが無い時
	if (err != 0)
	{
		musicVolume_ = 0.5f;
		soundVolume_ = 0.5f;
		noteType_ = 0;
		key_[0] = 0x57;
		key_[1] = 0x44;
		key_[2] = 0x53;
		key_[3] = 0x41;
		key_[4] = VK_UP;
		key_[5] = VK_RIGHT;
		key_[6] = VK_DOWN;
		key_[7] = VK_LEFT;
		judge_ = 0;
	}
	else
	{
		fread_s(&musicVolume_, sizeof(musicVolume_), sizeof(musicVolume_), 1, fp);
		fread_s(&soundVolume_, sizeof(soundVolume_), sizeof(soundVolume_), 1, fp);
		fread_s(&noteType_, sizeof(noteType_), sizeof(noteType_), 1, fp);
		fread_s(key_, sizeof(key_[0]) * _countof(key_), sizeof(key_[0]), _countof(key_), fp);
		fread_s(&judge_, sizeof(judge_), sizeof(judge_), 1, fp);
		fclose(fp);
	}
	// 適用
	AudioIns.SetMVolume(musicVolume_);
	AudioIns.SetEVolume(soundVolume_);
	Note::SetType(noteType_);
	ScoreLoader::judgeDiff_ = judge_;
}

void SettingScene::WaitEffect(void)
{
	if (DrawerIns.GetChangeState() == ChangeState::Ready)
	{
		updater_ = &SettingScene::Setting;
	}
}

void SettingScene::Setting(void)
{
	functions_[cursor_]();
}

void SettingScene::BackToSelect(void)
{
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		mng_->ChangeScene(new SelectScene(selectCursor_, selectDifficulty_));
	}
}

void SettingScene::Reset(void)
{
	musicVolume_ = 0.5f;
	soundVolume_ = 0.5f;
	noteType_ = 0;
	key_[0] = 0x57;
	key_[1] = 0x44;
	key_[2] = 0x53;
	key_[3] = 0x41;
	key_[4] = VK_UP;
	key_[5] = VK_RIGHT;
	key_[6] = VK_DOWN;
	key_[7] = VK_LEFT;

	// バーの位置リセット
	obj_->Get("mbar")->SetCoordinate(mVolumeBarCoord.x - mVolumeBarSize.x / 2.0f + mVolumeBarSize.x * musicVolume_,
		mVolumeBarCoord.y);
	obj_->Get("ebar")->SetCoordinate(eVolumeBarCoord.x - eVolumeBarSize.x / 2.0f + eVolumeBarSize.x * soundVolume_,
		eVolumeBarCoord.y);

	for (int i = 0; i < _countof(notePath); i++)
	{
		if (i == static_cast<int>(noteType_))
		{
			obj_->Get("noteF" + std::to_string(i))->SetColor(noteFrameColorActive);
		}
		else
		{
			obj_->Get("noteF" + std::to_string(i))->SetColor(noteFrameColorNormal);
		}
	}

	// 適用
	AudioIns.SetMVolume(musicVolume_);
	AudioIns.SetEVolume(soundVolume_);
	Note::SetType(noteType_);
}

void SettingScene::LoadImages(void)
{
	Image2D* func;

	Image2D* volf = new Image2D();
	volf->CreateObject("Resource/Image/setting_frame.png",
		volumeFrameCoord.x, volumeFrameCoord.y, volumeOverwrapSize.x, volumeOverwrapSize.y);
	volf->zBuffer_ = 200;
	volf->SetColor({ 0.3f, 0.3f, 0.3f });
	obj_->Add(volf, "volf");
	DrawerIns.RegisterObject(obj_->Get("volf"));

	volf = new Image2D();
	volf->CreateObject("Resource/Image/setting_frame.png",
		volumeFrameCoord.x + shadowDiff.x, volumeFrameCoord.y + shadowDiff.y, volumeOverwrapSize.x, volumeOverwrapSize.y);
	volf->zBuffer_ = 210;
	volf->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(volf, "volf_s");
	DrawerIns.RegisterObject(obj_->Get("volf_s"));

	Image2D* mvolume = new Image2D();
	mvolume->CreateObject("Resource/Image/volumeBar.png",
		mVolumeBarCoord.x, mVolumeBarCoord.y, mVolumeBarSize.x, mVolumeBarSize.y);
	mvolume->zBuffer_ = 50;
	obj_->Add(mvolume, "mvolume");
	DrawerIns.RegisterObject(obj_->Get("mvolume"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		mVolumeBarCoord.x, mVolumeBarCoord.y, volumeFrameSize.x, volumeFrameSize.y);
	func->zBuffer_ = 100;
	func->SetColor(frameColorActive);
	obj_->Add(func, "func0");
	DrawerIns.RegisterObject(obj_->Get("func0"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		mVolumeBarCoord.x + shadowDiff.x, mVolumeBarCoord.y + shadowDiff.y, volumeFrameSize.x, volumeFrameSize.y);
	func->zBuffer_ = 110;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "func0_s");
	DrawerIns.RegisterObject(obj_->Get("func0_s"));

	Image2D* evolume = new Image2D();
	evolume->CreateObject("Resource/Image/volumeBar.png",
		eVolumeBarCoord.x, eVolumeBarCoord.y, eVolumeBarSize.x, eVolumeBarSize.y);
	evolume->zBuffer_ = 50;
	obj_->Add(evolume, "evolume");
	DrawerIns.RegisterObject(obj_->Get("evolume"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		eVolumeBarCoord.x, eVolumeBarCoord.y, volumeFrameSize.x, volumeFrameSize.y);
	func->zBuffer_ = 100;
	func->SetColor(frameColorNormal);
	obj_->Add(func, "func1");
	DrawerIns.RegisterObject(obj_->Get("func1"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		eVolumeBarCoord.x + shadowDiff.x, eVolumeBarCoord.y + shadowDiff.y, volumeFrameSize.x, volumeFrameSize.y);
	func->zBuffer_ = 110;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "func1_s");
	DrawerIns.RegisterObject(obj_->Get("func1_s"));

	Image2D* mbar = new Image2D();
	mbar->CreateObject("Resource/Image/Bar.png",
		mVolumeBarCoord.x - mVolumeBarSize.x / 2.0f + mVolumeBarSize.x * musicVolume_, mVolumeBarCoord.y, barSize.x, barSize.y);
	mbar->zBuffer_ = 30;
	obj_->Add(mbar, "mbar");
	DrawerIns.RegisterObject(obj_->Get("mbar"));

	Image2D* ebar = new Image2D();
	ebar->CreateObject("Resource/Image/Bar.png",
		eVolumeBarCoord.x - eVolumeBarSize.x / 2.0f + eVolumeBarSize.x * soundVolume_, eVolumeBarCoord.y, barSize.x, barSize.y);
	ebar->zBuffer_ = 30;
	obj_->Add(ebar, "ebar");
	DrawerIns.RegisterObject(obj_->Get("ebar"));

	Image2D* back = new Image2D();
	back->CreateObject(scrWidth_ / 2.0f, scrHeight_ / 2.0f, scrWidth_, scrHeight_, { 0.2f, 0.2f, 0.4f, 1.0f });
	back->zBuffer_ = 10000;
	obj_->Add(back, "back");
	DrawerIns.RegisterObject(obj_->Get("back"));


	Image2D* note;
	for (int i = 0; i < _countof(notePath); i++)
	{
		note = new Image2D();
		note->CreateObject("Resource/Image/" + notePath[i], noteCoord.x + noteDur * static_cast<float>(i), 
			noteCoord.y, noteSize, noteSize);
		note->zBuffer_ = 40;
		obj_->Add(note, "note" + std::to_string(i));
		DrawerIns.RegisterObject(obj_->Get("note" + std::to_string(i)));

		note = new Image2D();
		note->CreateObject("Resource/Image/noteFrame.png", noteCoord.x + noteDur * static_cast<float>(i), 
			noteCoord.y, noteFrameSize, noteFrameSize);
		note->zBuffer_ = 50;
		obj_->Add(note, "noteF" + std::to_string(i));
		if (i == static_cast<int>(noteType_))
		{
			note->SetColor(noteFrameColorActive);
		}
		else
		{
			note->SetColor(noteFrameColorNormal);
		}
		DrawerIns.RegisterObject(obj_->Get("noteF" + std::to_string(i)));
	}

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		noteBigFrameCoord.x, noteBigFrameCoord.y, noteBigFrameSize.x, noteBigFrameSize.y);
	func->zBuffer_ = 100;
	func->SetColor(frameColorNormal);
	obj_->Add(func, "func2");
	DrawerIns.RegisterObject(obj_->Get("func2"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		noteBigFrameCoord.x + shadowDiff.x, noteBigFrameCoord.y + shadowDiff.y, noteBigFrameSize.x, noteBigFrameSize.y);
	func->zBuffer_ = 110;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "func2_s");
	DrawerIns.RegisterObject(obj_->Get("func2_s"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		noteBigFrameCoord.x + noteStrFrameDiff, noteBigFrameCoord.y, noteBigFrameSize.x, noteBigFrameSize.y);
	func->zBuffer_ = 105;
	func->SetColor({ 0.3f, 0.3f, 0.3f });
	obj_->Add(func, "notestr");
	DrawerIns.RegisterObject(obj_->Get("notestr"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		noteBigFrameCoord.x + noteStrFrameDiff + shadowDiff.x, noteBigFrameCoord.y + shadowDiff.y, noteBigFrameSize.x, noteBigFrameSize.y);
	func->zBuffer_ = 115;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "notestr_s");
	DrawerIns.RegisterObject(obj_->Get("notestr_s"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		judgeFrameCoord.x, judgeFrameCoord.y, judgeFrameSize.x, judgeFrameSize.y);
	func->zBuffer_ = 100;
	func->SetColor(frameColorNormal);
	obj_->Add(func, "func3");
	DrawerIns.RegisterObject(obj_->Get("func3"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		judgeFrameCoord.x + shadowDiff.x, judgeFrameCoord.y + shadowDiff.y, judgeFrameSize.x, judgeFrameSize.y);
	func->zBuffer_ = 110;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "func3_s");
	DrawerIns.RegisterObject(obj_->Get("func3_s"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		judgeStrFrameCoord.x, judgeStrFrameCoord.y, judgeStrFrameSize.x, judgeStrFrameSize.y);
	func->zBuffer_ = 105;
	func->SetColor({ 0.3f, 0.3f, 0.3f });
	obj_->Add(func, "judgestr");
	DrawerIns.RegisterObject(obj_->Get("judgestr"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		judgeStrFrameCoord.x + shadowDiff.x, judgeStrFrameCoord.y + shadowDiff.y, judgeStrFrameSize.x, judgeStrFrameSize.y);
	func->zBuffer_ = 115;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "judgestr_s");
	DrawerIns.RegisterObject(obj_->Get("judgestr_s"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		judgeFrameCoord.x, judgeFrameCoord.y, judgeNumFrameSize.x, judgeNumFrameSize.y);
	func->zBuffer_ = 90;
	func->SetColor({ 0.1f, 0.3f, 0.6f });
	obj_->Add(func, "judgenum");
	DrawerIns.RegisterObject(obj_->Get("judgenum"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		resetFrameCoord.x, resetFrameCoord.y, resetFrameSize.x, resetFrameSize.y);
	func->zBuffer_ = 100;
	func->SetColor(frameColorNormal);
	obj_->Add(func, "func4");
	DrawerIns.RegisterObject(obj_->Get("func4"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		resetFrameCoord.x + shadowDiff.x, resetFrameCoord.y + shadowDiff.y, resetFrameSize.x, resetFrameSize.y);
	func->zBuffer_ = 110;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "func4_s");
	DrawerIns.RegisterObject(obj_->Get("func4_s"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		backFrameCoord.x, backFrameCoord.y, backFrameSize.x, backFrameSize.y);
	func->zBuffer_ = 100;
	func->SetColor(frameColorNormal);
	obj_->Add(func, "func5");
	DrawerIns.RegisterObject(obj_->Get("func5"));

	func = new Image2D();
	func->CreateObject("Resource/Image/setting_frame.png",
		backFrameCoord.x + shadowDiff.x, backFrameCoord.y + shadowDiff.y, backFrameSize.x, backFrameSize.y);
	func->zBuffer_ = 110;
	func->SetColor({ 0.0f, 0.0f, 0.0f });
	obj_->Add(func, "func5_s");
	DrawerIns.RegisterObject(obj_->Get("func5_s"));
}

void SettingScene::DrawStr(void)
{
	DrawerIns.GetCharacterDrawer()->DrawString(L"音量", "setting_s", { 190.0f, 60.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"楽曲", "setting_s", { 230.0f, mVolumeBarCoord.y - 30.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"効果音", "setting_s", { 230.0f, eVolumeBarCoord.y - 30.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"ノーツの種類", "setting_s", { noteBigFrameCoord.x - noteBigFrameSize.x / 2.0f + noteStrFrameDiff + 20.0f, noteBigFrameCoord.y - 20.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"リセット", "setting_s", { resetFrameCoord.x - 50.0f, resetFrameCoord.y - 20.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"戻る", "setting_s", { backFrameCoord.x - 30.0f, backFrameCoord.y - 20.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(judgeStr_, "main_b", { judgeFrameCoord.x - 50.0f, judgeFrameCoord.y - 25.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"-", "main_b", { judgeFrameCoord.x - 85.0f, judgeFrameCoord.y - 25.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"+", "main_b", { judgeFrameCoord.x + 50.0f, judgeFrameCoord.y - 25.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(L"判定調整", "setting_s", { judgeStrFrameCoord.x - judgeStrFrameSize.x / 2.0f + 20.0f, judgeFrameCoord.y - 25.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SettingScene::Apply(void)
{
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, "Resource/data/settings.dat", "wb");
	if (err != 0) { return; }

	fwrite(&musicVolume_, sizeof(musicVolume_), 1, fp);
	fwrite(&soundVolume_, sizeof(soundVolume_), 1, fp);
	fwrite(&noteType_, sizeof(noteType_), 1, fp);
	fwrite(key_, sizeof(key_[0]), _countof(key_), fp);
	fwrite(&judge_, sizeof(judge_), 1, fp);

	fclose(fp);
}

void SettingScene::Initialize(void)
{
	// 楽曲音量調整
	functions_.push_back([this] {
		if (InputIns.Triggered("down") && cursor_ < functions_.size() - 1)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ + 1))->SetColor(frameColorActive);
			cursor_++;
			return;
		}
		if (InputIns.Pressed("left"))
		{
			musicVolume_ = std::clamp(musicVolume_ - 0.01f, 0.0f, 1.0f);
			obj_->Get("mbar")->SetCoordinate(mVolumeBarCoord.x - mVolumeBarSize.x / 2.0f + mVolumeBarSize.x * musicVolume_,
				mVolumeBarCoord.y);
			AudioIns.SetMVolume(musicVolume_);
			
		}
		if (InputIns.Pressed("right"))
		{
			musicVolume_ = std::clamp(musicVolume_ + 0.01f, 0.0f, 1.0f);
			obj_->Get("mbar")->SetCoordinate(mVolumeBarCoord.x - mVolumeBarSize.x / 2.0f + mVolumeBarSize.x * musicVolume_,
				mVolumeBarCoord.y);
			AudioIns.SetMVolume(musicVolume_);
		}
		});

	// 効果音音量調整
	functions_.push_back([this] {
		if (InputIns.Triggered("up") && cursor_ > 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ - 1))->SetColor(frameColorActive);
			cursor_--;
			return;
		}
		if (InputIns.Triggered("down") && cursor_ < functions_.size() - 1)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ + 1))->SetColor(frameColorActive);
			cursor_++;
			return;
		}
		if (InputIns.Pressed("left"))
		{
			soundVolume_ = std::clamp(soundVolume_ - 0.01f, 0.0f, 1.0f);
			obj_->Get("ebar")->SetCoordinate(eVolumeBarCoord.x - eVolumeBarSize.x / 2.0f + eVolumeBarSize.x * soundVolume_,
				eVolumeBarCoord.y);
			AudioIns.SetEVolume(soundVolume_);
		}
		if (InputIns.Pressed("right"))
		{
			soundVolume_ = std::clamp(soundVolume_ + 0.01f, 0.0f, 1.0f);
			obj_->Get("ebar")->SetCoordinate(eVolumeBarCoord.x - eVolumeBarSize.x / 2.0f + eVolumeBarSize.x * soundVolume_,
				eVolumeBarCoord.y);
			AudioIns.SetEVolume(soundVolume_);
		}
		if (InputIns.Released("left") || InputIns.Released("right") || InputIns.Released("up") || InputIns.Released("down"))
		{
			AudioIns.PlayWAVFile("clap", VoiceType::Effect);
		}
		});

	// ノーツ選択
	functions_.push_back([this] {
		if (InputIns.Triggered("up") && cursor_ > 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ - 1))->SetColor(frameColorActive);
			cursor_--;
			return;
		}
		if (InputIns.Triggered("down") && cursor_ < functions_.size() - 1)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ + 1))->SetColor(frameColorActive);
			cursor_++;
			return;
		}
		if (InputIns.Triggered("left") && noteType_ > 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("noteF" + std::to_string(static_cast<int>(noteType_)))->SetColor(noteFrameColorNormal);
			obj_->Get("noteF" + std::to_string(static_cast<int>(noteType_ - 1)))->SetColor(noteFrameColorActive);
			noteType_--;
			Note::SetType(noteType_);
		}
		if (InputIns.Triggered("right") && noteType_ < Note::typeCount_ - 1)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("noteF" + std::to_string(static_cast<int>(noteType_)))->SetColor(noteFrameColorNormal);
			obj_->Get("noteF" + std::to_string(static_cast<int>(noteType_ + 1)))->SetColor(noteFrameColorActive);
			noteType_++;
			Note::SetType(noteType_);
		}
		});

	// 判定調整
	functions_.push_back([this] {
		if (InputIns.Triggered("up") && cursor_ > 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ - 1))->SetColor(frameColorActive);
			cursor_--;
			return;
		}
		if (InputIns.Triggered("down") && cursor_ < functions_.size() - 1)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ + 1))->SetColor(frameColorActive);
			cursor_++;
			return;
		}
		if (InputIns.Triggered("left"))
		{
			judge_--;
			if (judge_ < -99) { judge_ = -99; }
			SetJudgeStr();
			return;
		}
		if (InputIns.Triggered("right"))
		{
			judge_++;
			if (judge_ > 99) { judge_ = 99; }
			SetJudgeStr();
			return;
		}
		});

	// リセット
	functions_.push_back([this] {
		if (InputIns.Triggered("up") && cursor_ > 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ - 1))->SetColor(frameColorActive);
			cursor_--;
			return;
		}
		if (InputIns.Triggered("right") && cursor_ < functions_.size() - 1)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ + 1))->SetColor(frameColorActive);
			cursor_++;
			return;
		}
		if (InputIns.Triggered("ok"))
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			Reset();
		}
		});

	// 戻る
	functions_.push_back([this]{
		if (InputIns.Triggered("up") && cursor_ - 2 >= 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ - 2))->SetColor(frameColorActive);
			cursor_-=2;
			return;
		}
		if (InputIns.Triggered("left") && cursor_ > 0)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("func" + std::to_string(cursor_))->SetColor(frameColorNormal);
			obj_->Get("func" + std::to_string(cursor_ - 1))->SetColor(frameColorActive);
			cursor_--;
			return;
		}
		if (InputIns.Triggered("ok"))
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			Apply();
			Note::SetType(noteType_);
			ScoreLoader::judgeDiff_ = judge_;
			AudioIns.StopAll(true);
			AudioIns.UnloadWAVFile("test");
			DrawerIns.changeEffect();
			updater_ = &SettingScene::BackToSelect;
		}
		});
}

void SettingScene::SetJudgeStr(void)
{
	judgeStr_.clear();
	judgeStr_.push_back(judge_ >= 0 ? '+' : '-');
	short j = abs(judge_);
	for (int i = 0; i < 2 - std::to_string(j).length(); i++)
	{
		judgeStr_.push_back('0');
	}
	judgeStr_ += std::to_string(j);
}
