CC = g++
src_nlms := ./aec_nlms.cpp
EXE = ./nlms
LINK = -IInclude -L./Lib -lwav 


.PHONY: run
run:$(EXE) 
	LD_LIBRARY_PATH=./Lib ./$^ ./wave/far_end.wav ./wave/echo.wav ./wave/mic_with_echo.wav ./wave/mic_remove_echo.wav

$(EXE):$(src_nlms)
	$(CC) $^ -o $@ $(LINK)

clean:
	rm $(EXE)
