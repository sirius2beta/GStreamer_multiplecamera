CXX = g++
CXXFLAGS = `pkg-config --cflags --libs gstreamer-1.0`
LIBS = -lwiringPi -lmosquitto

GCAM: GCAM.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS)
  
GCAM_BETA: GCAM_beta.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS)
