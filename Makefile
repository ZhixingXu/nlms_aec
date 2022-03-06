src_lms := ./aec_lms.cpp
src_nlms := ./aec_nlms.cpp


lms:$(src_lms)
	g++ $^ -o lms

nlms:$(src_nlms)
	g++ $^ -o nlms

run_lms:lms
	./lms ./wave/out.wav ./wave/girl.wav ./wave/lms_out.wav

run_nlms:nlms
	./nlms ./wave/out.wav ./wave/girl.wav ./wave/nlms_out.wav
