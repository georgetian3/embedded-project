.ONESHELL:

SXX_KEY = ~/myir.key
SXX_ARGS = -i $(SXX_KEY) -o HostKeyAlgorithms=+ssh-rsa -o PubkeyAcceptedKeyTypes=+ssh-rsa
SXX_PORT = 22#2222#
SXX_HOST = root@192.168.1.5#root@georgetian.com#

DEBUG_EXE = audioplayer-x64
XC_EXE = audioplayer-arm

HEADERS = waveheader.h audioplayer.h
SOURCES = main.c audioplayer.c

#ALSA_LIB_VERSION = 1.2.8

DEBUG_WAV = example.wav
XC_WAV = test.wav

debug: $(SOURCES)
#	gcc -g $(SOURCES) -lasound -o $(DEBUG_EXE)
	gcc -g $(SOURCES) -o $(DEBUG_EXE)

drun:
	./$(DEBUG_EXE) $(DEBUG_WAV)

# alsa:
# 	wget -nc https://www.alsa-project.org/files/pub/lib/alsa-lib-$(ALSA_LIB_VERSION).tar.bz2
# 	tar xf alsa-lib-$(ALSA_LIB_VERSION).tar.bz2
# 	cd alsa-lib-$(ALSA_LIB_VERSION)
# 	CC=arm-linux-gnueabihf-gcc ./configure --host=armv7l-unknown-linux-gnueabihf
# 	make

xc: $(SOURCES) #alsa-lib-$(ALSA_LIB_VERSION)/src/.libs/libasound.so
#	arm-linux-gnueabihf-gcc $(SOURCES) alsa-lib-$(ALSA_LIB_VERSION)/src/.libs/libasound.so -o $(XC_EXE)
	arm-linux-gnueabihf-gcc $(SOURCES) -o $(XC_EXE)

ssh:
	ssh $(SXX_ARGS) -p $(SXX_PORT) $(SXX_HOST)

scp: $(XC_EXE)
	scp $(SXX_ARGS) -P $(SXX_PORT) $(XC_EXE) $(SXX_HOST):~

xrun:
	ssh -t $(SXX_ARGS) -p $(SXX_PORT) $(SXX_HOST) "./$(XC_EXE) $(XC_WAV); bash -l"

clean:
	rm -f audioplayer-*
	rm -rf alsa-lib-*
	rm -f *.out
	rm -f *.exe
	rm -f *.txt