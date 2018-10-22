#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
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

  for (unsigned i = 0; i < data.size(); ++i) stooge_sort(data[i], 0, data[i].size() - 1);

  // Write the sorted data from data.txt into stooge.out
  write_output("stooge.out", data);
  std::cout << "Sorted output has been written to stooge.out!" << std::endl;

  return 0;
}
