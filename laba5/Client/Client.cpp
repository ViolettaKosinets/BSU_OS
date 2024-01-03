#include "Employee.h"
#include <conio.h>
#include <windows.h>
#include <iostream>


HANDLE CreatePipe()
{
	HANDLE pipe = CreateFile(L"\\\\.\\pipe\\pipeName",
		GENERIC_WRITE | GENERIC_READ,
		0, NULL, OPEN_EXISTING, 0, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "Pipe creation failed\n";
		std::cout << "Last error: " << GetLastError();
		_getch();

		exit(GetLastError());
	}

	return pipe;
}

void ChangeEmployee(Employee* emp)
{
	std::cout << "\nEmployee Id: " << emp->num << "\n"
		<< "Employee name: " << emp->name << "\n"
		<< "Employee hours: " << emp->hours << "\n";

	std::cout << "Enter new name: ";
	std::cin >> emp->name;

	std::cout << "Enter new hours: ";
	std::cin >> emp->hours;
}

void ReadEmployee(Employee* emp)
{
	std::cout << "\nEmployee Id: " << emp->num << "\n"
		<< "Employee name: " << emp->name << "\n"
		<< "Employee hours: " << emp->hours << "\n";
}

int main(int argc, char* argv)
{
	HANDLE event = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"Process Started");
	if (event == NULL)
	{
		std::cout << "Event failed";
		_getch();
		return GetLastError();
	}
	SetEvent(event);

	HANDLE pipe = CreatePipe();
	int option = 0;
	while (option != 3)
	{
		std::cout << "Choose what do you want\n"
			<< "1. Modify data\n"
			<< "2. Read data\n"
			<< "3. Exit\n";

		std::cin >> option;

		if (option == 1)
		{
			DWORD bytesWriten = 0;
			DWORD bytesRead = 0;
			
			int ID;
			std::cout << "Enter employee ID: ";
			std::cin >> ID;
			
			int msg = ID * 10 + option;
			bool writen = WriteFile(pipe, &msg, sizeof(msg), &bytesWriten, NULL);
			if (writen)
			{
				std::cout << "Message was sent\n";
			}
			else
			{
				std::cout << "Message wasn`t sent\n";
			}

			Employee* emp = new Employee();
			bool read = ReadFile(pipe, emp, sizeof(Employee), &bytesRead, NULL);

			ChangeEmployee(emp);
			
			writen = WriteFile(pipe, emp, sizeof(Employee), &bytesWriten, NULL);
			if (writen)
			{
				std::cout << "Message was sent\n";
			}
			else
			{
				std::cout << "Message wasn`t sent\n";
			}

			msg = 1;
			WriteFile(pipe, &msg, sizeof(msg), &bytesWriten, NULL);
		}
		else if (option == 2)
		{
			DWORD bytesWritten = 0;
			DWORD bytesRead = 0;
			
			int ID;
			std::cout << "Input an Id of employee: ";
			std::cin >> ID;

			int msg = ID * 10 + option;
			bool writen = WriteFile(pipe, &msg, sizeof(msg), &bytesWritten, NULL);
			if (writen)
			{
				std::cout << "Message was sent\n";
			}
			else
			{
				std::cout << "Message wasn`t sent\n";
			}

			Employee* emp = new Employee();
			bool read = ReadFile(pipe, emp, sizeof(Employee), &bytesRead, NULL);
			if (!read)
			{
				std::cout << "Message wasn`t read\n";
			}

			ReadEmployee(emp);
			msg = 1;
			WriteFile(pipe, &msg, sizeof(msg), &bytesWritten, NULL);
		}
		else if (option == 3)
		{
			DWORD bytesWritten;
			DWORD bytesRead;

			int msg = option;
			bool writen = WriteFile(pipe, &msg, sizeof(msg), &bytesWritten, NULL);
			if (writen)
			{
				std::cout << "Message was sent\n";
			}
			else
			{
				std::cout << "Message wasn`t sent\n";
			}
		}
		else
		{
			std::cout << "Incorrect key, try again\n\n";
		}
	}
	
	DisconnectNamedPipe(pipe);
	CloseHandle(pipe);

	return 0;
}