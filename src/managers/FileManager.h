#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>
#include <boost/hana.hpp>
#include <boost/hana/adapt_struct.hpp>
#include "project/projectManagment.h"

namespace hana = boost::hana;

class FileManager
{
public:
	static FileManager &getInstance()
	{
		static FileManager instance;
		return instance;
	}

	bool writeTextFile(const std::string &filename, const std::string &content);
	std::string readTextFile(const std::string &filename);

	template <typename T>
	bool readMember(std::ifstream &file, T &member)
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			uint32_t length;
			if (!file.read(reinterpret_cast<char *>(&length), sizeof(length)))
				return false; // Falha ao ler o tamanho da string

			std::vector<char> buffer(length);
			if (!file.read(buffer.data(), length))
				return false; // Falha ao ler os dados da string

			member = std::string(buffer.data(), length); // Converte para string
			return true;
		}
		else
		{
			// Leitura de tipos primitivos (como int, float, etc.)
			file.read(reinterpret_cast<char *>(&member), sizeof(member));
			return file.good(); // Verifica se a leitura foi bem-sucedida
		}
	}

	template <typename T>
	bool writeMember(std::ofstream &file, const T &member)
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			uint32_t length = member.length();
			if (!file.write(reinterpret_cast<const char *>(&length), sizeof(length)))
			{
				std::cerr << "Failed to write string length" << std::endl;
				return false;
			}

			if (length > 0)
			{
				if (!file.write(member.data(), length))
				{
					std::cerr << "Failed to write string data" << std::endl;
					return false;
				}
			}
		}
		else
		{
			if (!file.write(reinterpret_cast<const char *>(&member), sizeof(member)))
			{
				std::cerr << "Failed to write data" << std::endl;
				return false;
			}
		}
		return true;
	}

	template <typename T>
	bool writeBinaryFile(const std::string &filename, const T &data)
	{
		std::ofstream file(filename, std::ios::binary);
		if (!file)
		{
			std::cerr << "Failed to open file for writing" << std::endl;
			return false;
		}

		bool success = true;
		// Itera sobre os membros da estrutura
		hana::for_each(data, [&](auto member)
					   { success &= writeMember(file, member); });

		if (!success)
		{
			std::cerr << "Failed to write some data to the file" << std::endl;
		}

		return success;
	}

	template <typename T>
	bool readBinaryFile(const std::string &filename, T &data)
	{
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file for reading" << std::endl;
			return false;
		}

		bool success = true;
		hana::for_each(data, [&](auto member)
					   { success &= readMember(file, member); });

		if (!success)
		{
			std::cerr << "Failed to read some data from the file" << std::endl;
		}

		return success;
	}

	bool createDirectory(const std::string &path);
	bool deleteFile(const std::string &path);
	bool fileExists(const std::string &path);
	std::vector<std::string> listFiles(const std::string &directory);
	size_t getFileSize(const std::string &path);
	std::string getFileExtension(const std::string &path);

private:
	FileManager() = default;
};
