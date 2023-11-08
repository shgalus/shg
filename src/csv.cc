/**
 * \file src/csv.cc
 * CSV format.
 */

#include <shg/csv.h>
#include <stdexcept>

namespace SHG {

CSV_writer::CSV_writer(std::ostream& ostr, char field_separator,
                       char quote_character)
     : field_separator_(field_separator),
       quote_character_(quote_character),
       special_(std::string() + field_separator_ + quote_character_ +
                '\r' + '\n'),
       ostr_(ostr) {
     if (field_separator_ == quote_character_ ||
         field_separator_ == '\r' || field_separator_ == '\n' ||
         quote_character_ == '\r' || quote_character_ == '\n')
          throw std::invalid_argument(__func__);
}

void CSV_writer::write_record(std::vector<std::string> const& rec) {
     if (rec.size() == 0)
          return;
     rec_.clear();
     first_ = true;
     for (auto const& f : rec)
          add_field(f);
     rec_ += "\r\n";
     ostr_.write(rec_.c_str(), rec_.size());
}

void CSV_writer::add_field(std::string const& s) {
     if (first_)
          first_ = false;
     else
          rec_ += field_separator_;
     if (s.find_first_of(special_) == s.npos) {
          rec_ += s;
     } else {
          rec_ += quote_character_;
          for (char c : s) {
               if (c == quote_character_)
                    rec_ += quote_character_;
               rec_ += c;
          }
          rec_ += quote_character_;
     }
}

CSV_reader::CSV_reader(std::istream& istr, char field_separator,
                       char quote_character)
     : field_separator_(field_separator),
       quote_character_(quote_character),
       istr_(istr) {
     if (field_separator_ == quote_character_ ||
         field_separator_ == '\r' || field_separator_ == '\n' ||
         quote_character_ == '\r' || quote_character_ == '\n')
          throw std::invalid_argument(__func__);
}

void CSV_reader::getrec(std::vector<std::string>& rec) {
     inquote_ = false;
     prevquote_ = false;
     fld_.clear();
     rec.clear();

     for (;;) {
          istr_.get(c_);
          if (istr_.eof()) {
               if (!fld_.empty())
                    rec.push_back(fld_);
               break;
          }
          if (c_ == quote_character_) {
               handle_quote();
          } else {
               prevquote_ = false;
               if (c_ == field_separator_) {
                    handle_separator(rec);
               } else if (is_end_of_record()) {
                    rec.push_back(fld_);
                    break;
               } else {
                    fld_ += c_;
               }
          }
     }
}

void CSV_reader::handle_quote() {
     if (inquote_) {
          inquote_ = false;
          prevquote_ = true;
     } else if (prevquote_) {
          fld_ += quote_character_;
          prevquote_ = false;
          inquote_ = true;
     } else {
          inquote_ = true;
     }
}

void CSV_reader::handle_separator(std::vector<std::string>& rec) {
     if (inquote_) {
          fld_ += c_;
     } else {
          rec.push_back(fld_);
          fld_.clear();
     }
}

// Brian W. Kernighan, Rob Pike, Lekcja programowania, str. 133.
bool CSV_reader::is_end_of_record() {
     if (inquote_)
          return false;
     bool const eol = (c_ == '\r' || c_ == '\n');
     if (c_ == '\r') {
          istr_.get(c_);
          if (!istr_.eof() && c_ != '\n')
               istr_.putback(c_);
     }
     return eol;
}

}  // namespace SHG
