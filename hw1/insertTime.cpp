#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

// Sorts the array of integers using insertion sort
void insertion_sort(std::vector<int>& nums) {
  // Start from 1 since the first element is considered sorted on the first run
  for (unsigned i = 1; i < nums.size(); ++i) {
    // Move element towards the front of the array while it's smaller
    for (unsigned j = i; j > 0 && nums[j - 1] > nums[j]; --j) {
      std::swap(nums[j], nums[j - 1]);
    }
  }
}

// Generates an array with `n` random integers in the range [0, 10000]
std::vector<int> generate_random_numbers(unsigned n, std::mt19937& rng_engine) {
  // uniform_int_distribution range is inclusive
  std::uniform_int_distribution<int> uni(0, 10000);

  std::vector<int> numbers;
  numbers.reserve(n);

  for (unsigned i = 0; i < n; ++i) {
    numbers.push_back(uni(rng_engine));
  }

  return numbers;
}

// Initializes and seeds a std::mt19937 RNG engine to help generate random
// numbers.
// With help from https://kristerw.blogspot.com/2017/05/seeding-stdmt19937-random-number-engine.html
std::mt19937 make_rng_engine() {
  std::mt19937::result_type seed_data[std::mt19937::state_size];
  std::random_device r;
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(r));
  std::seed_seq seed(std::begin(seed_data), std::end(seed_data));
  return std::mt19937(seed);
}

int main() {
  std::mt19937 rng_engine = make_rng_engine();

  // Use ten different array sizes
  for (int i = 1; i <= 10; ++i) {
    // Increment by 10000 elements each iteration
    int n = 10000 * i;
    std::vector<double> times;

    // Run five times for a given array size
    for (int j = 0; j < 5; ++j) {
      std::vector<int> random_numbers = generate_random_numbers(n, rng_engine);
      clock_t time = clock();
      insertion_sort(random_numbers);
      time = clock() - time;
      double seconds = ((double) time) / CLOCKS_PER_SEC;
      times.push_back(seconds);
    }

    std::cout << "n = " << n << ":\n";

    for (int k = 0; k < times.size(); ++k) {
      std::cout << '\t' << times[k] << '\n';
    }

    // Calculate the average running time of the five runs
    double avg = std::accumulate(times.begin(), times.end(), (double) 0.0) / times.size();
    std::cout << "Average: " << avg << '\n' << std::endl;
  }

  std::cout << std::endl;

  return 0;
}
