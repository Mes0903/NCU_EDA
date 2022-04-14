#PA2 cpp compiler, use GCC
.PHONY: all run clear
all: 109201547_PA2.o Pre_work.o Schedule_Alg.o Transform.o Type_base.o
	@g++ -std=c++11 109201547_PA2.o Pre_work.o Schedule_Alg.o Transform.o Type_base.o -o 109201547_PA2.out
run: $(input) $(output)
	@./109201547_PA2.out $(input) $(output)
clean:
	@rm *.o
	@rm *.gch
	@rm 109201547_PA2.out
109201547_PA2.o:
	@g++ -std=c++11 -c pre_declared.h 109201547_PA2.cpp
Pre_work.o:
	@g++ -std=c++11 -c Pre_work.h Pre_work.cpp
Schedule_Alg.o:
	@g++ -std=c++11 -c Schedule_Alg.h Schedule_Alg.cpp
Transform.o:
	@g++ -std=c++11 -c Transform.h Transform.cpp
Type_base.o:
	@g++ -std=c++11 -c Type_base.h Type_base.cpp