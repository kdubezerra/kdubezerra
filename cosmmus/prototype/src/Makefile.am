bin_PROGRAMS = prototype
prototype_SOURCES = prototype.cpp

# set the include path found by configure
AM_CPPFLAGS =  $(LIBSDL_CFLAGS) $(all_includes)

# the library search path.
prototype_LDFLAGS = $(all_libraries) $(LIBSDL_RPATH)
prototype_LDADD = $(LIBSDL_LIBS)
