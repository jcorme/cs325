#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

// merge performs the merge operation of the sort.
void merge(std::vector<int>& sublist, int low, int mid, int hi) {
  // Calculate size of the first sublist
  int n1 = (mid - low) + 1;
  // Calculate size of the second sublist
  int n2 = hi - mid;
  // Auxiliary vectors representing the two sublists being merged
  std::vector<int> left, right;

  // Reserve enough memory for each vector to prevent reallocation on calls to
  // push_back, which could interfere with the timing numbers
  left.reserve(n1);
  right.reserve(n2);

  // Populate the first auxiliary vector with contents of the first sublist
  for (int i = 0; i < n1; ++i) {
    left.push_back(sublist[i + low]);
  }

  // Populate the second auxiliary vector with contents of the second sublist
  for (int j = 0; j < n2; ++j) {
    right.push_back(sublist[j + mid + 1]);
  }

  // Define counters to keep track of our current position in each sublist
  int i = 0, j = 0;

  // Loops from low to hi (inclusive), replacing each element with the sorted
  // element
  for (int k = low; k <= hi; ++k) {
    // Since left auxiliary array has already been added to the sublist, only
    // elements of right array remain.
    if (i == left.size()) {
      sublist[k] = right[j++];
    // Similar to the case above...right is empty, only left remains.
    } else if (j == right.size()) {
      sublist[k] = left[i++];
    // <= comparison keeps the sort stable.
    } else if (left[i] <= right[j]) {
      sublist[k] = left[i++];
    } else {
      sublist[k] = right[j++];
    }
  }
}

// Helper function for merge_sort, so merge_sort doesn't have to be called
// with three arguments.
// Recursively calls itself to sort and merge the elements.
void __merge_sort(std::vector<int>& nums, int low, int hi) {
  if (low < hi) {
    // Divide the array two
    int mid = (low + hi) / 2;
    // Sort left and right halves
    __merge_sort(nums, low, mid);
    __merge_sort(nums, mid + 1, hi);
    merge(nums, low, mid, hi);
  }
}

// Wrapper function to call __merge_sort
void merge_sort(std::vector<int>& nums) {
  __merge_sort(nums, 0, nums.size() - 1);
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
    // Increment by 100000 elements each iteration
    int n = 100000 * i;
    std::vector<double> times;

    // Run five times for a given array size
    for (int j = 0; j < 5; ++j) {
      std::vector<int> random_numbers = generate_random_numbers(n, rng_engine);
      clock_t time = clock();
      merge_sort(random_numbers);
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
