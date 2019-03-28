#include "TGAImage.h"

#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>

const size_t TGAImage::Header::size = 18;

TGAImage::Header::Header()
	: id_length(0),
	color_map_type(0),
	image_type(UNCOMPRESSED_TRUE_COLOR),
	color_map({ 0, 0, 0 }),
	image_spec({ 0, 0, 0, 0, 32, {8, 0} })
{

}

TGAImage::Header::Header(const std::vector<uint8_t> &header)
	: id_length(0),
	color_map_type(0),
	image_type(UNCOMPRESSED_TRUE_COLOR),
	color_map({ 0, 0, 0 }),
	image_spec({ 0, 0, 0, 0, 32, {8, 0} })
{
	const uint8_t *raw_header = header.data();
	id_length = extract<uint8_t>(raw_header, 0);
	color_map_type = extract<uint8_t>(raw_header, 1);
	image_type = extract<uint8_t>(raw_header, 2);

	color_map.first_entry = extract<uint16_t>(raw_header, 3);
	color_map.length = extract<uint16_t>(raw_header, 5);
	color_map.size = extract<uint8_t>(raw_header, 7);

	image_spec.origin_x = extract<uint16_t>(raw_header, 8);
	image_spec.origin_y = extract<uint16_t>(raw_header, 10);
	image_spec.width = extract<uint16_t>(raw_header, 12);
	image_spec.height = extract<uint16_t>(raw_header, 14);
	image_spec.bpp = extract<uint8_t>(raw_header, 16);

	// Unpacking alpha channel information:
	// 0bxxAABBBB
	// x: unused
	// A: direction bits
	// B: alpha channel depth
	uint8_t packed_alpha = extract<uint8_t>(raw_header, 17);
	image_spec.alpha.depth = packed_alpha & 0xF;
	image_spec.alpha.dir = (packed_alpha >> 4) & 0x3;
}

std::vector<uint8_t> TGAImage::Header::serialize() const
{
	std::vector<uint8_t> header_buffer;
	header_buffer.resize(18, 0);

	uint8_t *h = header_buffer.data();

	emplace<uint8_t>(h, 0, id_length);
	emplace<uint8_t>(h, 1, color_map_type);
	emplace<uint8_t>(h, 2, image_type);

	emplace<uint16_t>(h, 3, color_map.first_entry);
	emplace<uint16_t>(h, 5, color_map.length);
	emplace<uint8_t>(h, 7, color_map.size);

	emplace<uint16_t>(h, 8, image_spec.origin_x);
	emplace<uint16_t>(h, 10, image_spec.origin_y);
	emplace<uint16_t>(h, 12, image_spec.width);
	emplace<uint16_t>(h, 14, image_spec.height);
	emplace<uint8_t>(h, 16, image_spec.bpp);

	uint8_t packed_alpha = 0;
	packed_alpha = image_spec.alpha.depth;
	packed_alpha |= image_spec.alpha.dir << 4;
	emplace<uint8_t>(h, 17, packed_alpha);

	return header_buffer;
}

TGAImage::Header TGAImage::Header::createFromParameters(uint16_t width, uint16_t height)
{
	Header h;

	h.id_length = 0;

	h.color_map_type = 0;
	h.image_type = UNCOMPRESSED_TRUE_COLOR;
	h.color_map = { 0, 0, 0 };

	h.image_spec = {
		0, 0,
		width, height,
		32,
		{8, 0}
	};

	return h;
}

TGAImage::TGAImage()
	: pixel_data(),
	header()
{
}

TGAImage::TGAImage(std::string filename)
{
	std::ifstream file(filename, std::ios_base::binary);

	if (!file.is_open()) {
		return;
	}

	std::vector<uint8_t> raw_header(Header::size, 0);
	file.read((char *)raw_header.data(), Header::size);
	if (!file.good()) {
		return;
	}

	Header temp_header(raw_header);

	// Skip image ID data
	file.seekg(temp_header.id_length, std::ios_base::cur);
	// Skip color map data
	file.seekg(temp_header.color_map.length, std::ios_base::cur);

	if (!file.good())
		return;

	// Format: B, G, R, A
	// Always integral number of bytes per pixel
	if (temp_header.image_spec.bpp != 32) {
		std::cerr << "TGAImage only supports loading 32-bit uncompressed files\n";
		return;
	}

	int width = temp_header.image_spec.width;
	int height = temp_header.image_spec.height;

	std::vector<rgba> data_buffer;
	data_buffer.resize(width * height, rgba());
	char *data = (char *)data_buffer.data();

	file.read(data, width * height * sizeof(rgba));
	if (!file.good()) {
		std::cerr << "TGAImage: Failed to read image data block, expected size "
			<< width * height << "\n";
		return;
	}

	pixel_data = std::move(data_buffer);
	header = temp_header;
}

TGAImage::TGAImage(uint16_t width, uint16_t height, rgba fill)
{
	size_t size = width * height;
	pixel_data.resize(size, fill);
	header = Header::createFromParameters(width, height);
}

void TGAImage::write(std::string filename) const
{
	std::ofstream ofs(filename, std::ios_base::out | std::ios_base::binary);
	if (!ofs.is_open()) {
		std::cerr << "Failed to open \"" << filename << "\" for writing\n";
		return;
	}
	
	const std::vector<uint8_t> header_buffer = header.serialize();
	ofs.write((char *)header_buffer.data(), header_buffer.size());
	if (!ofs.good()) {
		std::cerr << "Failed to write TGA header to file \"" << filename << "\"\n";
		return;
	}

	ofs.write((char *)pixel_data.data(), pixel_data.size() * sizeof(rgba));
	if (!ofs.good()) {
		std::cerr << "Failed to write TGA image date to file \"" << filename << "\"\n";
		return;
	}
}

void TGAImage::reset()
{
	pixel_data.clear();
	header = Header();
}

TGAImage::rgba TGAImage::getPixel(uint16_t x, uint16_t y) const
{
	int offset = computeOffset(x, y);

	rgba pixel = pixel_data[offset];
	return pixel;
}

void TGAImage::setPixel(uint16_t x, uint16_t y, rgba pixel)
{
	int offset = computeOffset(x, y);

	pixel_data[offset] = pixel;
}

void TGAImage::setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b)
{
	int offset = computeOffset(x, y);

	rgba pixel = rgba(r, g, b);
	pixel_data[offset] = pixel;
}

void TGAImage::setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	int offset = computeOffset(x, y);

	rgba pixel = rgba(r, g, b, a);
	pixel_data[offset] = pixel;
}

const std::vector<TGAImage::rgba> &TGAImage::getPixelData() const
{
	return pixel_data;
}

const TGAImage::Header & TGAImage::getHeader() const
{
	return header;
}

size_t TGAImage::computeOffset(uint16_t x, uint16_t y) const
{
	size_t offset = y * header.image_spec.width + x;
	return offset;
}

TGAImage::rgba::rgba()
	: r(0), g(0), b(0), a(0)
{
}

TGAImage::rgba::rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	: r(red), g(green), b(blue), a(alpha)
{
}

int TGAImage::width() const
{
	return header.image_spec.width;
}

int TGAImage::height() const
{
	return header.image_spec.height;
}
