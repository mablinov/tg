#include <iostream>

#include "TGAImage.h"

void test_read_modify_write() {
	TGAImage image("800x600white.tga");

	for (uint16_t y = 100; y < 400; ++y) {
		for (uint16_t x = 200; x < 600; ++x) {
			image.setPixel(x, y, TGAImage::rgba(255, 255, 255));
		}
	}

	image.write("A_new_image.tga");
}

void test_write_1() {
	TGAImage image(32, 32, TGAImage::rgba(255, 255, 255));
	image.write("32x32newwhite.tga");
}

void test_write_2() {
	TGAImage image(32, 32, TGAImage::rgba(255, 255, 255));

	for (uint16_t y = 10; y < 22; ++y) {
		for (uint16_t x = 10; x < 22; ++x) {
			image.setPixel(x, y, TGAImage::rgba(0, 0, 255));
		}
	}

	image.write("32x32newbluesquare.tga");
}

void test_bottom_left_square() {
	TGAImage image(32, 32);
	for (uint16_t y = 0; y < 16; ++y) {
		for (uint16_t x = 0; x < 16; ++x) {
			image.setPixel(x, y, TGAImage::rgba(255, 0, 0));
		}
	}
	image.write("32x32bottomleft.tga");
}

void test_bottom_left_square_rewrite() {
	TGAImage image("32x32bottomleft.tga");
	image.write("32x32bottomleftrewrite.tga");
}

void test_write_empty() {
	// Irfanview doesn't like this one...
	TGAImage empty;
	empty.write("empty.tga");
}

void test_write_1x1() {
	TGAImage pixel(1, 1, TGAImage::rgba(255, 255, 255));
	pixel.write("pixel.tga");
}

int main()
{
	test_write_2();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
