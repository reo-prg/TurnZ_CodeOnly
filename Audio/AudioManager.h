#pragma once
#include <xaudio2.h>
#include <string>
#include <memory>
#include <unordered_map>

#define AudioIns AudioManager::GetInstance()

enum class VoiceState
{
	Playing,
	Stop,
};

enum class VoiceType
{
	Music,
	Effect,
};

class WAVLoader;
class AudioManager
{
public:
	static AudioManager& GetInstance(void);

	void LoadWAVFile(const std::string& filename, const std::string& key);
	void PlayWAVFile(const std::string& key, VoiceType type = VoiceType::Music);
	void PlayWAVFileLoop(const std::string& key, float begin, float length, unsigned int loopCount, VoiceType type = VoiceType::Effect);
	void SetVolume(const std::string& key, float volume);
	void ContinueWAVFile(const std::string& key);
	void StopWAVFile(const std::string& key, bool destroy);
	void UnloadWAVFile(const std::string& key);
	void ContinueAll(void);
	void StopAll(bool destroy, bool force = false);

	bool IsEnd(const std::string& key);

	float GetMVolume(void);
	float GetEVolume(void);

	void SetMVolume(float vol);
	void SetEVolume(float vol);

	void Update(void);
private:
	AudioManager();
	AudioManager(const AudioManager&) = delete;
	AudioManager operator=(const AudioManager&) = delete;
	~AudioManager();

	void Initialize(void);

	std::unique_ptr<WAVLoader> wavLoader_;

	IXAudio2* xaudioCore_;
	IXAudio2MasteringVoice* masterVoice_;

	float volume_[2];
	static constexpr float musicVolumeScale_ = 0.3f;
	static constexpr float effectVolumeScale_ = 0.7f;
	
	struct SourceV
	{
		WAVEFORMATEX waveFormat_;
		XAUDIO2_BUFFER buffer_;
		IXAudio2SourceVoice* sourceVoice_;
		VoiceState vState_;
		VoiceType type_;
	};
	std::unordered_map<std::string, std::string> filenameTable_;
	std::unordered_map<std::string, std::list<SourceV>> sources_;
};

