#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <Windows.h>

// Глобальные переменные для потоков min_max и average
std::vector<int> array;
int min_element, max_element;
double average_value;

// Функция потока min_max
void FindMinMax() {
    min_element = array[0];
    max_element = array[0];

    for (int i = 1; i < array.size(); i++) {
        if (array[i] < min_element) {
            min_element = array[i];
        }
        if (array[i] > max_element) {
            max_element = array[i];
        }
        Sleep(7); // "Спать" 7 миллисекунд
    }
    std::cout << "Min element: " << min_element << std::endl;
    std::cout << "Max element: " << max_element << std::endl;
}

// Функция потока average
void CalculateAverage() {
    double sum = 0;
    for (int i = 0; i < array.size(); i++) {
        sum += array[i];
        Sleep(12); // "Спать" 12 миллисекунд
    }
    average_value = sum / array.size();
    std::cout << "Avarege value: " << average_value << std::endl;
}

int main() {
    // Ввод размерности массива и элементов
    int size;
    std::cout << "Enter the size of the array: ";
    std::cin >> size;
    array.resize(size);

    std::cout << "Enter the array elements:\n";
    for (int i = 0; i < size; i++) {
        std::cin >> array[i];
    }

    // Создание потоков min_max и average
    std::thread min_max_thread(FindMinMax);
    std::thread average_thread(CalculateAverage);

    // Ожидание завершения потоков min_max и average
    min_max_thread.join();
    average_thread.join();

    // Замена минимального и максимального элементов на среднее значение
    auto min_max_it = std::minmax_element(array.begin(), array.end());
    //*min_max_it.first = average_value;
    //*min_max_it.second = average_value;

    // Вывод измененного массива
    std::cout << "Modified array:\n";
    for (int i = 0; i < size; i++) {
        if (array[i] == *min_max_it.first || array[i] == *min_max_it.second)std::cout << int(average_value) << ' ';
        else std::cout << array[i] << ' ';
    }
    std::cout << std::endl;

    return 0;
}
