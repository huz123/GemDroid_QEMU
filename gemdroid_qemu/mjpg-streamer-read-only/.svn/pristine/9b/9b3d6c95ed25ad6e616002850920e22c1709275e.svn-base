AUTOMAKE_OPTIONS = foreign
#CXXFLAGS +=
#LDFLAGS += -L.libs
LDFLAGS = -L.libs
#INCLUDES = -I$(top_srcdir)/include
ACLOCAL_AMFLAGS = -I m4
#LDFLAGS = -ldl
LDFLAGS += -ldl

bin_PROGRAMS = mjpg_streamer
mjpg_streamer_SOURCES = src/mjpg_streamer.c \
			src/utils.c
mjpg_streamer_LDADD = 

lib_LTLIBRARIES = libinput_uvc.la liboutput_file.la liboutput_http.la libinput_testpicture.la liboutput_autofocus.la libinput_gspcav1.la 

INCLUDES = -I$(top_srcdir)/src/*.h
INCLUDES += -I$(top_srcdir)/plugins/*.h
INCLUDES += -I$(top_srcdir)/plugins/input_uvc/*.h
INCLUDES += -I$(top_srcdir)/plugins/input_gspcav1/*.h
INCLUDES += -I$(top_srcdir)/plugins/input_testpicture/*.h
INCLUDES += -I$(top_srcdir)/plugins/output_autofocus/*.h
INCLUDES += -I$(top_srcdir)/plugins/output_http/*.h

libinput_gspcav1_la_SOURCES = plugins/input_gspcav1/spcav4l.c
				plugins/input_gspcav1/utils.c
				plugins/input_gspcav1/encoder.c
				plugins/input_gspcav1/huffman.c
				plugins/input_gspcav1/marker.c
				plugins/input_gspcav1/quant.c
libinput_gspcav1_la_LIBADD =
#libinput_gspcav1includedir = $(includedir)
#libinput_gspcav1include_HEADERS = $(top_srcdir)/plugins/input_gspcav1/*.h

libinput_uvc_la_SOURCES = plugins/input_uvc/input_uvc.c
			plugins/input_uvc/v4l2uvc.c
			plugins/input_uvc/jpeg_utils.c
			plugins/input_uvc/dynctrl.c
#libinput_uvc_la_LIBADD = -linput_gspcav1
#libinput_uvc_la_LIBADD = $(top_builddir)/.libs/libinput_gspcav1.so
libinput_uvc_la_LIBADD = libinput_gspcav1.la
#libinput_uvcincludedir = $(includedir)
#libinput_uvcinclude_HEADERS = $(top_srcdir)/plugins/input_uvc/v4l2uvc.h


#lib_LTLIBRARIES = liboutput_file.la
liboutput_file_la_SOURCES = plugins/output_file/output_file.c
liboutput_file_la_LIBADD =
#liboutput_fileincludedir = $(includedir)
#liboutput_fileinclude_HEADERS = $(top_srcdir)/plugins/output_file/*.h

#lib_LTLIBRARIES = liboutput_http.la
liboutput_http_la_SOURCES =plugins/output_http/httpd.c
			plugins/output_http/output_http.c
liboutput_http_la_LIBADD =
#liboutput_httpincludedir = $(includedir)
#liboutput_httpinclude_HEADERS = $(top_srcdir)/plugins/output_http/*.h

#lib_LTLIBRARIES = libinput_testpicture.la
libinput_testpicture_la_SOURCES = plugins/input_testpicture/input_testpicture.c
libinput_testpicture_la_LIBADD =
#libinput_testpictureincludedir = $(includedir)
#libinput_testpictureinclude_HEADERS = $(top_srcdir)/plugins/input_testpicture/*.h

#lib_LTLIBRARIES = liboutput_autofocus.la
liboutput_autofocus_la_SOURCES = plugins/output_autofocus/processJPEG_onlyCenter.c
				plugins/output_autofocus/output_autofocus.c
liboutput_autofocus_la_LIBADD =
#liboutput_autofocusincludedir = $(includedir)
#liboutput_autofocusinclude_HEADERS = $(top_srcdir)/plugins/output_autofocus/*.h

#libtinyxmlincludedir = $(includedir)
#libtinyxmlinclude_HEADERS = $(top_srcdir)/include/*.h
