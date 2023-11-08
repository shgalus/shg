/**
 * \file src/album.cc
 * Generating HTML album.
 */

#include <shg/album.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <boost/format.hpp>
#include <shg/tree.h>
#include <shg/utils.h>

namespace SHG::ALBUM {

namespace fs = std::filesystem;

Error::Error() : std::runtime_error("internal error") {}

Error::Error(char const* message) : std::runtime_error(message) {}

namespace {

class Builder {
public:
     Builder(std::filesystem::path const& path,
             std::string const& lang, std::string const& title,
             std::vector<Album_data> const& data);
     Builder(Builder const&) = delete;
     Builder& operator=(Builder const&) = delete;
     void build();

private:
     using Index = std::vector<Album_data>::size_type;

     class Data {
     public:
          Data() = default;
          explicit Data(Index i) : i_(i) {}
          Index idx() const { return i_; }
          std::string to_string() const { return std::to_string(i_); }

     private:
          Index i_{};
     };

     using Tree = SHG::Tree<Data>;

     void build_tree();
     void build_index();
     bool get_next_brother(Tree const& t, Index& idx) const;
     bool get_prev_brother(Tree const& t, Index& idx) const;
     std::string format_link(Index href,
                             std::string const& text) const;
     void build_files();
     void save(fs::path const& p, std::string const& pg);
     std::string head(std::string const& title);

     std::filesystem::path const& path_;
     std::string const& lang_;
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
     std::string const& title_;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
     std::vector<Album_data> const& data_;
     Tree tree_{};
     std::string next_button_{};
     std::string prev_button_{};
     std::string up_button_{};
     std::string toc_button_{};
     std::string loi_button_{};

     static std::string encode(std::string const& s);
     static char const* const head_;
     static char const* const tail_;
     static char const* const tab_en_[];
     static char const* const tab_pl_[];
};

Builder::Builder(std::filesystem::path const& path,
                 std::string const& lang, std::string const& title,
                 std::vector<Album_data> const& data)
     : path_(path), lang_(lang), title_(title), data_(data) {
     if (lang == "en") {
          next_button_ = tab_en_[0];
          prev_button_ = tab_en_[1];
          up_button_ = tab_en_[2];
          toc_button_ = tab_en_[3];
          loi_button_ = tab_en_[4];

     } else if (lang == "pl") {
          next_button_ = tab_pl_[0];
          prev_button_ = tab_pl_[1];
          up_button_ = tab_pl_[2];
          toc_button_ = tab_pl_[3];
          loi_button_ = tab_pl_[4];
     } else {
          throw Error("invalid language");
     }
}

void Builder::build() {
     if (data_.size() == 0)
          return;
     build_tree();
     build_files();
}

void Builder::build_tree() {
     if (data_[0].level != 1)
          throw Error("invalid level in the first record");
     Tree* t = &tree_;
     int level = 0;
     for (Index k = 0; k < data_.size(); k++) {
          int const lev = data_[k].level;
          if (lev <= 0) {
               t->push_back(Data(k));
          } else {
               if (lev == level) {
                    Tree& p = *t->parent();
                    auto nchildren = p.degree();
                    p.push_back(Data(k));
                    t = &p[nchildren];
               } else if (lev < level) {
                    int n = level - lev;
                    for (int i = 0; i < n; i++) {
                         t = t->parent();
                         level--;
                    }
                    Tree& p = *t->parent();
                    auto nchildren = p.degree();
                    p.push_back(Data(k));
                    t = &p[nchildren];
               } else {
                    if (lev - level > 1)
                         throw Error("invalid level");
                    t->push_back(Data(k));
                    t = &(*t)[t->degree() - 1];
                    level++;
               }
          }
     }
}

void Builder::build_index() {
     std::string pg;
     fs::path const p = path_ / "index.html";
     pg += "<hr>\n";
     pg += head(toc_button_);
     pg += "<h1>" + toc_button_ + "</h1>\n";
     pg += "<ol>\n";
     for (Tree::Index i = 0; i < tree_.degree(); i++) {
          pg += "<li>" +
                format_link(tree_[i].data().idx(),
                            data_[tree_[i].data().idx()].title) +
                "</li>\n";
     }
     pg += "</ol>\n";
     pg += "<hr>\n";
     pg += tail_;
     save(p, pg);
}

bool Builder::get_next_brother(Tree const& t, Index& idx) const {
     Tree const* p = t.parent();
     if (p == nullptr)
          return false;
     Tree const& t1 = *p;
     auto nchildren = t1.degree();
     assert(nchildren > 0);
     Index i;
     for (i = 0; i < nchildren; i++)
          if (&(t1[i]) == &t)
               break;
     assert(i < nchildren);
     if (++i >= nchildren)
          return false;
     idx = t1[i].data().idx();
     return true;
}

bool Builder::get_prev_brother(Tree const& t, Index& idx) const {
     Tree const* p = t.parent();
     if (p == nullptr)
          return false;
     Tree const& t1 = *p;
     auto nchildren = t1.degree();
     assert(nchildren > 0);
     Index i;
     for (i = 0; i < nchildren; i++)
          if (&(t1[i]) == &t)
               break;
     assert(i < nchildren);
     if (i == 0)
          return false;
     idx = t1[--i].data().idx();
     return true;
}

std::string Builder::format_link(Index href,
                                 std::string const& text) const {
     return str(boost::format("<a href=\"%|04|.html\">%|s|</a>\n") %
                href % text);
}

void Builder::build_files() {
     using boost::format;
     auto it = tree_.cbegin();
     if (it == tree_.cend())
          return;
     build_index();
     Index idx;
     std::string pg;
     std::string nav;
     for (++it; it != tree_.cend(); ++it) {
          Album_data const& d = data_[it->data().idx()];
          std::string s =
               str(format("%|04|.html") % it->data().idx());
          fs::path const p = path_ / s;
          pg.clear();
          nav.clear();
          pg += head(d.title);
          if (get_next_brother(*it, idx)) {
               nav =
                    next_button_ + format_link(idx, data_[idx].title);
          }
          if (get_prev_brother(*it, idx)) {
               if (!nav.empty())
                    nav += "&emsp;";
               nav +=
                    prev_button_ + format_link(idx, data_[idx].title);
          }
          Tree const* prnt = it->parent();
          assert(prnt != nullptr);
          if (prnt->parent() != nullptr) {
               idx = prnt->data().idx();
               if (!nav.empty())
                    nav += "&emsp;";
               nav += up_button_ + format_link(idx, data_[idx].title);
          }
          if (!nav.empty())
               nav += "&emsp;";
          nav += "<a href=\"index.html\">" + toc_button_ + "</a>\n";
          pg += nav;
          pg += "<hr>\n";
          pg += "<h1>" + data_[it->data().idx()].title + "</h1>\n";
          if (d.level == 0) {
               pg += "<div class=\"image\">";
               if (d.rotation.empty())
                    s = str(format("<img src=\"%1%\" alt=\"%2%\">") %
                            d.image_path % d.title);
               else {
                    s = str(format("<img src=\"%1%\" alt=\"%2%\" "
                                   "style=\"transform:rotate(%3%deg);"
                                   "\">") %
                            d.image_path % d.title % d.rotation);
               }
               pg += s;
               pg += "</div>\n";
               if (!d.text.empty()) {
                    pg += "<div class=\"description\">";
                    pg += encode(d.text);
                    pg += "</div>\n";
               }
          } else {
               pg += "<ol>\n";
               for (Tree::Index i = 0; i < it->degree(); i++) {
                    idx = (*it)[i].data().idx();
                    pg += "<li>" +
                          format_link(idx, data_[idx].title) +
                          "</li>\n";
               }
               pg += "</ol>\n";
          }
          pg += "<hr>\n";
          pg += nav;
          pg += tail_;
          save(p, pg);
     }
}

void Builder::save(fs::path const& p, std::string const& pg) {
     std::ofstream f(p, std::ios_base::out | std::ios_base::binary);
     f.write(pg.c_str(), pg.size());
     if (!f)
          throw Error("error writing file");
}

std::string Builder::head(std::string const& title) {
     return str(boost::format(head_) % lang_ % title);
}

std::string Builder::encode(std::string const& s) {
     std::string t;
     t.reserve(s.size());
     for (auto it = s.cbegin(); it != s.cend(); ++it) {
          switch (*it) {
          case '<':
               t.append("&lt;");
               break;
          case '>':
               t.append("&gt;");
               break;
          case '&':
               t.append("&amp;");
               break;
          case '\"':
               t.append("&quot;");
               break;
          case '\'':
               t.append("&apos;");
               break;
          default:
               t.append(1, *it);
               break;
          }
     }
     return t;
}

char const* const Builder::head_{
     R"(<!doctype html>
<html lang="%1%">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>%2%</title>
<style>
body {font: 12pt serif; color: black; background: white;
  margin-left: 10%%; margin-right: 10%%;}
h1, h2, h3, h4, h5, h6 {color: #900020; background: white;}
h2.rozm {font-family: "Comic Sans MS", sans-serif;}
p.footer {font-style: italic;}
span.attention {font-style: italic; color: red; background: white;}
div.new {color: black; background: yellow;}
div.contents {
}
div.image {
/* alternative:
display: grid;
height: 100%%; */
}
div.description {
/* border: 1px solid black; */
/* margin-top: 25px; */
}
.image img {
max-width: 100%%;
object-fit: contain;
/* alternative:
max-width: 100%%;
max-height: 100vh; better: 80vh
margin: auto;*/
}
</style>
</head>
<body>
)"};

char const* const Builder::tail_{
     R"(</body>
</html>
)"};

char const* const Builder::tab_en_[]{
     "Next: ",         "Previous: ", "Up: ", "Table of contents",
     "List of images",
};

char const* const Builder::tab_pl_[]{
     "Nast\304\231pny: ",         "Poprzedni: ",
     "W g\303\263r\304\231: ",    "Spis tre\305\233ci",
     "Wykaz zdj\304\231\304\207",
};

}  // anonymous namespace

bool operator==(Album_data const& lhs, Album_data const& rhs) {
     return lhs.level == rhs.level && lhs.title == rhs.title &&
            lhs.text == rhs.text &&
            lhs.image_path == rhs.image_path &&
            lhs.rotation == rhs.rotation;
}

std::vector<Album_data> convert(std::string const& s) {
     std::vector<std::string> const u =
          split_string(s, std::string("%%"));
     std::vector<Album_data> v(u.size());
     for (std::vector<Album_data>::size_type i = 0; i < v.size();
          i++) {
          auto w = split_string(u[i], std::string("\n*"));
          if (w.size() != 5 && w.size() != 6)
               throw Error("invalid entry");
          for (auto& s : w)
               clean_string(s);
          if (!w[0].empty())
               throw Error("entry mismatch");
          Album_data& d = v[i];
          try {
               d.level = std::stoi(w[1]);
          } catch (std::exception const&) {
               throw Error("invalid level");
          }
          if (d.level < 0)
               throw Error("invalid level");
          d.title = w[2];
          d.text = w[3];
          d.image_path = w[4];
          if (w.size() == 6) {
               double r;
               try {
                    r = std::stod(w[5]);
               } catch (std::exception const&) {
                    throw Error("invalid rotation");
               }
               if (r == 0.0)
                    throw Error("zero rotation");
               d.rotation = w[5];
          }
     }
     return v;
}

void build(std::filesystem::path const& path, std::string const& lang,
           std::string const& title,
           std::vector<Album_data> const& data) {
     Builder(path, lang, title, data).build();
}

}  // namespace SHG::ALBUM
