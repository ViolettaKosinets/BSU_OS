#include <Windows.h>
#include <iostream>
#include <algorithm>

int* arr;
int n;

CRITICAL_SECTION cs;

HANDLE* hThreads;
HANDLE* hThreadsStartEvents;
HANDLE* hThreadWorkStopped;
HANDLE* hThreadsExit;
HANDLE hMutex;

DWORD WINAPI marker(LPVOID thread_index) {
	WaitForSingleObject(hThreadsStartEvents[(int)thread_index], INFINITE);
	int marked_numbers_counter = 0;
	srand((int)thread_index);
	while (true) {
		int random_number = rand() % n;
		if (arr[random_number] == 0) {
			EnterCriticalSection(&cs);
			Sleep(5);
			arr[random_number] = (int)thread_index + 1;
			marked_numbers_counter++;
			Sleep(5);
			LeaveCriticalSection(&cs);
		}
		else {
			EnterCriticalSection(&cs);
			std::cout << "\nNumber of thread = " << (int)thread_index <<
				"\nNumber of marked elements = " << marked_numbers_counter <<
				"\nIndex of not implemented element = " << random_number << "\n";
			LeaveCriticalSection(&cs);
			SetEvent(hThreadWorkStopped[(int)thread_index]);
			ResetEvent(hThreadsStartEvents[(int)thread_index]);
			HANDLE hThreadsStartExit[]{ hThreadsStartEvents[(int)thread_index], hThreadsExit[(int)thread_index] };
			if (WaitForMultipleObjects(2, hThreadsStartExit, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
				EnterCriticalSection(&cs);
				for (size_t i = 0; i < n; i++)
				{
					if (arr[i] == (int)thread_index + 1)
						arr[i] = 0;
				}
				LeaveCriticalSection(&cs);
				ExitThread(NULL);
			}
			else {
				ResetEvent(hThreadWorkStopped[(int)thread_index]);
				continue;
			}
		}
	}


}

void ArrInitialization() {
	std::cout << "Enter size of array: ";
	std::cin >> n;

	arr = new int[n] {0};
}

int setNumOfThreads() {
	int numOfThreads;
	std::cout << "Enter num of threads: ";
	std::cin >> numOfThreads;
	return numOfThreads;
}

void CreateThreads(int numOfThreads, DWORD* dwThreads, HANDLE* hTreadsStartEvents, HANDLE* hThreadWorkStopped, HANDLE* hThreadsExit) {
	for (int i = 0; i < numOfThreads; i++) {
		hThreads[i] = CreateThread(NULL, 1, marker, (LPVOID)i, NULL, &dwThreads[i]);
		hThreadsStartEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		hThreadWorkStopped[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		hThreadsExit[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
}

void SetEvents(int numOfThreads, HANDLE* hTreadsStartEvents) {
	for (int i = 0; i < numOfThreads; i++) {
		SetEvent(hThreadsStartEvents[i]);
	}
}

void ResetEvents(int numOfThreads, bool* hThreadsExited, HANDLE* hThreadWorkStopped, HANDLE* hThreadsStartEvents) {
	for (size_t i = 0; i < numOfThreads; i++)
	{
		if (!hThreadsExited[i]) {
			ResetEvent(hThreadWorkStopped[i]);
			SetEvent(hThreadsStartEvents[i]);
		}
	}
}
void coutArr() {
	for (int i = 0; i < n; i++) {
		std::cout << arr[i] << '\t';
	}

	std::cout << '\n';
}

int setNumOfMarker() {
	int stopMarker;
	std::cout << "Input marker thread number to stop: ";
	std::cin >> stopMarker;
	return stopMarker;
}

int main() {

	ArrInitialization();

	int numOfThreads = setNumOfThreads();
	int numOfTerminatedThreads = 0;

	InitializeCriticalSection(&cs);

	hThreads = new HANDLE[numOfThreads];
	DWORD* dwThreads = new DWORD[numOfThreads];

	hThreadsStartEvents = new HANDLE[numOfThreads];
	hThreadWorkStopped = new HANDLE[numOfThreads];

	hThreadsExit = new HANDLE[numOfThreads];
	bool* hThreadsExited = new bool[numOfThreads] {};

	hMutex = CreateMutex(NULL, FALSE, NULL);

	CreateThreads(numOfThreads, dwThreads, hThreadsStartEvents, hThreadWorkStopped, hThreadsExit);

	SetEvents(numOfThreads, hThreadsStartEvents);


	while (numOfTerminatedThreads < numOfThreads) {

		if (WaitForMultipleObjects(numOfThreads, hThreadWorkStopped, TRUE, INFINITE) == WAIT_FAILED) {
			std::cout << "Wait for multiple objects failed.\n";
		}

		WaitForSingleObject(hMutex, INFINITE);

		coutArr();

		ReleaseMutex(hMutex);

		int stopMarker;
		stopMarker = setNumOfMarker();

		if (!hThreadsExited[stopMarker]) {
			numOfTerminatedThreads++;
			hThreadsExited[stopMarker] = true;
			SetEvent(hThreadsExit[stopMarker]);
			WaitForSingleObject(hThreads[stopMarker], INFINITE);
			CloseHandle(hThreads[stopMarker]);
			CloseHandle(hThreadsExit[stopMarker]);
			CloseHandle(hThreadsStartEvents[stopMarker]);
		}

		hMutex = OpenMutex(NULL, FALSE, NULL);
		WaitForSingleObject(hMutex, INFINITE);

		coutArr();

		ReleaseMutex(hMutex);

		ResetEvents(numOfThreads, hThreadsExited, hThreadWorkStopped, hThreadsStartEvents);
	}

	DeleteCriticalSection(&cs);
	for (int i = 0; i < numOfThreads; i++) {
		CloseHandle(hThreads[i]);
		CloseHandle(hThreadsStartEvents[i]);
		CloseHandle(hThreadWorkStopped[i]);
		CloseHandle(hThreadsExit[i]);
	}

	delete[] arr;
	return 0;
}