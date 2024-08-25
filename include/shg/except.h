/**
 * \file include/shg/except.h
 * Exception classes, auxiliary functions and macros.
 * \date Created on 22 July 2010.
 */

#ifndef SHG_EXCEPT_H
#define SHG_EXCEPT_H

#include <exception>
#include <iostream>
#include <string>

namespace SHG {

/**
 * \defgroup error_and_exception_handling Error and exception handling
 *
 * The main exception classes, some utility macros and functions.
 *
 * \{
 */

/**
 * Base class of all exception classes defined by SHG. The class is
 * built according to guidelines from <a href =
 * "http://www.boost.org/community/error_handling.html">David Abraham,
 * Error and Exception Handling</a>.
 */
class Exception : public virtual std::exception {
public:
     /**
      * \details After construction, strcmp(what(), "SHG::Exception")
      * == 0.
      */
     Exception() noexcept;
     /**
      * \details If \c what is longer than Exception::maxlen_
      * characters, it is cut to Exception::maxlen_ characters.
      */
     explicit Exception(std::string const& what);
     /**
      * \copydoc Exception(std::string const&)
      */
     explicit Exception(char const* what);
     Exception(Exception const& e);
     ~Exception();
     Exception& operator=(Exception const& e);
     char const* what() const noexcept;
     /**
      * Prints error message to the stream. If \c progname is given
      * and not empty, it is printed followed by a colon and a space.
      * Then \c what() is printed followed by a newline.
      */
     virtual void print(char const* progname = nullptr,
                        std::ostream& f = std::cerr) const;

protected:
     /**
      * Safe version of strncpy. If \e s = nullptr, \e t will be set
      * to "". Otherwise, maximum Exception::maxlen_ characters will
      * be copied and the string will be null-terminated.
      */
     static char* sstrncpy(char* t, char const* s);
     static std::size_t const maxlen_ =
          63; /**< Maximum length of a message. */

private:
     char what_[maxlen_ + 1]; /**< The message. */
};

/**
 * A class for making assertions. \details It should be used by the
 * means of the macros SHG_ASSERT(e). \c what() is always initialized
 * to "assertion failed".
 */
class Assertion : public virtual Exception {
public:
     /**
      * Constructs an assertion with source file name and line number.
      * \param file source file name \param line line number in this
      * file
      */
     Assertion(char const* file, int line);
     /**
      * \details Copy constructor and assignment operator are defined
      * to suppress the g++ warning 11: define a copy constructor and
      * an assignment operator for classes with dynamically allocated
      * memory. No allocation takes place here, but it is better to
      * define them rather than to weaken diagnostics.
      */
     Assertion(Assertion const& a);
     /**
      * \copydoc Assertion(Assertion const&)
      */
     Assertion& operator=(Assertion const& a);
     /**
      * Returns the source file name.
      */
     char const* file() const noexcept;
     /**
      * Returns line number.
      */
     int line() const noexcept;
     /**
      * \details If \c progname is given and not empty, it is printed
      * followed by a colon and a space. Then \c what() is printed. If
      * \c file() is not nullptr and not empty, " in file " file() ",
      * line " line() is printed. Then the newline character is
      * printed. For example, the code
      *
      * \code
      * Assertion("file.c", 10).print("prog");
      * Assertion("file.c", 10).print();
      * Assertion(nullptr, 10).print("prog");
      * Assertion(nullptr, 10).print();
      * \endcode
      * produces the following output:
      * \verbatim
        prog: assertion failed in file file.c, line 10
        assertion failed in file file.c, line 10
        prog: assertion failed
        assertion failed
        \endverbatim
      */
     void print(char const* progname = nullptr,
                std::ostream& f = std::cerr) const;

private:
     char const* file_; /**< Source file name. */
     int line_;         /**< Line number in this file. */
};

/**
 * Throws SHG::Assertion(file, line) if \c e is false.
 */
void Assert(bool e, char const* file, int line);

/**
 * Throws SHG::Assertion(\__FILE__, \__LINE__) if \c e is false. \sa
 * SHG::Assert(bool, char const*, int).
 */
#define SHG_ASSERT(e) SHG::Assert((e), __FILE__, __LINE__)

/**
 * A class for exceptions signalling invalid arguments in function
 * calls. It should be called by means of the macro SHG_VALIDATE(e).
 * \c what() is always initialized to "invalid argument".
 */
class Invalid_argument : public virtual Exception {
public:
     /**
      * Constructs an Invalid_argument with function name.
      */
     explicit Invalid_argument(char const* func);
     /**
      * \copydoc Assertion::Assertion(Assertion const&)
      */
     Invalid_argument(Invalid_argument const& e);
     /**
      * \copydoc Assertion::Assertion(Assertion const&)
      */
     Invalid_argument& operator=(Invalid_argument const& e);
     /**
      * Returns function name where exception happened.
      */
     char const* func() const noexcept;
     /**
      * \details If \c progname is given and not empty, it is printed
      * followed by a colon and a space. Then \c what() is printed. If
      * \c func() is not nullptr and not empty, " in function " func()
      * is printed. Then the newline character is printed.
      */
     void print(char const* progname = nullptr,
                std::ostream& f = std::cerr) const;

private:
     char const* func_; /**< Function name. */
};

/**
 * Throws SHG::Invalid_argument(func) if \c e is false.
 */
void validate(bool e, char const* func);

/**
 * Throws SHG::Invalid_argument(\__func__) if \c e is false. \sa
 * SHG::validate(bool, char const*).
 */
#define SHG_VALIDATE(e) SHG::validate((e), __func__)

/**
 * Prints error message to the stream. The function prints \c progname
 * followed by a colon and a space if \c progname is given and not
 * empty. Then the message is printed, but if it is nullptr or empty,
 * the word \c error is printed. The function is intended to use with
 * STL exceptions, which have the \c what().
 */
void error(char const* message, char const* progname = 0,
           std::ostream& f = std::cerr);

/**
 * Formats the message and throws the exception. If the message is
 * "division by zero", the file is "division.cc" and line is 121, the
 * message looks like "division.cc(121): division by zero". If the
 * message is nullptr or empty, the word "error" is printed as the
 * message.
 */
template <class T>
[[noreturn]] void throw_exception(char const* file, int line,
                                  char const* message = nullptr) {
     std::string what{file};
     what += "(" + std::to_string(line) + "): ";
     what +=
          message == nullptr || *message == '\0' ? "error" : message;
     throw T(what);
}

#define SHG_THROW(T, message) \
     SHG::throw_exception<T>(__FILE__, __LINE__, (message))

/** \} */  // end of group error_and_exception_handling

}  // namespace SHG

#endif
