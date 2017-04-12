AM_CPPFLAGS = $(CXX11_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(NODEBUG_CXXFLAGS)

noinst_LIBRARIES = libggutil.a

SUFFIXES = .asm
.asm.o:
	$(AS) $(ASFLAGS) -I$(srcdir)/../asm/ $<

libggutil_a_SOURCES = exception.hh optional.hh
