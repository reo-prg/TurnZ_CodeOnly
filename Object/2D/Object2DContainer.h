#pragma once
#include <unordered_map>
#include <memory>
#include <string>

class Object2D;
class Object2DContainer
{
public:
	Object2DContainer() = default;
	~Object2DContainer() = default;

	/// <summary>
	/// �I�u�W�F�N�g�̊i�[
	/// </summary>
	/// <param name="object">�i�[�������I�u�W�F�N�g</param>
	/// <param name="key">�L�[</param>
	void Add(Object2D* object, const std::string& key);

	/// <summary>
	/// �I�u�W�F�N�g�̎Q��
	/// </summary>
	/// <param name="key">�Q�Ƃ������I�u�W�F�N�g�̃L�[</param>
	/// <returns>�I�u�W�F�N�g�̎Q��</returns>
	const std::shared_ptr<Object2D>& Get(const std::string& key);
private:
	std::unordered_map<std::string, std::shared_ptr<Object2D>> object_;
};

