#include "config.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

config::ConfigVariable_t<std::string> string{"string", std::string{}};
config::ConfigVariable_t<std::vector<int>> vector_of_integers{
    "vector_of_integers", {1, 2, 3}};

// Narrowing type conversion
void print_vector(std::vector<int> vector) {
  for (auto &iterator : vector) {
    std::cout << iterator << ", ";
  }
  std::cout << std::endl;
}

int main() {
  config::instance.set_default_path(std::filesystem::current_path());

  // Accessing a stored variable 
  *string = "test_string";
  string.get() = "test_string";

  // Saving config in file
  config::instance.save("test.config");

  // Accessing a stored variable
  string->clear();
  vector_of_integers->clear();

  // Load config from file
  config::instance.load("test.config");

  // Output loaded data
  std::cout << "string: " << *string << std::endl;
  std::cout << "string: " << string.get() << std::endl;
  print_vector(vector_of_integers);
}
