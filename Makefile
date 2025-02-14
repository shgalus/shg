include ./make.inc

prefix = /usr/local
libdir = $(prefix)/lib
includedir = $(prefix)/include/shg
docdir = $(prefix)/share/doc/shg

VERSION = $(shell sed -n '/SHG_VERSION "/			\
s/^\([^0-9]*\)\([.0-9]*\)\(.*$$\)/\2/ p' include/shg/version.h)

SRCS_TIDY = $(shell find src include/shg tests \( -name '*.cc' -o -name '*.h' -not -name '*-inl.h' \))

SRCS_LIB := $(wildcard src/*.cc)
SRCS_TESTS := $(wildcard tests/*.cc)
SRCS_PLP := $(wildcard plp/*.cc)
OBJS_STATIC := $(patsubst src/%, build/static/%, $(SRCS_LIB:%.cc=%.o))
OBJS_SHARED := $(patsubst src/%, build/shared/%, $(SRCS_LIB:%.cc=%.o))
OBJS_TESTS := $(patsubst tests/%, build/tests/%,	\
  $(SRCS_TESTS:%.cc=%.o))
OBJS_PLP := $(patsubst plp/%, build/plp/%, $(SRCS_PLP:%.cc=%.o))

build/static/%.o : src/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

build/shared/%.o : src/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -fpic $< -o $@

build/tests/%.o : tests/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

build/plp/%.o : plp/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all doc run lint format install uninstall clean spotless

# Building shared library is disabled to speed up compilation time.
# all: lib/libshg.a lib/libshg.so.$(VERSION) bin/tests bin/plp
all: lib/libshg.a bin/tests bin/plp

lib/libshg.a: $(OBJS_STATIC) | lib
	$(AR) $(ARFLAGS) $@ $(OBJS_STATIC)
	$(RANLIB) $@

lib/libshg.so.$(VERSION): $(OBJS_SHARED) | lib
	$(CXX) -shared -Wl,-soname,libshg.so -o $@ $(OBJS_SHARED)
	cd lib && ln -sf libshg.so.$(VERSION) libshg.so

bin/tests: lib/libshg.a $(OBJS_TESTS) | bin
	$(CXX) $(LDFLAGS) $(OBJS_TESTS) $(LOADLIBES) $(LDLIBS) -o $@

bin/plp: lib/libshg.a $(OBJS_PLP) | bin
	$(CXX) $(LDFLAGS) $(OBJS_PLP) $(LOADLIBES) $(LDLIBS) -lboost_program_options -o $@

bin lib build build/static build/shared build/tests build/plp:
	mkdir -p $@


$(OBJS_STATIC): | build/static
$(OBJS_SHARED): | build/shared
$(OBJS_TESTS):  | build/tests
$(OBJS_PLP):    | build/plp

/tmp/basic.swf: data/basic.swf
	cp $< $@

doc:
	cd doc && $(MAKE)

run: all /tmp/basic.swf
	./bin/tests --log_level=message --run_test=*

lint:
	cpplint include/shg/* src/* tests/* extras/valgrind/tree.cc

tidy:
	clang-tidy-19 -extra-arg-before=-xc++ $(SRCS_TIDY) -- -std=c++20 $(INCLUDE)

format:
	clang-format-19 -i `find . -name '*.h' -o -name '*.cc'`

install: lib/libshg.a lib/libshg.so.$(VERSION) doc uninstall
	install -d $(includedir) $(libdir) $(docdir)/html/search
	install -m644 include/shg/*.h $(includedir)
	install -m644 lib/libshg.a lib/libshg.so.$(VERSION) $(libdir)
	install lib/libshg.so.$(VERSION) $(libdir)
	cd $(libdir) && ln -s libshg.so.$(VERSION) libshg.so
	install -m644 doc/html/*.* $(docdir)/html
	install -m644 doc/html/search/*.* $(docdir)/html/search

uninstall:
	rm -rf $(includedir)
	rm -f $(libdir)/libshg.*
	rm -rf $(docdir)

clean:
	rm -rf build
	cd doc && $(MAKE) clean

spotless: clean
	rm -rf bin
	rm -rf lib
	cd doc && $(MAKE) spotless

-include $(OBJS_STATIC:%.o=%.d)
-include $(OBJS_SHARED:%.o=%.d)
-include $(OBJS_TESTS:%.o=%.d)
-include $(OBJS_PLP:%.o=%.d)
