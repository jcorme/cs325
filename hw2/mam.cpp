#include <iostream>
#include <tuple>
#include <vector>

std::tuple<int, int> mam(const std::vector<int>& nums, int low, int high) {
  if (low < high) {
    int mid = (low + high) / 2;
    int min1, min2, max1, max2;
    std::tie(min1, max1) = mam(nums, low, mid);
    std::tie(min2, max2) = mam(nums, mid + 1, high);

    int min, max;

    if (min1 <= min2) min = min1;
    else min = min2;

    if (max1 >= max2) max = max1;
    else max = max2;

    return std::tuple<int, int>(min, max);
  } else if (low == high) {
    return std::tuple<int, int>(nums[low], nums[high]);
  } else {
    return std::tuple<int, int>(nums[0], nums[0]);
  }

}

int main() {
  std::vector<int> nums{5, 1, 3};
  int min, max;
  std::tie(min, max) = mam(nums, 0, nums.size() - 1);

  std::cout << "MIN: " << min << '\n'
            << "MAX: " << max << '\n';
}
