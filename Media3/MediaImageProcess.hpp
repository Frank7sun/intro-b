#pragma once

//�K�v�ȃC���N���[�h�t�@�C���̐ݒ�
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

//�G���[���b�Z�[�W�̕\��
#define ErrorLog(msg) printf("Error Files : %s\nError Line : %d\nError Massage : %s\n", __FILE__, __LINE__, msg)

namespace media
{
	//�N���X�錾
	class MediaCom;

	//��`
	class MediaCom
	{
	public:
		//�R���X�g���N�^
		MediaCom();

		//�f�X�g���N�^
		~MediaCom();
		
		//�t�@�C���̓ǂݍ��݁i�p�X�{�t�@�C�����j
		bool ReadFile(const std::string PATH);
		bool ReadFile(std::vector<std::string> &fileData, const std::string PATH);
		bool ReadFile(std::map<int, std::string> &fileData, const std::string PATH);

		//�t�H���_�̗L���̊m�F�B�Ȃ���΍쐬����
		bool CheckExistenceOfFolder(const std::string folder_name);

		//�w�肵���ӏ��̃e�L�X�g�ǂݍ���
		std::string GetText(const int index);

		//���b�Z�[�W�̃R���\�[���o��
		void AgentSpeaker(const std::string message);
		void HumanSpeaker(const std::string message);
	
		//�R���\�[�����~�߂�
		void Stop();

		//�x�N�g���z��̏�����
		template<typename T>
		void Init_Vector(std::vector<T> &vec);

		//�������Ԍv��
		void Restart();
		double Elapsed();
		void ProcessTime();

		//�x�N�g���z��̕\��
		void PrintVector(std::vector<int> &vec);
		void PrintVector(std::vector<double> &vec);
		void PrintVector(std::vector<std::string> &vec);

	private:
		//�t�@�C���̒��g���i�[����
		std::vector<std::string> read_files;

		DWORD start;

	};
}