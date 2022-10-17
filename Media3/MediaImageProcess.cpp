#include "MediaImageProcess.hpp"

using namespace media;

MediaCom::MediaCom()
{
	read_files.shrink_to_fit();
}

MediaCom::~MediaCom() {}

bool MediaCom::ReadFile(const std::string PATH)
{
	std::fstream file(PATH);

	//ファイルの読み込み失敗
	if (file.fail())
	{
		std::cerr << "Not Read [[" << PATH << "]]\n";
		return false;
	}

	std::string line;
	while (std::getline(file, line, '\n')) {
		//「@」マークがあるときその1行をコメント扱いにする
		std::string::size_type atmark = line.find("@");

		if (atmark == std::string::npos) {
			std::istringstream ss(line);
			std::string s;

			while (std::getline(ss, s, '\t')) {
				read_files.push_back(s);
			}
		}
	}

	file.close();
	return true;
}

bool MediaCom::ReadFile(std::vector<std::string> &fileData, const std::string PATH)
{
	std::fstream file(PATH);

	//ファイルの読み込み失敗
	if (file.fail())
	{
		std::cerr << "Not Read [[" << PATH << "]]\n";
		return false;
	}

	std::string line;
	while (std::getline(file, line, '\n')) {
		//「@」マークがあるときその1行をコメント扱いにする
		std::string::size_type atmark = line.find("@");

		if (atmark == std::string::npos) {
			std::istringstream ss(line);
			std::string s;

			while (std::getline(ss, s, ',')) {
				fileData.push_back(s);
			}
		}
	}

	file.close();
	return true;
}

bool MediaCom::ReadFile(std::map<int, std::string> &fileData, const std::string PATH)
{
	std::ifstream file(PATH);
	if (file.fail())
	{
		std::cerr << "Not Read [[" << PATH << "]]\n";
		return false;
	}

	std::string line;
	while (std::getline(file, line, '\n'))
	{
		std::string::size_type atmark = line.find("@");
		if (atmark == std::string::npos)
		{
			std::vector<std::string> ldata;

			std::istringstream ss(line);
			std::string s;

			while (std::getline(ss, s, ','))
				ldata.push_back(s);

			fileData.insert(std::map<int, std::string>::value_type(atoi(ldata[0].c_str()), ldata[1].c_str()));
		}
	}
	file.close();

	return true;
}

/**
* @brief
*   ディレクトリが存在するか確認
* @note
*   ディレクトリがなければ作成
*/
bool MediaCom::CheckExistenceOfFolder(const std::string folder_name)
{
	if (_mkdir(folder_name.c_str()) == 0) {
		return true;
	}
	else {
		return false;
	}
}

std::string MediaCom::GetText(const int index)
{
	if (index > read_files.size())
	{
		ErrorLog("");
		throw std::runtime_error("Not found index");
	}

	return read_files[index];
}

void MediaCom::AgentSpeaker(const std::string message)
{
	std::cerr << "   Agent Message -> [" << message << "]\n";
}

void MediaCom::HumanSpeaker(const std::string message)
{
	std::cerr << "   Human Message -> [" << message << "]\n";
}

void MediaCom::Stop()
{
	std::cerr << "Press bottom\n";
	system("pause");
}

template<typename T>
void MediaCom::Init_Vector(std::vector<T> &vec)
{
	vec.clear();
	vec.begin();
	vec.shrink_to_fit();
	std::vector<T>().swap(vec);
}

//ベクトル配列の表示
void MediaCom::PrintVector(std::vector<int> &vec)
{
	std::cerr << vec[0];
	for (int i = 1; i < vec.size(); ++i)
		std::cerr << " " << vec[i];
	std::cerr << "\n";
}
void MediaCom::PrintVector(std::vector<double> &vec)
{
	std::cerr << vec[0];
	for (int i = 1; i < vec.size(); ++i)
		std::cerr << " " << vec[i];
	std::cerr << "\n";
}
void MediaCom::PrintVector(std::vector<std::string> &vec)
{
	std::cerr << vec[0];
	for (int i = 1; i < vec.size(); ++i)
		std::cerr << " " << vec[i];
	std::cerr << "\n";
}

//処理時間計測
void MediaCom::Restart()
{
	start = timeGetTime();
}

double MediaCom::Elapsed()
{
	DWORD end = timeGetTime();
	return (double)(end - start) / 1000;
}

void MediaCom::ProcessTime()
{
	std::cerr << "Process time: " << Elapsed() << "[s].\n\n";
}





