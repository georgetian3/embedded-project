sxx_args = -o HostKeyAlgorithms=+ssh-rsa
sxx_target = root@192.168.2.2

debug_exe = audioplayer-debug
xc_exe = audioplayer-arm

cpps = main.cpp audioplayer.cpp

compile_flags = -lasound -Wall -Wno-parentheses
xc_flags = -static

debug: $(cpps)
	g++ -g $(cpps) $(compile_flags) -o $(debug_exe)
	./$(debug_exe) example.wav

ssh:
	ssh $(sxx_args) $(sxx_target)

xc: $(cpps)
	aarch64-linux-gnu-g++ $(cpps) $(compile_flags) $(xc_flags) -o $(xc_exe)
	scp $(sxx_args) $(xc_exe) $(sxx_target):~
	ssh -t $(sxx_args) $(sxx_target) "./$(xc_exe) test.wav; bash -l"

clean:
	rm music-*