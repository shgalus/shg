#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
extern "C" {
#include <mcheck.h>
}
#include "shg/utils.h"

class Test_node {
public:
     enum class Execution_assignment {to_complete, not_to_complete};

     Test_node(const std::string& node_name) : node_name_(node_name) {}
     void execute() {
          do_execute();
     }
     const std::string& node_name() const {
          return node_name_;
     }
     void node_name(const std::string& name) {
          node_name_ = name;
     }
     Execution_assignment execution_assignment() const {
          return execution_assignment_;
     }
     void execution_assignment(Execution_assignment a) {
          execution_assignment_ = a;
     }
     double elapsed_time_in_seconds() const  {
          return elapsed_time_in_seconds_;
     }
     virtual ~Test_node() = default;
protected:
     virtual void do_execute() = 0;

     std::string node_name_ {};
     Execution_assignment execution_assignment_
     {Execution_assignment::to_complete};
     double elapsed_time_in_seconds_ {0.0};
};

class Test_case : public Test_node {
public:
     typedef void (*const Test_function)();
     Test_case(Test_function test_function,
               const std::string& test_name) :
          Test_node(test_name),
          test_function_(test_function) {}

protected:
     void do_execute() override {
          SHG::Timer timer;
          if (execution_assignment_ == Execution_assignment::to_complete)
               test_function_();
          elapsed_time_in_seconds_ = timer.elapsed();
     }

private:
     const Test_function test_function_;
};

class Test_suite : public Test_node {
public:
     Test_suite(const std::string& suite_name) :
          Test_node(suite_name) {}
     void add(Test_node* test_node) {
          // sprawdzac, czy taka nazwa juz jest!!!!!!!!!!!!
          test_nodes_.push_back(test_node);
     }
protected:
     void do_execute() override {
          SHG::Timer timer;
          for (auto& tn : test_nodes_)
               tn->execute();
          elapsed_time_in_seconds_ = timer.elapsed();
     }
     std::vector<Test_node*> test_nodes_ {};
};

class Test_module : public Test_suite {
public:
     Test_module(const std::string& module_name) :
          Test_suite(module_name) {}
     void run(int argc, char* argv[]);
protected:
     void do_execute() override {
          SHG::Timer timer;
          for (auto& tn : test_nodes_)
               tn->execute();
          elapsed_time_in_seconds_ = timer.elapsed();
          std::cout << "finished\n";
     }
};

void Test_module::run(int argc, char* argv[]) {
     if (argc <= 1) {
          for (auto& tn : test_nodes_)
               tn->execution_assignment(Execution_assignment::to_complete);
     } else {
          for (int i = 1; i < argc; i++) {
               const char* const s = argv[i];
               const auto it = std::find_if(
                    test_nodes_.begin(),
                    test_nodes_.end(),
                    [&s](const Test_node* tn) {
                         return tn->node_name() == s;}
                    );
               assert(it != test_nodes_.end());
               (*it)->execution_assignment(Execution_assignment::to_complete);
          }
     }
     execute();
}

double sqr(double x) {
     return x * x;
}

double zz;

void example1() {
     std::cout << "in example 1\n";
     double y = 0.0;
     for (int i = 0; i < 1000000000; i++)  {
          y += sqr(i);
     }
     zz = y;
}

void example2() {
     std::cout << "in example 2\n";
}

void example3() {
     std::cout << "in example 3\n";
}

void test1() {
     Test_case tc(example1, "example1");
     tc.execute();
}

void test2() {
     Test_case tc(example1, "example1");
     Test_suite ts("suite22");
     ts.add(&tc);
     ts.execute();
     std::cout << ts.elapsed_time_in_seconds() << '\n';
}

void test3(int argc, char* argv[]) {
     Test_case tc(example1, "example1");
     Test_suite ts("suite1");
     ts.add(&tc);
     ts.add(new Test_case(example2, "example2"));  // Is not freed!
     Test_module tm("module1");
     tm.add(&ts);
     tm.run(argc, argv);
     std::cout << tm.elapsed_time_in_seconds() << '\n';
}

int main(int argc, char* argv[]) {
     mtrace();
     test1();
     test2();
     test3(argc, argv);
}
