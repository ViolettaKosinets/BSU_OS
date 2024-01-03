#pragma once
#include <iostream>
#include <fstream>

void EnterOption(int& key, std::istream& is = std::cin, std::ostream& os = std::cout)
{
link:
	std::cout << "\nEventWasStarted\n\n";
	std::cout << "Choose options:\n";
	std::cout << "1. Write message\n";
	std::cout << "2. Exit process\n";
	std::cout << "Your choice is: ";
	std::cin >> key;
	if (key != 1 && key != 2)
	{
		system("cls");
		std::cout << "Incorrect input. Try again :-(\n\n";
		goto link;
	}
}