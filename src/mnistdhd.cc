/**
 * \file src/mnistdhd.cc
 * mnistdhd implementation.
 */

#include <shg/neuralnet.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ios>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/endian/conversion.hpp>

namespace SHG::Neural_networks {

Mnistdhd mnistdhd(char const* const path, char const* const kind) {
     using std::ios_base;
     using boost::numeric::ublas::zero_vector;
     using boost::iostreams::input;
     using boost::iostreams::filtering_streambuf;
     using boost::iostreams::gzip_decompressor;
     using boost::endian::load_big_s32;
     using boost::endian::load_big_u32;

     std::string fn;
     std::ifstream f;
     std::stringstream buf(ios_base::in | ios_base::out |
                           ios_base::binary);
     union {
          char c[784];
          unsigned char uc[784];
     } charbuf;
     auto const load = [&buf, &charbuf](std::streamsize n) {
          buf.read(charbuf.c, n);
          if (buf.gcount() != n)
               throw std::runtime_error("error reading file");
     };
     Mnistdhd v;
     if (std::strcmp(kind, "train") == 0)
          v.resize(60000);
     else if (std::strcmp(kind, "t10k") == 0)
          v.resize(10000);
     else
          throw std::invalid_argument("invalid kind");

     {
          // load images
          fn = path;
          fn += kind;
          fn += "-images-idx3-ubyte.gz";
          f.open(fn, ios_base::in | ios_base::binary);
          filtering_streambuf<input> inbuf;
          inbuf.push(gzip_decompressor());
          inbuf.push(f);
          std::istream instream(&inbuf);
          buf << instream.rdbuf();
          f.close();
          load(4);
          if (load_big_s32(charbuf.uc) != 2051)
               throw std::runtime_error("invalid magic number");
          load(4);
          if (load_big_u32(charbuf.uc) != v.size())
               throw std::runtime_error("invalid number of images");
          load(4);
          if (load_big_s32(charbuf.uc) != 28)
               throw std::runtime_error("invalid number of rows");
          load(4);
          if (load_big_s32(charbuf.uc) != 28)
               throw std::runtime_error("invalid number of columns");
          for (Uint i = 0; i < v.size(); i++) {
               load(784);
               for (Uint j = 0; j < 784; j++)
                    v(i).image(j) = charbuf.uc[j];
          }
     }
     {
          // load labels
          fn = path;
          fn += kind;
          fn += "-labels-idx1-ubyte.gz";
          f.open(fn, ios_base::in | ios_base::binary);
          filtering_streambuf<input> inbuf;
          inbuf.push(gzip_decompressor());
          inbuf.push(f);
          std::istream instream(&inbuf);
          buf.str("");
          buf.clear();
          buf << instream.rdbuf();
          f.close();
          load(4);
          if (load_big_s32(charbuf.uc) != 2049)
               throw std::runtime_error("invalid magic number");
          load(4);
          if (load_big_u32(charbuf.uc) != v.size())
               throw std::runtime_error("invalid number of labels");
          for (Uint i = 0; i < v.size(); i++) {
               load(1);
               v(i).label = zero_vector<Real>(v(i).label.size());
               if (charbuf.uc[0] > 9)
                    throw std::runtime_error("invalid label");
               v(i).label(charbuf.uc[0]) = 1.0;
          }
     }
     return v;
}

}  // namespace SHG::Neural_networks
