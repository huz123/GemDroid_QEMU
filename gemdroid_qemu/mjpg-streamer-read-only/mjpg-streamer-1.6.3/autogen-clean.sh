#!/bin/sh
if [ -f Makefile ]; then
	echo "Making make distclean..."
	make distclean
elif [ -f build/Makefile ]; then
	echo "Making make distclean..."
	rm -rf build
fi
echo "Removing autogenned files..."
rm -rf config.guess config.sub configure install-sh missing mkinstalldirs Makefile.in ltmain.sh stamp-h.in */Makefile.in ltconfig stamp-h config.h.in depcomp config.h.in~ autom4te.cache aclocal.m4 
echo "Done."
