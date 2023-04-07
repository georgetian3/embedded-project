sxx_args = -o HostKeyAlgorithms=+ssh-rsa
sxx_target = root@192.168.2.2

debug_exe = music-debug
xc_exe = music-arm

debug: wave.cpp
	g++ -g wave.cpp -o $(debug_exe)
	./$(debug_exe) example.wav

ssh:
	ssh $(sxx_args) $(sxx_target)

xc: wave.cpp
	arm-linux-gnueabihf-g++ wave.cpp -static -o $(xc_exe)
	scp $(sxx_args) $(xc_exe) $(sxx_target):~
	ssh -t $(sxx_args) $(sxx_target) "./$(xc_exe) test.wav; bash -l"

clean:
	rm music-*