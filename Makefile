default:
	g++ -std=c++20 -o main main.cpp

clean:
	rm -f main gray.png gray_pixelated.png invImg.png asciiArt.txt
