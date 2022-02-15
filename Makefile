CXX = g++
CXXFLAGS = `pkg-config --cflags --libs gstreamer-1.0`  #sudo apt-get install libopencv-dev
LIBS = -lwiringPi

GCAM: GCAM.cpp
  $(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS)
  
