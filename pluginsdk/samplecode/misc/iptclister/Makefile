CPP = c++
# INCLUDE = -I/usr/include/gcc/darwin/3.1/g++-v3/
# CPP = g++2
# INCLUDE = -I/usr/include/gcc/darwin/2.95.2/g++/
INCLUDE = -I/usr/include/
OPTIONS = 
HEADERS = 

IPTCLister : IPTCLister.o
	   	   $(CPP) -o IPTCLister IPTCLister.o
	   	   
IPTCLister.o : IPTCLister.cpp $(HEADERS)
			  $(CPP) $(INCLUDE) $(OPTIONS) -c IPTCLister.cpp
