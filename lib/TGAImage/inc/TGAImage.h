#pragma once

#include <string>
#include <vector>

/*
The alpha value is used in the RGBA colour space to indicate
the transparency of a colour. The alpha value goes from 0 to 1
where 0 is completely transparent and 1 not transparent at all.
*/

class TGAImage {
	template <class T>
	static T extract(const uint8_t *header, size_t index);

	template <class T>
	static void emplace(const uint8_t *header, size_t index, int value);

public:

	enum ImageType {
		NO_IMAGE_DATA = 0,
		UNCOMPRESSED_COLOR_MAPPED = 1,
		UNCOMPRESSED_TRUE_COLOR = 2,
		UNCOMPRESSED_GRAYSCALE = 3,
		RLE_COLOR_MAPPED = 9,
		RLE_TRUE_COLOR = 10,
		RLE_GRAYSCALE = 11
	};

	struct Header {
		uint8_t id_length;
		uint8_t color_map_type;
		uint8_t image_type;

		struct {
			uint16_t first_entry;
			uint16_t length;
			uint8_t size;
		} color_map;

		struct {
			uint16_t origin_x;
			uint16_t origin_y;

			uint16_t width;
			uint16_t height;

			uint8_t bpp;

			struct {
				uint8_t depth : 4;
				uint8_t dir : 2;
			} alpha;
		} image_spec;

		Header();
		Header(const std::vector<uint8_t> &header);

		std::vector<uint8_t> serialize() const;

		static Header createFromParameters(uint16_t width, uint16_t height);
		static const size_t size;
	};

	struct rgba {
		uint8_t b, g, r, a;

	public:
		rgba();
		rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	};

	TGAImage();
	TGAImage(std::string filename);
	TGAImage(uint16_t width, uint16_t height, rgba fill = rgba(0, 0, 0));

	void write(std::string filename) const;
	void reset();

	int width() const;
	int height() const;

	rgba getPixel(uint16_t x, uint16_t y) const;
	void setPixel(uint16_t x, uint16_t y, rgba pixel);
	void setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
	void setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	const std::vector<rgba> &getPixelData() const;
	const Header &getHeader() const;

	size_t computeOffset(uint16_t x, uint16_t y) const;

private:

	Header header;
	std::vector<rgba> pixel_data;
};

template<class T>
static T TGAImage::extract(const uint8_t * header, size_t index)
{
	return *((T *)&header[index]);
}

template<class T>
static void TGAImage::emplace(const uint8_t * header, size_t index, int value)
{
	*((T *)(&header[index])) = value;
}
