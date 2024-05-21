#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <chrono>
#include <random>

// Глобальные переменные для синхронизации
std::mutex mtx;
std::condition_variable cv;
bool firstHalfSorted = false;
bool secondHalfSorted = false;

// Функция для печати массива
void printArray(const std::string &threadName, const std::vector<double> &arr, int start, int end) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Задержка
    mtx.lock();  // Захватываем мьютекс
    for (int i = start; i < end; ++i) {
        std::cout << threadName << ": " << arr[i] << std::endl;
    }
    mtx.unlock();  // Освобождаем мьютекс
}

// Функция сортировки методом прямого выбора
void selectionSort(std::vector<double> &arr, int start, int end, const std::string &threadName) {
    for (int i = start; i < end - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < end; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        std::swap(arr[i], arr[minIndex]);
    }
    printArray(threadName, arr, start, end);
}

// Сортировка первой половины массива
void sortFirstHalf(std::vector<double> &arr) {
    selectionSort(arr, 0, arr.size() / 2, "thread1");
    mtx.lock();  // Захватываем мьютекс
    firstHalfSorted = true;
    mtx.unlock();  // Освобождаем мьютекс
    cv.notify_one();
}

// Сортировка второй половины массива
void sortSecondHalf(std::vector<double> &arr) {
    selectionSort(arr, arr.size() / 2, arr.size(), "thread2");
    mtx.lock();  // Захватываем мьютекс
    secondHalfSorted = true;
    mtx.unlock();  // Освобождаем мьютекс
    cv.notify_one();
}

// Полная сортировка массива после завершения первых двух потоков
void sortFullArray(std::vector<double> &arr) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return firstHalfSorted && secondHalfSorted; });
    lock.unlock(); // Разблокируем, чтобы дать возможность захватить снова в selectionSort
    selectionSort(arr, 0, arr.size(), "thread3");
}

int main() {
    const int n = 30; // Размер массива
    std::vector<double> arr(n);

    // Заполнение массива случайными числами
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 100);

    for (auto &elem : arr) {
        elem = dis(gen);
    }

    // Создание и запуск потоков
    std::thread t1(sortFirstHalf, std::ref(arr));
    std::thread t2(sortSecondHalf, std::ref(arr));
    std::thread t3(sortFullArray, std::ref(arr));

    t1.join();
    t2.join();
    t3.join();

    std::cout << "All threads completed.\n";
    return 0;
}
