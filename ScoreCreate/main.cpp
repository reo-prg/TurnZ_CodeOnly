#include <vector>

struct StartData
{
	unsigned short offset_;
	short coordX_;
	short coordY_;
	unsigned char dir_;
	unsigned short bpm_;
	float speed_;
};

struct ScoreData
{
	StartData start_;
	unsigned short note_count_;
	std::vector<unsigned char> note_;
};

//	0:Ç»Çµ
//	1:è„	Å™
//	2:âE	Å®
//	3:â∫	Å´
//	4:ç∂	Å©
//	5:è¨êﬂ

int main(void)
{
	ScoreData score;

	score.start_.offset_ = 158;
	score.start_.coordX_ = 640;
	score.start_.coordY_ = 1000;
	score.start_.dir_ = 1;
	score.start_.bpm_ = 180;
	score.start_.speed_ = 5.0f;

	score.note_ = {
		2,5,
		3,5,
		4,5,
		1,5,
		4,5,
		3,5,
		2,5,
		1,5,

		4,5,
		3,0,0,2,0,0,3,0,5,
		2,5,
		1,0,0,4,0,0,1,0,5,
		2,5,
		3,0,0,4,0,0,3,0,5,
		4,5,
		1,2,5,

		4,5,
		1,0,0,2,0,0,3,0,5,
		3,5,
		4,0,0,1,0,0,2,0,5,
		2,5,
		3,0,0,4,0,0,1,0,5,
		1,5,
		4,0,0,0,0,3,4,0,5,

		3,2,5,
		3,4,1,0,5,
		4,3,5,
		2,1,2,3,5,
		2,1,5,
		4,3,4,3,5,
		2,2,5,
		1,1,4,3,5,

		4,3,5,
		2,1,2,1,5,
		2,3,5,
		4,4,1,1,5,
		4,3,5,
		2,1,4,1,5,
		2,2,5,
		3,3,4,3,5,

		4,0,4,0,1,1,1,0,5,
		2,0,2,0,3,0,4,0,5,
		1,0,1,0,2,2,2,0,5,
		1,0,1,0,4,0,3,0,5,
		4,0,0,0,3,3,3,0,5,
		2,0,2,0,3,0,2,0,5,		
		1,0,1,0,4,4,4,0,5,
		3,0,3,0,4,4,1,1,5,

		4,0,4,0,1,1,1,0,5,
		2,0,2,0,3,0,2,0,5,
		3,0,3,0,4,4,4,0,5,
		1,0,1,0,4,0,3,0,5,
		4,0,4,4,1,0,1,1,5,
		2,0,2,2,3,0,0,0,5,
		2,2,2,2,5,
		1,4,5,

		3,5,
		2,1,5,
		4,5,
		3,2,5,
		1,5,
		4,3,5,
		2,5,
		1,4,5,

		4,3,5,
		2,0,3,2,0,3,2,3,5,
		2,1,5,
		4,0,0,1,0,0,4,0,0,3,0,0,4,0,4,0,5,
		3,2,3,2,5,
		1,2,1,1,5,
		4,0,4,0,3,0,3,0,2,2,2,2,2,2,2,2,5,
		0,0,3,4,5,

		1,1,4,4,5,
		3,0,4,3,0,4,3,0,5,
		4,1,4,1,5,
		2,2,2,2,0,0,0,0,5,
		3,2,3,4,5,
		4,0,4,4,0,4,1,4,5,
		0,1,1,1,2,2,2,2,5,
		3,4,5,

		1,1,2,3,5,
		4,0,1,4,0,1,4,0,5,
		3,2,2,1,5,
		2,2,2,2,0,3,3,3,5,
		4,4,1,1,5,
		4,0,3,4,0,3,3,3,5,
		2,2,2,2,1,1,1,1,5,
		4,4,4,4,4,4,4,4,4,0,1,0,1,0,0,0,5,

		2,3,2,3,5,
		1,4,1,4,5,
		3,0,3,3,3,0,2,2,5,
		0,1,1,1,0,2,2,2,5,
		3,2,1,1,5,
		4,4,3,3,5,
		4,3,4,1,5,
		2,2,2,2,2,0,0,0,5,
	};
	unsigned short dataSize = static_cast<unsigned short>(score.note_.size());

	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, "Score/Chartreuse Green2.score", "wb");
	if (err != 0) { return -1; }

	fwrite(&score.start_.offset_, sizeof(score.start_.offset_), 1, fp);
	fwrite(&score.start_.coordX_, sizeof(score.start_.coordX_), 1, fp);
	fwrite(&score.start_.coordY_, sizeof(score.start_.coordY_), 1, fp);
	fwrite(&score.start_.dir_, sizeof(score.start_.dir_), 1, fp);
	fwrite(&score.start_.bpm_, sizeof(score.start_.bpm_), 1, fp);
	fwrite(&score.start_.speed_, sizeof(score.start_.speed_), 1, fp);
	fwrite(&dataSize, sizeof(dataSize), 1, fp);
	fwrite(score.note_.data(), sizeof(unsigned  char), score.note_.size(), fp);

	fclose(fp);

	return 0;
}