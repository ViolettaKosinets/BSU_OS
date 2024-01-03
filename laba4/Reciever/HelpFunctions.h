#pragma once
#include <iostream>
#include <fstream>

void EnterNums(int& num, std::string message, std::istream& is = std::cin, std::ostream& os = std::cout)
{
link:
	os << message;
	is >> num;

	if (num <= 0)
	{
		system("cls");
		os << "Incorrect input, try again\n";
		goto link;
	}
}

void CreateBinaryFile(const std::string& binaryFileName)
{
	std::fstream file;
	file.open(binaryFileName, std::ios::out);
	file.close();
}

void EnterOption(int& key, std::istream& is = std::cin, std::ostream& os = std::cout)
{
link:
	std::cout << "\nChoose options:\n";
	std::cout << "1. Read message\n";
	std::cout << "2. Exit process\n";
	std::cout << "Your choice is: ";
	std::cin >> key;
	if (key != 1 && key != 2)
	{
		system("cls");
		std::cout << "Incorrect input. Try again(\n\n";
		goto link;
	}
}