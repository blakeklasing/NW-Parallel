How to Compile and Run the Programs

Note: The following programs run on Linux. 

Project1 - Lock based vector array
In the command prompt, type 
	g++ -Wall -w -std=c++11 -pthread parallel1.cpp -o main
to create the executable file. Then type the following to run the program. 
	./main
Output: The output will be the length of the strings used, execution time, and the overall alignment score. 

Project2 - Lock-free
In the command prompt, type 
	g++ -Wall -w -std=c++11 -pthread parallel2.cpp -o main
to create the executable file. Then type the following to run the program. 
	./main
Output: The output will be the length of the strings used, execution time, and the overall alignment score.   

Project3 - lock based linked list
In the command prompt, type 
	g++ -Wall -w -std=c++11 -pthread parallel3.cpp -o main
to create the executable file. Then type the following to run the program. 
	./main
Output: The output will be the length of the strings used, execution time, and the overall alignment score. 
