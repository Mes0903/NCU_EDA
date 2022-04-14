# global variable

+ std::vector<Type_base::Node> List; (in namespace Schedule_Alg)
+ constexpr int UNIT[] = ...; (in namespace Type_base)
+ std::string buf; (in namespace Transform)
+ std::stringstream ss; (in namespace Transform)

g++ -std=c++11 -c Pre_work.h Pre_work.cpp
g++ -std=c++11 -c Schedule_Alg.h Schedule_Alg.cpp
g++ -std=c++11 -c Transform.h Transform.cpp
g++ -std=c++11 -c Type_base.h Type_base.cpp
g++ -std=c++11 -c pre_declared.h 109201547_PA2.cpp
g++ -std=c++11 109201547_PA2.o Pre_work.o Schedule_Alg.o Transform.o Type_base.o -o main.exe
