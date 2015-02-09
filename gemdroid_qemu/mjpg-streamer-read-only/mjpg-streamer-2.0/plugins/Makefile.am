AM_LDFLAGS = -lpthread -ldl
AM_CFLAGS = -DLINUX -D_GNU_SOURCE -Wall

INCLUDES = -I$(top_srcdir)/src

lib_LTLIBRARIES = libinput_uvc.la 
libinput_uvc_la_SOURCES = input_uvc/input_uvc.c \
							input_uvc/v4l2uvc.c \
							input_uvc/jpeg_utils.c \
							input_uvc/dynctrl.c
libinput_uvc_la_LIBADD = -ljpeg

lib_LTLIBRARIES += libinput_gspcav1.la
libinput_gspcav1_la_SOURCES = 	input_gspcav1/spcav4l.c \
								input_gspcav1/spcautils.c \
								input_gspcav1/encoder.c \
								input_gspcav1/huffman.c \
								input_gspcav1/marker.c \
								input_gspcav1/quant.c
libinput_gspcav1_la_LIBADD = 


lib_LTLIBRARIES += liboutput_file.la
liboutput_file_la_SOURCES = output_file/output_file.c
liboutput_file_la_LIBADD =


lib_LTLIBRARIES += liboutput_http.la
liboutput_http_la_SOURCES = output_http/httpd.c \
							output_http/output_http.c
liboutput_http_la_LIBADD =


lib_LTLIBRARIES += libinput_testpicture.la
libinput_testpicture_la_SOURCES = input_testpicture/input_testpicture.c
libinput_testpicture_la_LIBADD =


lib_LTLIBRARIES += liboutput_autofocus.la
liboutput_autofocus_la_SOURCES = output_autofocus/processJPEG_onlyCenter.c \
								 output_autofocus/output_autofocus.c
liboutput_autofocus_la_LIBADD =

libpluginsincludedir = $(includedir)/plugins/
libpluginsinclude_HEADERS = $(top_srcdir)/plugins/*.h

all-am-local: input_testpicture/testpictures.h

# converts multiple JPG files to a single C header file
input_testpicture/testpictures.h: pictures/960x720_1.jpg pictures/640x480_1.jpg pictures/320x240_1.jpg pictures/160x120_1.jpg pictures/160x120_2.jpg pictures/320x240_2.jpg pictures/640x480_2.jpg pictures/960x720_2.jpg
	@echo "/* automatically generated content, do not edit manually!" > testpictures.h.tmp
	@echo " */ " >> testpictures.h.tmp
	@FILES="160x120_1 320x240_1 640x480_1 960x720_1 160x120_2 320x240_2 640x480_2 960x720_2" && \
	  for file in $$FILES; do \
	    echo "converting file: $$file.jpg"; \
	    echo "static const unsigned char PIC_$$file[] = { " >> testpictures.h.tmp; \
	    od -An -v -t x1 pictures/$$file.jpg | sed -e "s/\ \([0-9a-fA-F]*\)/\ 0x\1,/g" >> testpictures.h.tmp; \
	    echo "};" >> testpictures.h.tmp; \
	  done;
	@cat testpictures.h.tmp | tr '\n' 'N' | sed -e "s/,N};/};/g" | tr 'N' '\n' > testpictures.h
	@rm testpictures.h.tmp

pictures/160x120_1.jpg: pictures/960x720_1.jpg
	convert pictures/960x720_1.jpg -resize 160x120! pictures/160x120_1.jpg

pictures/160x120_2.jpg: pictures/960x720_2.jpg
	convert pictures/960x720_2.jpg -resize 160x120! pictures/160x120_2.jpg

pictures/320x240_1.jpg: pictures/960x720_1.jpg
	convert pictures/960x720_1.jpg -resize 320x240! pictures/320x240_1.jpg

pictures/320x240_2.jpg: pictures/960x720_2.jpg
	convert pictures/960x720_2.jpg -resize 320x240! pictures/320x240_2.jpg

pictures/640x480_1.jpg: pictures/960x720_1.jpg
	convert pictures/960x720_1.jpg -resize 640x480! pictures/640x480_1.jpg

pictures/640x480_2.jpg: pictures/960x720_2.jpg
	convert pictures/960x720_2.jpg -resize 640x480! pictures/640x480_2.jpg

