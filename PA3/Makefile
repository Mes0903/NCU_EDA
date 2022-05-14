#PA3 cpp compiler, use GCC
ifeq (run,$(firstword $(MAKECMDGOALS)))
RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
$(eval $(RUN_ARGS):;@:)
endif

.PHONY: all run clear
all: 109201547_PA3.o
	@g++ -std=c++11 109201547_PA3.o 109201547_PA3.out
run:
	@./109201547_PA3.out $(RUN_ARGS)
clean:
	@rm *.o
	@rm *.gch
	@rm 109201547_PA3.out
109201547_PA2.o:
	@g++ -std=c++11 -c 109201547_PA3.cpp