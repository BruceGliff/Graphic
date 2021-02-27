FLAGS	:= -std=c++17 -ffast-math -Wall -Wextra -ljpeg -pthread
# geometry/SSE_specialization/vector_base4.cc

all:
	g++ -o launch main.cpp window/window_base.cc object/object_base.cc geometry/AdvancedGeometry.cc outContext/JPG_CR.cc -O3 -march=native $(FLAGS) $(LI-WpedanticBS)

test:
	g++ -o ltest test.cc window/window_base.cc object/object_base.cc geometry/AdvancedGeometry.cc -O3 -march=native $(FLAGS) $(LI-WpedanticBS)
clean: 
	rm launch
