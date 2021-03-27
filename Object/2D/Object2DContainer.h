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
	/// オブジェクトの格納
	/// </summary>
	/// <param name="object">格納したいオブジェクト</param>
	/// <param name="key">キー</param>
	void Add(Object2D* object, const std::string& key);

	/// <summary>
	/// オブジェクトの参照
	/// </summary>
	/// <param name="key">参照したいオブジェクトのキー</param>
	/// <returns>オブジェクトの参照</returns>
	const std::shared_ptr<Object2D>& Get(const std::string& key);
private:
	std::unordered_map<std::string, std::shared_ptr<Object2D>> object_;
};

