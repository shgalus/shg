/**
 * \file src/recjar.cc
 * Record-jar format.
 */

#include <shg/recjar.h>
#include <cassert>
#include <format>
#include <fstream>
#include <sstream>
#include <string>

namespace SHG::RECJAR {

Error::Error(char const* message, char const* file, std::size_t line)
     : std::runtime_error(format_message(message, file, line)) {}

std::string Error::format_message(char const* message,
                                  char const* file,
                                  std::size_t line) {
     std::string s{"error"};
     if (message != nullptr) {
          s += ": ";
          s += message;
     }
     if (file != nullptr) {
          s += ", file: ";
          s += file;
     }
     if (line > 0) {
          s += ", line: ";
          s += std::to_string(line);
     }
     return s;
}

bool operator==(Field const& lhs, Field const& rhs) {
     if (lhs.name != rhs.name)
          return false;
     return lhs.lines == rhs.lines;
}

namespace {

constexpr char const* const ws = " \f\n\r\t\v";

Records read(std::istream& stream, char const* fname) {
     if (!stream.good()) {
          std::string s{fname};
          s += " not found";
          throw Error(s.c_str());
     }

     Records records;
     std::string line;
     std::size_t nlines{0};
     Field field;
     Record record;

     while (std::getline(stream, line)) {
          nlines++;
          auto const pos0 = line.find_first_not_of(ws);
          if (pos0 == std::string::npos)
               continue;
          if (line[pos0] == '#')
               continue;
          if (line[pos0] == '%') {
               if (pos0 + 1 < line.size() && line[pos0 + 1] == '%') {
                    if (!field.name.empty()) {
                         record.push_back(field);
                         field.name.clear();
                         field.lines.clear();
                    }
                    if (!record.empty())
                         records.push_back(record);
                    record.clear();
                    continue;
               }
               throw Error("invalid start of record", fname, nlines);
          }
          if (line[pos0] == '>') {
               if (field.name.empty())
                    throw Error("no preceding field", fname, nlines);
               auto const p0 = line.find_first_not_of(ws, pos0 + 1);
               if (p0 == std::string::npos) {
                    field.lines.push_back("");
               } else {
                    auto const p1 = line.find_last_not_of(ws);
                    assert(p1 != std::string::npos);
                    assert(p1 >= p0);
                    field.lines.push_back(
                         line.substr(p0, p1 - p0 + 1));
               }
               continue;
          }
          if (!field.name.empty()) {
               record.push_back(field);
               field.name.clear();
               field.lines.clear();
          }
          auto const pos1 = line.find_first_of(':', pos0);
          if (pos1 == std::string::npos || pos1 == pos0)
               throw Error("empty field name", fname, nlines);
          assert(pos1 > pos0);
          auto const pos2 = line.find_last_not_of(ws, pos1 - 1);
          assert(pos2 >= pos0);
          field.name = line.substr(pos0, pos2 - pos0 + 1);
          auto const p0 = line.find_first_not_of(ws, pos1 + 1);
          if (p0 == std::string::npos) {
               field.lines.push_back("");
               continue;
          }
          auto const p1 = line.find_last_not_of(ws);
          assert(p1 != std::string::npos);
          assert(p1 >= p0);
          field.lines.push_back(line.substr(p0, p1 - p0 + 1));
     }
     if (!stream.eof() || stream.bad())
          throw Error("error reading stream");
     if (!field.name.empty())
          record.push_back(field);
     if (!record.empty())
          records.push_back(record);
     return records;
}

}  // namespace

Records read(std::filesystem::path const& path) {
     std::ifstream f(path);
     return read(f, path.string().c_str());
}

Records read(char const* str) {
     std::stringstream ss;
     ss << str;
     return read(ss, "string");
}

Records read(std::istream& stream) {
     return read(stream, "stream");
}

void print(Records const& records, std::ostream& stream) {
     bool first{true};
     for (Records::size_type i = 0; i < records.size(); i++) {
          auto const& r = records[i];
          if (first)
               first = false;
          else
               stream << "%%\n";
          for (Record::size_type j = 0; j < r.size(); j++) {
               stream << r[j].name << ':';
               if (r[j].lines.size() > 0)
                    stream << ' ' << r[j].lines[0] << '\n';
               for (std::vector<Line>::size_type k = 1;
                    k < r[j].lines.size(); k++)
                    stream << "> " << r[j].lines[k] << '\n';
          }
     }
}

Headers headers(Records const& records) {
     Headers h;
     for (auto it = records.cbegin(); it != records.cend(); ++it)
          for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2)
               h.insert(it2->name);
     return h;
}

namespace {

constexpr char const* const head{
     R"(<!doctype html>
<html lang="{}">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>{}</title>
<style>
body {{font: 12pt serif; color: black; background: white;
  margin-left: 10%%; margin-right: 10%%;}}
h1, h2, h3, h4, h5, h6 {{color: #900020; background: white;}}
h2.rozm {{font-family: "Comic Sans MS", sans-serif;}}
p.footer {{font-style: italic;}}
span.attention {{font-style: italic; color: red; background: white;}}
div.new {{color: black; background: yellow;}}
div.record {{
    margin: 5px;
    padding: 5px;
    border: 1px solid black;
    border-radius: 5px;
}}
div.field {{
}}
</style>
</head>
<body>
)"};

constexpr char const* const tail{
     R"(</body>
</html>
)"};

/** Replace all occurences of t in s by u. */
std::string replace_all(std::string s, std::string t, std::string u) {
     auto pos = s.find(t);
     while (pos != std::string::npos) {
          s.replace(pos, t.length(), u);
          pos = s.find(t, pos + u.length());
     }
     return s;
}

std::string build_field_text(std::vector<Line> const& lines) {
     std::string s;
     for (auto it = lines.cbegin(); it != lines.cend(); ++it) {
          if (it != lines.cbegin())
               s += ' ';
          s += *it;
     }
     s = replace_all(s, "<", "&lt;");
     s = replace_all(s, ">", "&gt;");
     return s;
}

}  // namespace

void build_html(Records const& records, std::string const& lang,
                std::string const& title,
                std::filesystem::path const& path) {
     std::ofstream f(path);
     if (!f.good()) {
          std::string s{"writing file "};
          s += path.string();
          throw Error(s.c_str());
     }

     f << std::format(head, lang, title);
     for (auto it = records.cbegin(); it != records.cend(); ++it) {
          f << "<div class=\"record\">\n";
          for (auto it2 = it->cbegin(); it2 != it->cend(); ++it2) {
               f << "<div class=\"field\">\n";
               f << "<strong>" << it2->name << "</strong>: ";
               f << build_field_text(it2->lines) << '\n';
               f << "</div>\n";
          }
          f << "</div>\n";
     }
     f << tail;
}

}  // namespace SHG::RECJAR
