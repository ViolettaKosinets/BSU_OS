#include <fstream>
#include <vector>
#include <windows.h>
#include <iostream>
#include "Employee.h"
#include <conio.h>

std::string fileName;
int numberOfEmployees;
std::vector<Employee> employees;
int numberOfClients;
HANDLE* pipes;
HANDLE* threads;
HANDLE* semaphores;
HANDLE* processes;

template <class T>
bool NumIsCorrect(T num)
{
	if (num <= 0)
	{
		std::cout << "Incorrect value, Try again\n";
		return false;
	}

	return true;
}

void EnterEmployees(std::ostream& os = std::cout, std::istream& is = std::cin)
{
	os << "\n\nNow enter all employees\n";

	for (int i = 0; i < numberOfEmployees; i++)
	{
		os << "\nEmployee number " << i + 1 << "\n\n";

		employees[i].num = i + 1;

		os << "Enter employee name: ";
		is >> employees[i].name;

		do
		{
			os << "Enter employee hours: ";
			is >> employees[i].hours;
		} while (!NumIsCorrect(employees[i].hours));
	}
}

void PrintEmployeesInFile(std::ostream& fout)
{
	for (int i = 0; i < numberOfEmployees; i++)
	{
		fout << employees[i].num
			<< " " << employees[i].name
			<< " " << employees[i].hours << "\n";
	}
}

void PrintEmployessInConsole(std::ostream& os = std::cout)
{
	os << "\nAll employees:\n\n";

	std::ifstream fin(fileName);
	for (int i = 0; i < numberOfEmployees; i++)
	{
		int id;
		std::string name;
		double hours;
		fin >> id >> name >> hours;

		os << "Employee " << i + 1 << ":\n"
			<< "Id: " << id
			<< "\tName: " << name
			<< "\tHours: " << hours << "\n\n";
	}
	fin.close();
}

void CreateSemaphores()
{
	for (int i = 0; i < numberOfEmployees; i++)
	{
		semaphores[i] = CreateSemaphore(NULL, numberOfClients, numberOfClients, L"semaphore");
	}
}

DWORD WINAPI ServerOperations(LPVOID pipe);

void CreateProcesses()
{
	for (int i = 0; i < numberOfClients; ++i)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		std::string cmd = "Client.exe";
		std::wstring strToWstr = std::wstring(cmd.begin(), cmd.end());
		LPWSTR clientCmdLine = &strToWstr[0];
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, clientCmdLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		processes[i] = CreateEvent(NULL, FALSE, FALSE, L"Process Started");
		CloseHandle(pi.hProcess);
	}
}

void CreateThreads()
{
	for (int i = 0; i < numberOfClients; i++)
	{
		pipes[i] = CreateNamedPipe(L"\\\\.\\pipe\\pipeName", PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);

		if (pipes == INVALID_HANDLE_VALUE)
		{
			std::cout << "Pipe creation error. Last error: "
				<< GetLastError() << "\n";
			_getch();

			exit(GetLastError());
		}

		if (!ConnectNamedPipe(pipes[i], (LPOVERLAPPED)NULL))
		{
			std::cout << "Connection failed. Last error "
				<< GetLastError() << "\n";
			CloseHandle(pipes[i]);
			_getch();

			exit(GetLastError());
		}

		threads[i] = CreateThread(NULL, 0, ServerOperations, static_cast<LPVOID>(pipes[i]), 0, NULL);
	}
}

int main()
{
	std::cout << "Enter file name: ";
	std::cin >> fileName;

	do
	{
		std::cout << "Enter number of employees: ";
		std::cin >> numberOfEmployees;
	} while (!NumIsCorrect(numberOfEmployees));

	employees.resize(numberOfEmployees);
	EnterEmployees();

	std::ofstream fout(fileName);
	PrintEmployeesInFile(fout);
	fout.close();

	PrintEmployessInConsole();

	do
	{
		std::cout << "Enter number of clients: ";
		std::cin >> numberOfClients;
	} while (!NumIsCorrect(numberOfClients));

	semaphores = new HANDLE[numberOfEmployees];
	CreateSemaphores();

	processes = new HANDLE[numberOfClients];
	CreateProcesses();
	WaitForMultipleObjects(numberOfClients, processes, TRUE, INFINITE);

	pipes = new HANDLE[numberOfClients];
	threads = new HANDLE[numberOfClients];
	CreateThreads();
	WaitForMultipleObjects(numberOfClients, threads, TRUE, INFINITE);

	PrintEmployessInConsole();

	delete[] processes;
	delete[] semaphores;
	delete[] pipes;
	delete[] threads;

	_getch();

	return 0;
}

void ChangeEmployee(int index, HANDLE& hPipe, DWORD& bytesWrite)
{
	Employee* emp = new Employee;
	*emp = employees[index];
	bool written = WriteFile(hPipe, emp, sizeof(Employee), &bytesWrite, NULL);
	if (written)
	{
		std::cout << "Data sent\n";
	}
	else
	{
		std::cout << "Data wasn`t sent\n";
	}

	bool read = ReadFile(hPipe, emp, sizeof(Employee), &bytesWrite, NULL);
	if (!read)
	{
		std::cout << "Missed read data";
	}
	employees[index] = *emp;

	std::ofstream fout(fileName);
	PrintEmployeesInFile(fout);
	fout.close();
}

void ReadEmployee(int index, HANDLE& hPipe, DWORD& bytesToWrite)
{
	Employee* emp = new Employee();
	*emp = employees[index];
	bool written = WriteFile(hPipe, emp, sizeof(Employee), &bytesToWrite, NULL);
	if (written)
	{
		std::cout << "Data to read sent\n";
	}
	else
	{
		std::cout << "Data to read wasn`t sent";
	}
}

DWORD WINAPI ServerOperations(LPVOID pipe)
{
	HANDLE hPipe = (HANDLE)pipe;
	DWORD bytesRead = 0;
	DWORD bytesWrite = 0;

	int message;
	while (true)
	{
		if (!ReadFile(hPipe,
			&message,
			sizeof(message),
			&bytesRead,
			NULL))
		{
			std::cout << "Data reading failed\n\n";
		}
		else
		{
			int ID = message / 10;
			int option = message % 10;
			if (option == 1)
			{
				for (int i = 0; i < numberOfClients; i++)
				{
					WaitForSingleObject(semaphores[ID - 1], INFINITE);
				}

				ChangeEmployee(ID - 1, hPipe, bytesWrite);

				int msg;
				ReadFile(hPipe, &msg, sizeof(msg), &bytesWrite, NULL);
				if (msg == 1)
				{
					for (int i = 0; i < numberOfClients; i++)
					{
						ReleaseSemaphore(semaphores[ID - 1], 1, NULL);
					}
				}
			}

			if (option == 2)
			{
				WaitForSingleObject(semaphores[ID - 1], INFINITE);

				ReadEmployee(ID - 1, hPipe, bytesWrite);

				int msg;
				ReadFile(hPipe, &msg, sizeof(msg), &bytesWrite, NULL);
				if (msg == 1)
				{
					ReleaseSemaphore(semaphores[ID - 1], 1, NULL);
				}

			}

			if (option == 3)
			{
				break;
			}
		}
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	return 0;
}