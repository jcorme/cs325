#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

// Data structure that represents a store item, with the price and weight properties
struct StoreItem {
  int price;
  int weight;
};

// Data structure that represents a test case, holding the items the store sells and the weights
// each family member can hold
struct TestCase {
  std::vector<StoreItem> items;
  std::vector<int> family_weights;
};

// Helper template function to read a line and parse a value from the given input stream
// Reads an entire line from the istream, turns the line into a stringstream, and attempts to parse
// the line's contents into the given T reference; if the parsing fails, an exception is thrown
// with the err_msg argument
template <class T>
void stream_get_t(std::istream &is, T &t, const char* err_msg) {
  std::string line;
  std::getline(is, line);
  std::istringstream iss(line);
  iss >> t;
  if (!iss) {
    throw std::runtime_error(err_msg);
  }
}

// Reads an individual test case from the input file, parsing it into a TestCase object
TestCase read_test_case(std::ifstream& in) {
  // Read the number of items in the store
  int item_count;
  stream_get_t(in, item_count, "Unable to read the number of items in test case!");

  // Holds the price and weight of every item sold in the store
  std::vector<StoreItem> items;
  std::string line;

  // Read line by line, parsing each store item into a StoreItem object that contains price and
  // weight data members
  for (int i = 0; i < item_count; ++i) {
    if (!std::getline(in, line)) {
      throw std::runtime_error("Failed to read a line from the input file.");
    }

    int price, weight;
    std::istringstream iss(line);

    iss >> price >> weight;

    if (!iss) {
      throw std::runtime_error("Failed to read either the price or weight, or both, of an item.");
    }

    StoreItem item = {price, weight};
    items.push_back(item);
  }

  // Parse the number of family members
  int family_member_count;
  stream_get_t(in, family_member_count, "Failed to read the number of family members in test case.");

  // Holds the maximum weight each family member can have
  std::vector<int> family_weights;

  // Read the maximum weight each family member can have
  for (int i = 0; i < family_member_count; ++i) {
    int max_weight;
    stream_get_t(in, max_weight, "Failed to read the max weight of a family member.");

    family_weights.push_back(max_weight);
  }

  // Create and return a TestCase object from the parsed store items and family weights
  TestCase test_case = {items, family_weights};

  return test_case;
}

// Reads the input file and returns a list of TestCase objects, which allow for easier
// handling of the input data
std::vector<TestCase> read_input(const char* filename) {
  // Open the file for reading
  std::ifstream in;
  in.open(filename);
  if (!in) {
    throw std::runtime_error("Unable to read file shopping.txt!");
  }

  // Store every test case from the input
  std::vector<TestCase> test_cases;
  // Get the first line and parse the integer
  int test_case_count;
  stream_get_t(in, test_case_count, "Failed to read the number of test cases.");

  // Read and parse all the test cases based off the parsed test case count
  for (int i = 0; i < test_case_count; ++i) {
    test_cases.push_back(read_test_case(in));
  }

  return test_cases;
}

// Computes the maximum integer in the given vector (O(n), since it looks through n - 1 elements)
int max_int(const std::vector<int>& family_weights) {
  if (family_weights.size() < 1) {
    return -1;
  }

  int max = family_weights[0];

  for (int i = 1; i < family_weights.size(); ++i) {
    if (family_weights[i] > max) {
      max = family_weights[i];
    }
  }

  return max;
}

// Computes the maximum total price for the family in the test case, along with the specific items
// they each individual family member takes for the given test case.
//
// Returns a std::pair, with the first element being the maximum total price the family received
// in items, and the second element being the vector containing the lists of items purchased by
// each family member.
std::pair<int, std::vector<std::vector<int> > > shopping_spree(const TestCase& test_case) {
  // Array of the maximum amount of weight for each family member
  const std::vector<int>& family_weights = test_case.family_weights;
  // The items the family can take
  const std::vector<StoreItem>& items = test_case.items;
  // Get the largest weight that can be carried by any of the family members; this allows the
  // algorithm to compute the table once for all family members
  int max_family_weight = max_int(family_weights);

  // p[k, w] holds the maximum total price for a given number of items (k) and a maximum weight (w)
  // Initialize the array and reserve space to minimize time spent reallocating
  std::vector<std::vector<int> > p;
  p.reserve(items.size() + 1);
  for (int i = 0; i <= items.size(); ++i) {
    std::vector<int> p_i(max_family_weight + 1);
    p.push_back(p_i);
  }

  // Apply the 0-1 knapsack algorithm
  for (int k = 0; k <= items.size(); ++k) {
    for (int w = 0; w <= max_family_weight; ++w) {
      int item_price = items[k - 1].price;
      int item_weight = items[k - 1].weight;

      // When either is zero, the maximum total price is 0
      if (k == 0 || w == 0) {
        p[k][w] = 0;
      // When this condition is true, the item fits the weight constraints
      } else if (item_weight <= w) {
        // Add the current item if it increases the maximum total price
        if ((item_price + p[k - 1][w - item_weight]) > p[k - 1][w]) {
          p[k][w] = item_price + p[k - 1][w - item_weight];
        // Otherwise, leave it out
        } else {
          p[k][w] = p[k - 1][w];
        }
      // Adding the item break the weight constraints
      } else {
        p[k][w] = p[k - 1][w];
      }
    }
  }

  // Holds the items selected by each family member
  std::vector<std::vector<int> > family_chosen_items;
  // Counter for the total cost of everything taken by the family
  int total_price = 0;

  // Loop through all family members
  for (int i = 0; i < family_weights.size(); ++i) {
    std::vector<int> chosen_items;

    // Loop back and find the items that each family member received based off of weight
    for (int j = items.size(), k = family_weights[i]; j > 0; --j) {
      // Item was added, so push into list of received items
      if (p[j][k] != p[j - 1][k]) {
        chosen_items.push_back(j);
        k = k - items[j - 1].weight;
      }
    }

    family_chosen_items.push_back(chosen_items);

    // Add the total cost of the family member's items based off of their maximum weight
    total_price = total_price + p[items.size()][family_weights[i]];
  }

  return std::make_pair(total_price, family_chosen_items);
}

// Writes the sorted output to the given file
void write_output(const char* filename, std::vector<std::pair<int, std::vector<std::vector<int> > > > shopping_results) {
  // Open file for writing
  std::ofstream of;
  of.open(filename);
  if (!of) {
    throw std::runtime_error("Unable to open file to write output!");
  }

  // Loop through the results for every test case
  for (int i = 0; i < shopping_results.size(); ++i) {
    // Results are returned in a pair<int, vector<vector<int>>>, where the first element is
    // the total price of all items received by the family members, and the second element is
    // a vector of the items they each took
    int total_price = shopping_results[i].first;
    const std::vector<std::vector<int> >& family_items = shopping_results[i].second;

    of << "Test Case " << i + 1 << '\n';
    of << "Total Price " << total_price << '\n';
    of << "Member Items" << '\n';

    // Loop through and print the specific items they each took
    for (int j = 0; j < family_items.size(); ++j) {
      const std::vector<int>& member_items = family_items[j];

      of << '\t' << j + 1 << ":   ";
      for (int k = member_items.size() - 1; k >= 0; --k) {
        of << member_items[k] << ' ';
      }
      of << '\n';
    }
  }
}

int main() {
  // Read the input shopping.txt file into TestCase data structures for easier processing
  std::vector<TestCase> test_cases = read_input("shopping.txt");
  // Hold the results of all the test cases
  std::vector<std::pair<int, std::vector<std::vector<int> > > > results;

  // Perform the shopping spree computation for each test case
  for (int i = 0; i < test_cases.size(); ++i) {
    results.push_back(shopping_spree(test_cases[i]));
  }

  // Write the computed shopping spree results into shopping.out
  write_output("shopping.out", results);
  std::cout << "Shopping spree results have been written to shopping.out!" << std::endl;
  return 0;
}
