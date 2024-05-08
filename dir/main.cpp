/*Вариант 3
Представить 2 варианта программы: использование шаблона future и функции async и
использование класса thread.

Реализовать программу, в которой кроме главного создается три отдельных потока:
первый поток сортирует первую половину вещественного массива,
второй поток сортирует вторую половину вещественного массива,
третий поток запускается после завершение первых двух, он сортирует массив
полностью (уже частично отсортированный). Каждый поток имеет свое имя (например,
thread1, thread2, thread3), и печатает отсортированный массив, перед печатью
каждого значения элемента массива поток должен напечатать с новой строки свое
имя. После завершения дочерних потоков главный поток выдает сообщение об
окончании работы. Имена потоков и массив передаются в потоковую функцию через ее
параметры, использовать одну потоковую функцию для всех трех потоков. Массив
предварительно до запуска потоков заполняются числами с использованием ГПСЧ. Для
сортировки использовать метод прямого выбора. 1.	Запустить программу
несколько раз при одних и тех же исходных данных, посмотреть, как меняются
результаты вывода.  Сделать выводы. 2.	Между печатью имени потока и значением
установить небольшую задержку, например, 10 мс. Посмотреть, как меняются
результаты вывода.  Сделать выводы.
*/

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <random>

void sortArray(std::vector<double>& arr, const std::string& threadName,
               const std::string& part) {
  if (part == "first") {
    std::sort(arr.begin(), arr.begin() + arr.size() / 2);
  } else if (part == "second") {
    std::sort(arr.begin() + arr.size() / 2, arr.end());
  } else if (part == "full")
    std::sort(arr.begin(), arr.end());
  for (const auto& num : arr) {
    std::cout << threadName << ": ";
    std::this_thread::sleep_for(
        std::chrono::milliseconds(50));  // Задержка 50 мс
    std::cout << num << std::endl;
  }
}

int main() {
  // Создание и заполнение массива с использованием генератора случайных чисел
  std::vector<double> array(10);

  std::random_device rd;
  std::mt19937_64 g(rd());
  std::uniform_real_distribution<> dis(0.0, 10000.0);
  std::generate(array.begin(), array.end(), [&]() { return dis(g); });

  std::string choose;
  std::cout << "future implementation or will be thread implementation?\n";
  std::cin >> choose;

  if (choose == "future") {
    std::future<void> future1 = std::async(std::launch::async, sortArray,
                                           std::ref(array), "Thread1", "first");
    std::future<void> future2 = std::async(
        std::launch::async, sortArray, std::ref(array), "Thread2", "second");
    future1.get();
    future2.get();

    std::future<void> future3 = std::async(std::launch::async, sortArray,
                                           std::ref(array), "Thread3", "full");
    future3.get();

    std::cout << "All threads have finished sorting the array.\n";

  } else {
    std::thread thread1(sortArray, std::ref(array), "Thread1", "first");
    std::thread thread2(sortArray, std::ref(array), "Thread2", "second");

    thread1.join();
    thread2.join();

    std::thread thread3(sortArray, std::ref(array), "Thread3", "full");
    thread3.join();

    std::cout << "All threads have finished sorting the array.\n";
  }
  return 0;
}
