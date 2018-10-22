#include <algorithm>
#include <fstream>
#include <iostream>
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

// Reads a data file containing lines of numbers, storing them in vectors.
// Each line represents an array of integers, with the first number being the
// the quantity of numbers on that line. Because there may be multiple lines
// in a file, read_data returns a two-dimensional vector.
//
// This function will throw exceptions if it cannot successfully read and parse
// the given file.
std::vector<std::vector<int> > read_data(const char* filename) {
  // Try to open the file as a stream from which to read
  std::ifstream in;
  in.open(filename);
  if (!in) {
    throw std::runtime_error("Unable to read file data.txt!");
  }

  // Holds the integer arrays from the file
  std::vector<std::vector<int> > data;
  // Holds the current line being parsed
  std::string line;

  // The while loop reads every line of the file
  while (std::getline(in, line)) {
    // Treat the line string as a stream to simplify parsing
    std::istringstream iss(line);
    // The first number of a line, giving the quantity of numbers on the line
    int nums_on_line;

    // Read and try to parse an integer from the line stream
    iss >> nums_on_line;
    if (!iss) {
      throw std::runtime_error("Unable to read first number in line!");
    }

    // Holds the integers read from the current line
    std::vector<int> line_data;

    // Loop runs nums_on_line times to read all integers on the current line
    for (; nums_on_line > 0; --nums_on_line) {
      // Variable to hold the parsed number
      int num;

      // Read and try to parse an integer from the line stream
      iss >> num;
      if (!iss) {
        throw std::runtime_error("Unable to read number!");
      }

      // Add the parsed number to the line's vector
      line_data.push_back(num);
    }

    // Throws an exception if more numbers were expected but not present
    if (nums_on_line > 0) {
      throw std::runtime_error("Missing numbers on line!");
    }

    // Add the parsed line data to the two-dimensional vector
    data.push_back(line_data);
  }

  return data;
}

// Writes the sorted output to the given file
void write_output(const char* filename, std::vector<std::vector<int> > sorted_data) {
  std::ofstream of;
  of.open(filename);
  if (!of) {
    throw std::runtime_error("Unable to open file to write output!");
  }

  for (unsigned i = 0; i < sorted_data.size(); ++i) {
    const std::vector<int>& line = sorted_data[i];

    for (unsigned j = 0; j < line.size(); ++j) {
      of << line[j] << ' ';
    }

    of << '\n';
  }
}

int main() {
  // Read every number from data.txt
  std::vector<std::vector<int> > data = read_data("data.txt");

  for (unsigned i = 0; i < data.size(); ++i) merge_sort(data[i]);

  // Write the sorted data from data.txt into merge.out
  write_output("merge.out", data);
  std::cout << "Sorted output has been written to merge.out!" << std::endl;

  return 0;
}
