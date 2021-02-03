scripts = check.sh compile.sh

common_cppflags = $(HARDEN_CPPFLAGS)
common_cxxflags = $(OPTIM_CXXFLAGS) $(HARDEN_CXXFLAGS) -I$(top_srcdir)/src
common_ldflags = $(HARDEN_LDFLAGS)

check_PROGRAMS = clexec comp

clexec_SOURCES = clexec.cpp
clexec_CPPFLAGS = $(common_cppflags)
clexec_CXXFLAGS = $(common_cxxflags)
clexec_LDFLAGS = $(common_ldflags)

comp_SOURCES = comp.cpp
comp_CPPFLAGS = $(common_cppflags)
comp_CXXFLAGS = $(common_cxxflags)
comp_LDFLAGS = $(common_ldflags)

compile_files = cgood1.cpp cgood2.cpp cfail1.cpp cfail2.cpp cfail3.cpp \
                cfail4.cpp cfail5.cpp cfail6.cpp cfail7.cpp cfail8.cpp

EXTRA_DIST = $(scripts) $(compile_files)
CLEANFILES = cgood1 cgood2

TESTS = $(scripts)
AM_TESTS_ENVIRONMENT = \
	export CXX="$(CXX)"; \
	export CXXFLAGS="$(common_cppflags) $(common_cxxflags) $(common_ldflags)"; \
	export top_srcdir="$(top_srcdir)"; \
	export top_builddir="$(top_builddir)";
