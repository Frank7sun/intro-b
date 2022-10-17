#pragma once

//必要なインクルードファイルの設定
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <direct.h>
#include <map>
#include <exception>
#include <stdexcept> 
#include <algorithm>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#include "OPENCV.hpp"
#include "OPENNI.hpp"

//エラーメッセージの表示
#define ErrorLog(msg) printf("Error Files : %s\nError Line : %d\nError Massage : %s\n", __FILE__, __LINE__, msg)

namespace media
{
	//クラス宣言
	class MediaCom;

	//定義
	class MediaCom
	{
	public:
		//コンストラクタ
		MediaCom();

		//デストラクタ
		~MediaCom();
		
		//ファイルの読み込み（パス＋ファイル名）
		bool ReadFile(const std::string PATH);
		bool ReadFile(std::vector<std::string> &fileData, const std::string PATH);
		bool ReadFile(std::map<int, std::string> &fileData, const std::string PATH);

		//フォルダの有無の確認。なければ作成する
		bool CheckExistenceOfFolder(const std::string folder_name);

		//指定した箇所のテキスト読み込み
		std::string GetText(const int index);

		//メッセージのコンソール出力
		void AgentSpeaker(const std::string message);
		void HumanSpeaker(const std::string message);
	
		//コンソールを止める
		void Stop();

		//ベクトル配列の初期化
		template<typename T>
		void Init_Vector(std::vector<T> &vec);

		//処理時間計測
		void Restart();
		double Elapsed();
		void ProcessTime();

		//ベクトル配列の表示
		void PrintVector(std::vector<int> &vec);
		void PrintVector(std::vector<double> &vec);
		void PrintVector(std::vector<std::string> &vec);

	private:
		//ファイルの中身を格納する
		std::vector<std::string> read_files;

		DWORD start;

	};
}