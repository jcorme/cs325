#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

// Implements the stooge sort alogirhtm; low and high are the (inclusive) boundary indices
void stooge_sort(std::vector<int>& data, int low, int high) {
  // Compute the number of elements that need to be sorted (the size of the subarray)
  // Need to add 1 to the difference to count the range [low, high]; otherwise, the high
  // index isn't counted.
  int size = (high - low) + 1;

  // Trivial case if there are only two elements; swaps if the element at the left boundary
  // is smaller than the element at the high boundary
  if (size == 2 && data[low] > data[high]) {
    std::swap(data[low], data[high]);
  // Otherwise, recursively sort the larger array
  } else if (size > 2) {
    // Calculates the index at which two-thirds of the elements are smaller than that at the
    // index; takes the ceiling of (2n/3)
    // Ceiling formula from https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
    int m = ((2 * size) + 2) / 3;
    // Recursively sort the first 2/3s of the array
    stooge_sort(data, low, low + m - 1);
    // Recursively sort the last 1/3 of the array
    stooge_sort(data, high - m + 1, high);
    // Recursively sort the first 2/3s of the array again
    stooge_sort(data, low, low + m - 1);
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
    int n = 400 * i;
    std::vector<double> times;

    // Run five times for a given array size
    for (int j = 0; j < 5; ++j) {
      std::vector<int> random_numbers = generate_random_numbers(n, rng_engine);
      clock_t time = clock();
      stooge_sort(random_numbers, 0, random_numbers.size() - 1);
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
