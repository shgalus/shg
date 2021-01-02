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

Exception::Exception(const std::string& what)
     : Exception(what.c_str()) {}

Exception::Exception(const char* what) {
     sstrncpy(what_, what);
}

Exception::Exception(const Exception& e) {
     std::strcpy(what_, e.what_);
}

Exception::~Exception() {}

Exception& Exception::operator=(const Exception& e) {
     std::strcpy(what_, e.what_);
     return *this;
}

const char* Exception::what() const noexcept {
     return what_;
}

void Exception::print(const char* progname, std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what_ << '\n';
}

char* Exception::sstrncpy(char* t, const char* s) {
     if (s != nullptr) {
          std::strncpy(t, s, maxlen_);
          t[maxlen_] = 0;
     } else {
          *t = 0;
     }
     return t;
}

Assertion::Assertion(const char* file, int line)
     : Exception("assertion failed"), file_(file), line_(line) {}

Assertion::Assertion(const Assertion& a)
     : Exception("assertion failed"),
       file_(a.file_),
       line_(a.line_) {}

Assertion& Assertion::operator=(const Assertion& a) {
     file_ = a.file_;
     line_ = a.line_;
     return *this;
}

const char* Assertion::file() const noexcept {
     return file_;
}

int Assertion::line() const noexcept {
     return line_;
}

void Assertion::print(const char* progname, std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what();
     if (file_ && *file_)
          f << " in file " << file_ << ", line " << line_;
     f << '\n';
}

void Assert(bool e, const char* file, int line) {
     if (!e)
          throw Assertion(file, line);
}

Invalid_argument::Invalid_argument(const char* func)
     : Exception("invalid argument"), func_(func) {}

Invalid_argument::Invalid_argument(const Invalid_argument& e)
     : Exception("invalid argument"), func_(e.func_) {}

Invalid_argument& Invalid_argument::operator=(
     const Invalid_argument& e) {
     func_ = e.func_;
     return *this;
}

const char* Invalid_argument::func() const noexcept {
     return func_;
}

void Invalid_argument::print(const char* progname,
                             std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what();
     if (func_ && *func_)
          f << " in function " << func_;
     f << '\n';
}

void validate(bool e, const char* func) {
     if (!e)
          throw Invalid_argument(func);
}

File_error::File_error() : Exception("file error") {
     *filename_ = 0;
}

File_error::File_error(const char* filename) : File_error() {
     this->filename(filename);
}

void File_error::filename(const char* filename) {
     sstrncpy(filename_, filename);
}

const char* File_error::filename() const {
     return filename_;
}

void File_error::print(const char* progname, std::ostream& f) const {
     if (progname && *progname)
          f << progname << ": ";
     f << what();
     if (*filename_)
          f << ": file: " << filename_;
     f << '\n';
}

void error(const char* message, const char* progname,
           std::ostream& f) {
     if (progname && *progname)
          f << progname << ": ";
     f << ((message && *message) ? message : "error") << '\n';
}

}  // namespace SHG
