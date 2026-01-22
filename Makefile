test: test/main.cpp src/logging.h
	$(CXX) -Isrc test/main.cpp -std=c++20 -o test
