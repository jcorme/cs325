#include <iostream>
#include <vector>

int ternary_search(const std::vector<int>& nums, int x, int low, int high) {
  if (low < high) {
    int mid1 = 1 + ((low + high) / 3);
    int mid2 = 2 * (low + high) / 3 + 1;

    if (nums[mid1] == x) {
      return mid1;
    } else if (nums[mid2] == x) {
      return mid2;
    } else if (nums[mid1] > x) {
      return ternary_search(nums, x, low, mid1 - 1);
    } else if (nums[mid2] > x) {
      return ternary_search(nums, x, mid1, mid2 - 1);
    } else {
      return ternary_search(nums, x, mid2, high);
    }
  } else {
    if (low == high && nums[low] == x) {
      return low;
    }

    return -1;
  }
}

int main() {
  //std::vector<int> nums{6, 3, 15, 8, 2, 5, 1, 4, 23, 7};
  std::vector<int> nums{};
  int i = ternary_search(nums, 4, 0, nums.size() - 1);

  std::cout << "FOUND: " << i << '\n';
}
