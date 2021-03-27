#pragma once

namespace
{
	enum class JudgeType : unsigned int
	{
		Awesome,
		Good,
		Bad,
		Miss,
		None
	};

	JudgeType CheckJudge(short diff)
	{
		if (diff < 0)
		{
			return JudgeType::None;
		}
		static constexpr short awesome = 2;
		static constexpr short good = 5;
		static constexpr short bad = 9;
		static constexpr short miss = 12;

		if (diff < awesome) { return JudgeType::Awesome; }
		if (diff < good) { return JudgeType::Good; }
		if (diff < bad) { return JudgeType::Bad; }
		if (diff < miss) { return JudgeType::Miss; }
		return JudgeType::None;
	}
}
