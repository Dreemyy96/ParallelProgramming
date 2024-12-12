#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <iomanip>
#include <future>

template <typename T>
class Vector {
private:
    size_t n;
    T* data;
    bool is_initialized;

    // Вспомогательная функция для проверки границ
    void check_index(size_t index) const {
        if (index >= n) {
            throw std::out_of_range("Index is out of range");
        }
    }

    template <typename Func>
    T parallel_reduce(Func f, size_t num_threads) const{
        check_initialization();
        if(n == 0){
            return 0;
        }
        std::vector<std::future<T>> futures;
        size_t chunk_size = n / num_threads;

        for (size_t i = 0; i < num_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1) ? n : (i + 1) * chunk_size;
            futures.push_back(std::async(std::launch::async, f, start, end));
        }

        T result = 0;
        for (auto& future : futures) {
            result += future.get();
        }
        return result;
    }

    template <typename Func>
    auto parallel_find_min_max(Func f, size_t num_threads) const {
        check_initialization();
        if(n == 0){
             return std::make_pair(std::make_pair(static_cast<T>(0), static_cast<size_t>(0)), std::make_pair(static_cast<T>(0), static_cast<size_t>(0)));
        }
        std::vector<std::future<std::pair<T, size_t>>> futures;
        size_t chunk_size = n / num_threads;

        for (size_t i = 0; i < num_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1) ? n : (i + 1) * chunk_size;
            futures.push_back(std::async(std::launch::async, f, start, end));
        }
       std::pair<T, size_t> min_result = futures[0].get();
       std::pair<T, size_t> max_result = futures[0].get();
        for(size_t i = 1; i < futures.size(); ++i){
           auto result = futures[i].get();
           if(result.first < min_result.first){
                min_result = result;
           }
           if(result.second > max_result.second){
                max_result = result;
           }
        }

       return std::make_pair(min_result, max_result);
    }
public:
    // Конструктор
    Vector(size_t size) : n(size), data(nullptr), is_initialized(false) {
      if (size > 0) {
          try {
              data = new T[n];
          } catch (const std::bad_alloc& e) {
              std::cerr << "Memory allocation failed: " << e.what() << std::endl;
              throw; // Re-throw the exception to be handled by the caller
          }
      } else {
        throw std::invalid_argument("Vector size must be positive");
      }
    }

    // Деструктор
    ~Vector() {
        delete[] data;
    }

    // Проверка инициализации
    void check_initialization() const {
        if (!is_initialized) {
            throw std::runtime_error("Vector is not initialized");
        }
    }

    // Инициализация константой
    std::chrono::duration<double> initialize(const T& value) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) {
            data[i] = value;
        }
        is_initialized = true;
        auto end = std::chrono::high_resolution_clock::now();
        return end - start;
    }


    // Оператор доступа по индексу (с проверкой границ)
    T& operator[](size_t index) {
        check_initialization();
        check_index(index);
        return data[index];
    }

    const T& operator[](size_t index) const{
        check_initialization();
        check_index(index);
        return data[index];
    }

    // Инициализация случайными числами
    std::chrono::duration<double> initialize_random(T min, T max) {
        auto start = std::chrono::high_resolution_clock::now();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> dist(min, max);

        for (size_t i = 0; i < n; ++i) {
            data[i] = dist(gen);
        }
        is_initialized = true;
        auto end = std::chrono::high_resolution_clock::now();
        return end - start;
    }
        // Экспорт в файл
    std::chrono::duration<double> export_to_file(const std::string& filename) {
        check_initialization();
        auto start = std::chrono::high_resolution_clock::now();
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
           file.write(reinterpret_cast<char*>(data), sizeof(T) * n);
           file.close();
        } else {
           throw std::runtime_error("File can not be opened.");
        }
        auto end = std::chrono::high_resolution_clock::now();
        return end - start;
    }

    // Импорт из файла
    std::chrono::duration<double> import_from_file(const std::string& filename) {
        if (data == nullptr) {
            data = new T[n];
        }
        auto start = std::chrono::high_resolution_clock::now();
        std::ifstream file(filename, std::ios::binary);
        if(file.is_open()){
            file.read(reinterpret_cast<char*>(data), sizeof(T) * n);
            file.close();
            is_initialized = true;
        } else {
           throw std::runtime_error("File can not be opened.");
        }
        auto end = std::chrono::high_resolution_clock::now();
        return end - start;
    }

    // ... (Методы для поиска мин/макс, среднего, суммы, норм и скалярного произведения - см. ниже)
    // Методы поиска минимума и максимума с индексами.
   std::pair<std::pair<T, size_t>, std::pair<T, size_t>> parallel_find_min_max(size_t num_threads) const {

       return parallel_find_min_max([this](size_t start, size_t end) {
            if (start >= end) {
                return std::make_pair(std::numeric_limits<T>::max(), size_t(-1)); // Return placeholder for empty chunk
            }

        T min_val = data[start];
        T max_val = data[start];
        size_t min_index = start;
        size_t max_index = start;

        for (size_t i = start + 1; i < end; ++i) {
            min_val = std::min(min_val, data[i]);
            max_val = std::max(max_val, data[i]);
            min_index = (data[i] < min_val) ? i : min_index;
            max_index = (data[i] > max_val) ? i : max_index;
        }

        return std::make_pair(std::make_pair(min_val, min_index), std::make_pair(max_val, max_index));
        }, num_threads);
    }
     //Параллельная Евклидова норма
    double parallel_euclidean_norm(size_t num_threads) const{
        return std::sqrt(parallel_reduce([this](size_t start, size_t end){
            double local_sum_of_squares = 0;
            for(size_t i = start; i < end; ++i){
                local_sum_of_squares += std::pow(data[i], 2);
            }
            return local_sum_of_squares;
        }, num_threads));
    }

    // Среднее значение (с использованием std::accumulate)
    T average() const{
        check_initialization();
        if (n == 0) {
          throw std::runtime_error("Vector is empty, can't calculate average");
        }
        return std::accumulate(data, data + n, static_cast<T>(0)) / n;
    }

    // Сумма элементов (с использованием std::accumulate)
    T sum() const{
        check_initialization();
        return std::accumulate(data, data + n, static_cast<T>(0));
    }

    T parallel_sum(size_t num_threads) const{

       return parallel_reduce([this](size_t start, size_t end){
           T local_sum = 0;
           for(size_t i = start; i < end; ++i){
               local_sum += data[i];
           }
           return local_sum;
       }, num_threads);
    }
    //Параллельное среднее
    T parallel_average(size_t num_threads) const{
        if(n == 0){
            return 0;
        }
        return parallel_sum(num_threads)/n;
    }
    //Евклидова норма
    double euclidean_norm() const{
        check_initialization();
        double sum_of_squares = 0.0;
        for (size_t i = 0; i < n; ++i) {
            sum_of_squares += std::pow(data[i], 2);
        }
        return std::sqrt(sum_of_squares);
    }
    //Манхеттенская норма
    T manhattan_norm() const{
        check_initialization();
        T sum = 0;
        for (size_t i = 0; i < n; ++i) {
            sum += std::abs(data[i]);
        }
        return sum;
    }
    //Скалярное произведение
    T dot_product(const Vector<T>& other) const{
        check_initialization();
        other.check_initialization();
        if (n != other.n) {
            throw std::invalid_argument("Vectors must have the same size for dot product");
        }

        T result = 0;
        for (size_t i = 0; i < n; ++i) {
            result += data[i] * other.data[i];
        }
        return result;
    }


    //Параллельная Манхеттенская норма
    T parallel_manhattan_norm(size_t num_threads) const{
       return parallel_reduce([this](size_t start, size_t end){
            T local_sum = 0;
            for(size_t i = start; i < end; ++i){
                local_sum += std::abs(data[i]);
            }
            return local_sum;
       }, num_threads);
    }

    //Параллельное Скалярное произведение
     T parallel_dot_product(const Vector<T>& other, size_t num_threads) const{
        check_initialization();
        other.check_initialization();
        if (n != other.n) {
            throw std::invalid_argument("Vectors must have the same size for dot product");
        }
        return parallel_reduce([this, &other](size_t start, size_t end){
            T local_result = 0;
            for (size_t i = start; i < end; ++i) {
                local_result += data[i] * other.data[i];
            }
            return local_result;
        }, num_threads);
    }

    size_t size() const { return n; } // Возвращаем размер вектора


};

int main() {
    try {
        size_t size = 10000000;
        Vector<double> vec(size);
        vec.initialize_random(-10.0, 10.0);

        auto measure_time = [&](const auto& func, const std::string& name) {
            auto start = std::chrono::high_resolution_clock::now();
            auto result = func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << name << " time: " << duration << "ms\n";
            return std::make_pair(duration, result); // Возвращаем пару (время, результат)
        };

        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) {
            num_threads = 2; // Запасной вариант
        }

        std::cout << "Number of threads: " << num_threads << std::endl; // Выводим количество потоков

        // Открываем файл для записи результатов
        std::ofstream output_file("results.txt");
        if (!output_file.is_open()) {
            throw std::runtime_error("Unable to open output file");
        }

        output_file << std::fixed << std::setprecision(6); // Установка точности вывода

        int num_iterations = 5; // Количество итераций для замеров
        for (int i = 0; i < num_iterations; ++i) {
            output_file << "Iteration " << i + 1 << ":\n";

            output_file << "Sequential tests:\n";
            output_file << "Sum: " << measure_time([&]() { return vec.sum(); }, "Sequential sum").first << "\n";
            output_file << "Average: " << measure_time([&]() { return vec.average(); }, "Sequential average").first << "\n";
            Vector<double> vec2 = vec;
            output_file << "Dot product: " << measure_time([&]() { return vec.dot_product(vec2); }, "Sequential dot product").first << "\n";


            output_file << "\nParallel tests with " << num_threads << " threads:\n";
            output_file << "Sum: " << measure_time([&]() { return vec.parallel_sum(num_threads); }, "Parallel sum").first << "\n";
            output_file << "Average: " << measure_time([&]() { return vec.parallel_average(num_threads); }, "Parallel average").first << "\n";
            output_file << "Dot product: " << measure_time([&]() { return vec.parallel_dot_product(vec2, num_threads); }, "Parallel dot product").first << "\n";
            output_file << "\n";
        }
       output_file.close();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}