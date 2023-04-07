sxx_args = -o HostKeyAlgorithms=+ssh-rsa
sxx_target = root@192.168.2.2

debug_exe = music-debug
xc_exe = music-arm

cpps = main.cpp wavereader.cpp

debug: $(cpps)
	g++ -g $(cpps) -o $(debug_exe)
	./$(debug_exe) example.wav

ssh:
	ssh $(sxx_args) $(sxx_target)

xc: $(cpps)
	arm-linux-gnueabihf-g++ $(cpps) -static -o $(xc_exe)
	scp $(sxx_args) $(xc_exe) $(sxx_target):~
	ssh -t $(sxx_args) $(sxx_target) "./$(xc_exe) test.wav; bash -l"

clean:
	rm music-*