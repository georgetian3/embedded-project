.ONESHELL:

SXX_KEY = ~/myir.key
SXX_ARGS = -i $(SXX_KEY) -o HostKeyAlgorithms=+ssh-rsa -o PubkeyAcceptedKeyTypes=+ssh-rsa
SXX_PORT = 2222#22#
SXX_HOST = root@59.66.244.24#root@georgetian.com#root@192.168.2.2#root@192.168.1.5#

DEBUG_EXE = audioplayer-x64
XC_EXE = audioplayer-arm

HEADERS = waveheader.h audioplayer.h audioplayer_gui.h stretch.h
SOURCES = main.c audioplayer.c audioplayer_gui.c stretch.c

LIBS = libasound.so.2 libgthread-2.0.so.0 libgtk-3.so.0 libgdk-3.so.0 libXrandr.so.2 libXcursor.so.1 libXext.so.6 libgmodule-2.0.so.0 libpangocairo-1.0.so.0 libX11.so.6 libXi.so.6 libXcomposite.so.1 libXdamage.so.1 libXfixes.so.3 libcairo-gobject.so.2 libcairo.so.2 libgdk_pixbuf-2.0.so.0 libatk-1.0.so.0 libatk-bridge-2.0.so.0 libxkbcommon.so.0 libwayland-cursor.so.0 libwayland-egl.so.1 libwayland-client.so.0 libepoxy.so.0 libfribidi.so.0 libgio-2.0.so.0 libpangoft2-1.0.so.0 libpango-1.0.so.0 libgobject-2.0.so.0 libglib-2.0.so.0 libharfbuzz.so.0 libfontconfig.so.1 libfreetype.so.6 libz.so.1 libffi.so.7 libXrender.so.1 libxcb.so.1 libpixman-1.so.0 libEGL.so libGLESv2.so libpng16.so.16 libxcb-shm.so.0 libxcb-render.so.0 libdbus-1.so.3 libatspi.so.0 libpcre.so.1 libexpat.so.1 libuuid.so.1 libXau.so.6 libXdmcp.so.6 libGAL.so libdrm.so.2 libgbm.so libgbm_viv.so libwayland-server.so.0 libVSC.so libsystemd.so.0 liblzma.so.5 libcap.so.2

ALSA_LIB_VERSION = 1.2.8

DEBUG_WAV = example.wav
XC_WAV = example.wav#test.wav

debug: $(SOURCES)
	gcc `pkg-config --cflags gtk+-3.0` -g $(SOURCES) -lasound -lpthread -o $(DEBUG_EXE) `pkg-config --libs gtk+-3.0`

drun:
	./$(DEBUG_EXE) $(DEBUG_WAV)

alsa:
	wget -nc https://www.alsa-project.org/files/pub/lib/alsa-lib-$(ALSA_LIB_VERSION).tar.bz2
	tar xf alsa-lib-$(ALSA_LIB_VERSION).tar.bz2
	cd alsa-lib-$(ALSA_LIB_VERSION)
	CC=arm-linux-gnueabihf-gcc ./configure --host=armv7l-unknown-linux-gnueabihf
	make
	cp src/.libs/libasound.so.2 ../libasound.so

copy-libs:
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libasound.so.2 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgthread-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgtk-3.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgdk-3.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXrandr.so.2 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXcursor.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXext.so.6 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgmodule-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libpangocairo-1.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libX11.so.6 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXi.so.6 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXcomposite.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXdamage.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXfixes.so.3 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libcairo-gobject.so.2 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libcairo.so.2 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgdk_pixbuf-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libatk-1.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libatk-bridge-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libxkbcommon.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libwayland-cursor.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libwayland-egl.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libwayland-client.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libepoxy.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libfribidi.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgio-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libpangoft2-1.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libpango-1.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgobject-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libglib-2.0.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libharfbuzz.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libfontconfig.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libfreetype.so.6 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libffi.so.7 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXrender.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libxcb.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libpixman-1.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libEGL.so .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libGLESv2.so .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libpng16.so.16 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libxcb-shm.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libxcb-render.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libdbus-1.so.3 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libatspi.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libpcre.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libexpat.so.1 .

	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/lib/libz.so.1 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/lib/libuuid.so.1 .

	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXau.so.6 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libXdmcp.so.6 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libGAL.so .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libdrm.so.2 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgbm.so .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libgbm_viv.so .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libwayland-server.so.0 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/libVSC.so .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/lib/libsystemd.so.0 .

	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib/liblzma.so.5 .
	scp $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/lib/libcap.so.2 .

	scp -r $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/lib .
	scp -r $(SXX_ARGS) -P $(SXX_PORT) $(SXX_HOST):/usr/lib .




	 



xc: $(SOURCES) $(LIBS)
#arm-linux-gnueabihf-gcc $(SOURCES) libasound.so libgthread.so libgtk.so -lpthread -o $(XC_EXE)
	arm-linux-gnueabihf-gcc `pkg-config --cflags gtk+-3.0` $(SOURCES) $(LIBS) -lpthread -lm -o $(XC_EXE) 

ssh:
	ssh $(SXX_ARGS) -p $(SXX_PORT) $(SXX_HOST)

scp: $(XC_EXE)
	scp $(SXX_ARGS) -P $(SXX_PORT) $(XC_EXE) $(SXX_HOST):~

xrun:
	ssh -t $(SXX_ARGS) -p $(SXX_PORT) $(SXX_HOST) "./$(XC_EXE) $(XC_WAV) && exit; bash -l"

clean:
	rm -f audioplayer-*
	rm -rf alsa-lib-*
	rm -f *.out
	rm -f *.exe
	rm -f *.txt
	rm -f *.so