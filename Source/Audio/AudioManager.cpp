#include "AudioManager.h"
#include <cassert>
#include "WAVLoader.h"

AudioManager& AudioManager::GetInstance(void)
{
	static AudioManager a;
	return a;
}

void AudioManager::LoadWAVFile(const std::string& filename, const std::string& key)
{
	if (!wavLoader_->LoadWAVFile(filename))
	{
		return;
	}
	filenameTable_.emplace(key, filename);
}

void AudioManager::PlayWAVFile(const std::string& key, VoiceType type)
{
	if (filenameTable_.find(key) == filenameTable_.end())
	{
		OutputDebugString(L"key not found");
		return;
	}
	if (sources_.find(key) == sources_.end())
	{
		sources_.emplace(key, std::list<SourceV>());
	}
	HRESULT result;
	SourceV source = {};

	const auto& data = wavLoader_->GetWAVFile(filenameTable_.at(key));
	if (data.dataSize_ == UINT_MAX)
	{
		OutputDebugString(L"key not found");
		return;
	}

	source.waveFormat_.wFormatTag = WAVE_FORMAT_PCM;
	source.waveFormat_.nChannels = data.fmt_.channel_;
	source.waveFormat_.nSamplesPerSec = data.fmt_.samplesPerSec_;
	source.waveFormat_.nAvgBytesPerSec = data.fmt_.bytePerSec_;
	source.waveFormat_.nBlockAlign = data.fmt_.blockAlign_;
	source.waveFormat_.wBitsPerSample = data.fmt_.bitPerSample_;

	source.buffer_.AudioBytes = data.dataSize_;
	source.buffer_.pAudioData = data.data_;
	source.buffer_.PlayBegin = 0;
	source.buffer_.PlayLength = 0;
	source.buffer_.LoopBegin = 0;
	source.buffer_.LoopCount = 0;
	source.buffer_.LoopLength = 0;
	source.buffer_.Flags = XAUDIO2_END_OF_STREAM;

	result = xaudioCore_->CreateSourceVoice(&source.sourceVoice_, &source.waveFormat_, 0, 4.0f);
	if (FAILED(result)) { return; }

	result = source.sourceVoice_->SubmitSourceBuffer(&source.buffer_);
	if (FAILED(result)) { return; }

	source.vState_ = VoiceState::Playing;
	source.type_ = type;
	source.sourceVoice_->Start();
	source.sourceVoice_->SetVolume(volume_[static_cast<unsigned int>(type)]);

	sources_.at(key).emplace_back(source);
}

void AudioManager::PlayWAVFileLoop(const std::string& key, float begin, float length, unsigned int loopCount, VoiceType type)
{
	if (filenameTable_.find(key) == filenameTable_.end())
	{
		OutputDebugString(L"key not found");
		return;
	}
	if (sources_.find(key) == sources_.end())
	{
		sources_.emplace(key, std::list<SourceV>());
	}
	HRESULT result;
	SourceV source = {};

	const auto& data = wavLoader_->GetWAVFile(filenameTable_.at(key));
	if (data.dataSize_ == UINT_MAX)
	{
		OutputDebugString(L"key not found");
		return;
	}

	source.waveFormat_.wFormatTag = WAVE_FORMAT_PCM;
	source.waveFormat_.nChannels = data.fmt_.channel_;
	source.waveFormat_.nSamplesPerSec = data.fmt_.samplesPerSec_;
	source.waveFormat_.nAvgBytesPerSec = data.fmt_.bytePerSec_;
	source.waveFormat_.nBlockAlign = data.fmt_.blockAlign_;
	source.waveFormat_.wBitsPerSample = data.fmt_.bitPerSample_;

	source.buffer_.AudioBytes = data.dataSize_;
	source.buffer_.pAudioData = data.data_;
	source.buffer_.PlayBegin = static_cast<UINT32>(static_cast<float>(source.waveFormat_.nSamplesPerSec) * begin);
	source.buffer_.PlayLength = static_cast<UINT32>(static_cast<float>(source.waveFormat_.nSamplesPerSec) * length);
	source.buffer_.LoopBegin = static_cast<UINT32>(static_cast<float>(source.waveFormat_.nSamplesPerSec) * begin);
	source.buffer_.LoopLength = static_cast<UINT32>(static_cast<float>(source.waveFormat_.nSamplesPerSec) * length);
	source.buffer_.LoopCount = loopCount;
	source.buffer_.Flags = XAUDIO2_END_OF_STREAM;

	result = xaudioCore_->CreateSourceVoice(&source.sourceVoice_, &source.waveFormat_, 0, 4.0f);
	if (FAILED(result)) { return; }

	result = source.sourceVoice_->SubmitSourceBuffer(&source.buffer_);
	if (FAILED(result)) { return; }

	source.vState_ = VoiceState::Playing;
	source.type_ = type;
	source.sourceVoice_->Start();
	source.sourceVoice_->SetVolume(volume_[static_cast<unsigned int>(type)]);

	sources_.at(key).emplace_back(source);
}

void AudioManager::SetVolume(const std::string& key, float volume)
{
	if (sources_.find(key) == sources_.end())
	{
		OutputDebugString(L"SetVolume:key not found");
		return;
	}
	for (auto& s : sources_.at(key))
	{
		if (s.type_ == VoiceType::Music)
		{
			s.sourceVoice_->SetVolume(volume_[static_cast<unsigned int>(VoiceType::Music)] * volume);
		}
		else if (s.type_ == VoiceType::Effect)
		{
			s.sourceVoice_->SetVolume(volume_[static_cast<unsigned int>(VoiceType::Effect)] * volume);
		}
		else
		{
			s.sourceVoice_->SetVolume(volume);
		}
	}
}

void AudioManager::ContinueWAVFile(const std::string& key)
{
	if (sources_.find(key) != sources_.end())
	{
		for (auto& s : sources_.at(key))
		{
			if (s.vState_ == VoiceState::Playing) { continue; }
			s.sourceVoice_->Start();
			s.vState_ = VoiceState::Playing;
		}
		sources_.at(key).clear();
	}
}

void AudioManager::StopWAVFile(const std::string& key, bool destroy)
{
	if (sources_.find(key) != sources_.end())
	{
		for (auto& s : sources_.at(key))
		{
			if (s.vState_ == VoiceState::Stop) { continue; }
			s.sourceVoice_->Stop();
			s.vState_ = VoiceState::Stop;
			if (destroy) { s.sourceVoice_->DestroyVoice(); }
		}
		sources_.at(key).clear();
	}
}

void AudioManager::UnloadWAVFile(const std::string& key)
{
	if (filenameTable_.find(key) == filenameTable_.end()) { return; }
	wavLoader_->DestroyWAVFile(filenameTable_.at(key));
	filenameTable_.erase(key);
}

void AudioManager::ContinueAll(void)
{
	for (auto& s : sources_)
	{
		for (auto& v : s.second)
		{
			if (v.vState_ == VoiceState::Playing) { continue; }
			v.sourceVoice_->Start();
			v.vState_ = VoiceState::Playing;
		}
	}
}

void AudioManager::StopAll(bool destroy, bool force)
{
	if (force)
	{
		for (auto& s : sources_)
		{
			for (auto& v : s.second)
			{
				if (v.vState_ != VoiceState::Stop)
				{
					v.sourceVoice_->Stop();
				}
				v.sourceVoice_->DestroyVoice();
			}
			s.second.clear();
		}
		sources_.clear();
	}
	else
	{
		for (auto& s : sources_)
		{
			for (auto& v : s.second)
			{
				if (v.vState_ == VoiceState::Stop) { continue; }
				v.sourceVoice_->Stop();
				v.vState_ = VoiceState::Stop;
				if (destroy) { v.sourceVoice_->DestroyVoice(); }
			}
			if (destroy) { s.second.clear(); }
		}
	}
}

bool AudioManager::IsEnd(const std::string& key)
{
	if (sources_.find(key) == sources_.end()) { return true; }
	if (sources_.at(key).empty()) { return true; }
	return false;
}

float AudioManager::GetMVolume(void)
{
	return volume_[static_cast<unsigned int>(VoiceType::Music)];
}

float AudioManager::GetEVolume(void)
{
	return volume_[static_cast<unsigned int>(VoiceType::Effect)];
}

void AudioManager::SetMVolume(float vol)
{
	volume_[static_cast<unsigned int>(VoiceType::Music)] = vol * musicVolumeScale_;
	for (auto& sr : sources_)
	{
		for (auto& s : sr.second)
		{
			if (s.type_ == VoiceType::Music)
			{
				s.sourceVoice_->SetVolume(vol * musicVolumeScale_);
			}
		}
	}
}

void AudioManager::SetEVolume(float vol)
{
	volume_[static_cast<unsigned int>(VoiceType::Effect)] = vol * effectVolumeScale_;
	for (auto& sr : sources_)
	{
		for (auto& s : sr.second)
		{
			if (s.type_ == VoiceType::Effect)
			{
				s.sourceVoice_->SetVolume(vol * effectVolumeScale_);
			}
		}
	}
}

void AudioManager::Update(void)
{
	for (auto& s : sources_)
	{
		for (auto it = s.second.begin(); it != s.second.end();)
		{
			XAUDIO2_VOICE_STATE state;
			it->sourceVoice_->GetState(&state);
			if (state.BuffersQueued == 0)
			{
				it->sourceVoice_->Stop();
				it->sourceVoice_->DestroyVoice();
				it = s.second.erase(it);
				continue;
			}
			it++;
		}
	}
}

AudioManager::AudioManager():xaudioCore_(nullptr), masterVoice_(nullptr)
{
	Initialize();
}

AudioManager::~AudioManager()
{
	for (auto& s : sources_)
	{
		for (auto& v : s.second)
		{
			v.sourceVoice_->DestroyVoice();
		}
		s.second.clear();
	}
	sources_.clear();
	if (masterVoice_)
	{
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	
	xaudioCore_->Release();
}

void AudioManager::Initialize(void)
{
	HRESULT result;

	wavLoader_.reset(new WAVLoader());
	
	// IXAudio2オブジェクトの作成
	result = XAudio2Create(&xaudioCore_);
	assert(SUCCEEDED(result));

	// デバッグの設定
#ifdef _DEBUG
	XAUDIO2_DEBUG_CONFIGURATION debugc = {};
	debugc.TraceMask = XAUDIO2_LOG_WARNINGS;
	debugc.BreakMask = XAUDIO2_LOG_ERRORS;
	xaudioCore_->SetDebugConfiguration(&debugc);
#endif

	// マスタリングボイスの作成
	result = xaudioCore_->CreateMasteringVoice(&masterVoice_, XAUDIO2_DEFAULT_CHANNELS,
		XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, nullptr);
	assert(SUCCEEDED(result));

}
