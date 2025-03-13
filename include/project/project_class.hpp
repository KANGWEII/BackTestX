#ifndef PROJECT_CLASS_HPP
#define PROJECT_CLASS_HPP

namespace project {
class ProjectClass {
 public:
  ProjectClass();

  // Do not allow copy
  ProjectClass(const ProjectClass &) = delete;
  ProjectClass &operator=(const ProjectClass &) = delete;

  void Run();
};
}  // namespace project

#endif /* PROJECT_CLASS_HPP */