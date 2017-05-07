AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggutil.a

SUFFIXES = .asm
.asm.o:
	$(AS) $(ASFLAGS) -I$(srcdir)/../asm/ $<

libggutil_a_SOURCES = exception.hh optional.hh
