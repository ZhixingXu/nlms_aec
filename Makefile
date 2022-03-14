src_lms := ./aec_lms.cpp
src_nlms := ./aec_nlms.cpp
test := ./test.cpp


lms:$(src_lms)
	g++ $^ -o lms

nlms:$(src_nlms)
	g++ $^ -o nlms

test:$(test)
	g++ $^ -o test

run_lms:lms
	./lms ./wave/mic.wav ./wave/girl.wav ./wave/expect.wav ./wave/lms_out.wav

run_nlms:nlms
	./nlms ./wave/tog.wav ./wave/girl.wav ./wave/nlms_out.wav

run_test:test
	./test ./wave/boy.wav ./wave/girl.wav ./wave/mic.wav ./wave/expect.wav
