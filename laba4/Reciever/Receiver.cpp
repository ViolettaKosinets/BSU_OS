#include <fstream>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include<cstdio>


int main()
{
	std::string bin_file_name;
	std::cout << "Enter binary file name\n";
	std::cin >> bin_file_name;

	long long number_of_notes;
	std::cout << "Enter number of notes:\n";
	std::cin >> number_of_notes;

	std::fstream file;
<<<<<<< HEAD
	file.open(bin_file_name, std::ios::trunc);
	file.open(bin_file_name, std::ios::out);
=======
	file.open(bin_file_name, std::ios::trunc | std::ios::out);
	//file.open(bin_file_name, std::ios::out);
>>>>>>> 7545492234984373973f0f119b95167c1b80c203
	file.close();

	int number_of_senders;
	std::cout << "Enter number of Sender processes:\n";
	std::cin >> number_of_senders;

	HANDLE hWriteReadySemaphore = CreateSemaphore(NULL, 0, number_of_notes, L"Input_semaphore_started");
	if (hWriteReadySemaphore == NULL)
		return GetLastError();

	HANDLE hMutex = CreateMutex(NULL, 0, L"mutex");

	HANDLE* hEventStarted = new HANDLE[number_of_senders];
	HANDLE hOutput = CreateEvent(NULL, 0, 0, L"read");

	LPWSTR lpwstrSenderProcessCommandLine;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	for (int i = 0; i < number_of_senders; ++i)
	{
		std::string sender_cmd = "Sender.exe " + bin_file_name;
		std::wstring converting_sender_to_lpwstr = std::wstring(sender_cmd.begin(), sender_cmd.end());
		lpwstrSenderProcessCommandLine = &converting_sender_to_lpwstr[0];

		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

<<<<<<< HEAD
		if (!CreateProcess(NULL, lpwstrSenderProcessCommandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			std::cout << "The Sender process is not started.\n";
			return GetLastError();
=======
		if (!CreateProcess(NULL, lpwstrSenderProcessCommandLine, NULL, NULL, TRUE ,CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			std::cout << "The Sender process is not started.\n";
			GetLastError();
>>>>>>> 7545492234984373973f0f119b95167c1b80c203
		}

		hEventStarted[i] = CreateEvent(NULL, FALSE, FALSE, L"Process_started");

		if (hEventStarted[i] == NULL)
			return GetLastError();

		CloseHandle(pi.hProcess);
	}

	WaitForMultipleObjects(number_of_senders, hEventStarted, TRUE, INFINITE);

	std::string number;

	file.open(bin_file_name, std::ios::in);

	while (true)
	{
		std::cout << "\nEnter 1 to read message\nEnter 0 to exit process\n";
		std::cin >> number;

		if (number == "1")
		{
			std::string message;

			WaitForSingleObject(hWriteReadySemaphore, INFINITE);
			WaitForSingleObject(hMutex, INFINITE);

			std::getline(file, message);

			std::cout << message;

			SetEvent(hOutput);
			ReleaseMutex(hMutex);
		}
		else if (number == "0")
		{
			std::cout << "Process ended";
			break;
		}
		else if (number != "0" && number != "1")
			std::cout << "\nIncorrect value. Enter again\n";
	}

	file.close();

	CloseHandle(hWriteReadySemaphore);
	CloseHandle(hOutput);

	for (int i = 0; i < number_of_senders; i++)
		CloseHandle(hEventStarted[i]);

	return 0;
}