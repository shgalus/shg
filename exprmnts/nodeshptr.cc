#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>

class Base_node {
public:
     enum class Execution_status {do_execute, do_not_execute};
     
     virtual void execute() const = 0;
     virtual ~Base_node() = default;
};

class Test_case : public Base_node {
public:
     typedef void (*Test_function)();
     Test_case(Test_function test_function,
               const std::string& test_name,
               Execution_status execution_status) : 
          test_function_(test_function),
          test_name_(test_name),
          execution_status_(execution_status) {}
     void execute() const {
          if (execution_status_ == Execution_status::do_execute)
               test_function_();
     }
          
private:
     const Test_function test_function_;
     const std::string test_name_;
     const Execution_status execution_status_;
};

class Test_suite : public Base_node {
public:
     void add(Base_node* bn) {
          contents_.push_back(std::unique_ptr<Base_node>(bn));
     }
     void execute() const {
          for (auto& a : contents_)
               a->execute();
     }
private:
     Execution_status execution_status_ {};
     std::vector<std::unique_ptr<Base_node>> contents_ {};
};

void example1() {
     std::cout << "in example 1\n";
}

void test1() {
     Test_case tc(example1,
                  "example1",
                  Test_case::Execution_status::do_execute);
     tc.execute();
}

void test2() {
     Test_case tc(example1,
                  "example1",
                  Test_case::Execution_status::do_execute);

     Test_suite ts;
     //ts.add(&tc);
     ts.add(new Test_case(example1,
                  "example1",
                          Test_case::Execution_status::do_execute));
     ts.execute();
}

int main() {
     // test1();
     test2();
}
