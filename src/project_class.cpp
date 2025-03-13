#include "project/project_class.hpp"

#include <iostream>

namespace project {
ProjectClass::ProjectClass() {}

void ProjectClass::Run() { std::cout << "Hello, World!" << std::endl; }
}  // namespace project