/**
 * \file src/except.cc
 * Exception classes, auxiliary functions and macros.
 * \date Created on 22 July 2010.
 */

#include <shg/except.h>
#include <cstring>

namespace SHG {

Exception::Exception() noexcept {
     sstrncpy(what_, "SHG::Exception");
}

Exception::Exception(std::string const& what)
     : Exception(what.c_str()) {}

Exception::Exception(char const* what) {
     sstrncpy(what_, what);
}

Exception::Exception(Exception const& e) {
     std::strcpy(what_, e.what_);  // NOLINT
}

Exception::~Exception() {}

Exception& Exception::operator=(Exception const& e) {
     std::strcpy(what_, e.what_);  // NOLINT
     return *this;
}

char const* Exception::what() const noexcept {
     return what_;
}

void Exception::print(char const* progname, std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what_ << '\n';
}

char* Exception::sstrncpy(char* t, char const* s) {
     if (s != nullptr) {
          std::strncpy(t, s, maxlen_);
          t[maxlen_] = 0;
     } else {
          *t = 0;
     }
     return t;
}

Assertion::Assertion(char const* file, int line)
     : Exception("assertion failed"), file_(file), line_(line) {}

Assertion::Assertion(Assertion const& a)
     : Exception("assertion failed"),
       file_(a.file_),
       line_(a.line_) {}

Assertion& Assertion::operator=(Assertion const& a) {
     file_ = a.file_;
     line_ = a.line_;
     return *this;
}

char const* Assertion::file() const noexcept {
     return file_;
}

int Assertion::line() const noexcept {
     return line_;
}

void Assertion::print(char const* progname, std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what();
     if (file_ && *file_)
          f << " in file " << file_ << ", line " << line_;
     f << '\n';
}

void Assert(bool e, char const* file, int line) {
     if (!e)
          throw Assertion(file, line);
}

Invalid_argument::Invalid_argument(char const* func)
     : Exception("invalid argument"), func_(func) {}

Invalid_argument::Invalid_argument(Invalid_argument const& e)
     : Exception("invalid argument"), func_(e.func_) {}

Invalid_argument& Invalid_argument::operator=(
     Invalid_argument const& e) {
     func_ = e.func_;
     return *this;
}

char const* Invalid_argument::func() const noexcept {
     return func_;
}

void Invalid_argument::print(char const* progname,
                             std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what();
     if (func_ && *func_)
          f << " in function " << func_;
     f << '\n';
}

void validate(bool e, char const* func) {
     if (!e)
          throw Invalid_argument(func);
}

void error(char const* message, char const* progname,
           std::ostream& f) {
     if (progname && *progname)
          f << progname << ": ";
     f << ((message && *message) ? message : "error") << '\n';
}

}  // namespace SHG
