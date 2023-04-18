.ONESHELL:

sxx_key = ~/myir.key
sxx_args = -i $(sxx_key) -o HostKeyAlgorithms=+ssh-rsa -o PubkeyAcceptedKeyTypes=+ssh-rsa
sxx_port = 22 #2222
sxx_host = root@192.168.1.5#root@georgetian.com

debug_exe = audioplayer-debug
xc_exe = audioplayer-xc

src_files = main.cpp audioplayer.cpp

compile_flags = -Wno-parentheses
xc_flags = 


alsa_lib_version = 1.2.8

debug_wav = example.wav
xc_wav = test.wav

debug: $(src_files)
	g++ -g $(src_files) $(compile_flags) -lasound -o $(debug_exe)

drun:
	./$(debug_exe) $(debug_wav)

alsa:
	wget -nc https://www.alsa-project.org/files/pub/lib/alsa-lib-$(alsa_lib_version).tar.bz2
	tar xf alsa-lib-$(alsa_lib_version).tar.bz2
	cd alsa-lib-$(alsa_lib_version)
	CC=arm-linux-gnueabihf-gcc ./configure --enable-shared=no --enable-static=yes --host=armv7l-unknown-linux-gnueabihf
	make

xc: $(src_files)
	arm-linux-gnueabihf-g++ $(src_files) $(xc_flags) $(compile_flags) -o $(xc_exe)
	

ssh:
	ssh $(sxx_args) -p $(sxx_port) $(sxx_host)

scp: $(xc_exe)
	scp $(sxx_args) -P $(sxx_port) $(xc_exe) $(sxx_host):~

xrun:
	ssh -t $(sxx_args) -p $(sxx_port) $(sxx_host) "./$(xc_exe) $(xc_wav); bash -l"

clean:
	rm -f audioplayer-*
	rm -rf alsa-lib-*
	rm -f *.out