#if !LIGHTBOX_PI
#define PNG_DEBUG 9
#include <libpng/png.h>
#endif

#include "Program.h"
#include "TextureFace2D.h"
using namespace std;
using namespace lb;

void TextureFace2D::bindData(iSize const& _dims, foreign_vector<uint8_t> const& _data, GLenum _format, int _internalFormat, int _level)
{
	bind();
	LB_GL(glTexParameterf, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_sampling);
	LB_GL(glTexParameterf, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_sampling);
	LB_GL(glTexImage2D, GL_TEXTURE_2D, _level, _internalFormat, _dims.w(), _dims.h(), 0, _format, GL_UNSIGNED_BYTE, _data.data());
	m_dims = _dims;
}

void TextureFace2D::activate(uint _unit) const
{
	LB_GL(glActiveTexture, GL_TEXTURE0 + _unit);
	bind();
	LB_GL(glActiveTexture, GL_TEXTURE0);
}

void TextureFace2D::deactivate(uint _unit) const
{
	LB_GL(glActiveTexture, GL_TEXTURE0 + _unit);
	LB_GL(glBindTexture, GL_TEXTURE_2D, 0);
}

#if !LIGHTBOX_PI
static void readPngAux(png_structp png, png_bytep data, png_size_t size)
{
	(*(std::function<void(uint8_t*, size_t)> const*)png_get_io_ptr(png))(data, size);
}

pair<iSize, foreign_vector<uint8_t> > lb::readPng(std::function<void(uint8_t*, size_t)> const& _read)
{
	pair<iSize, foreign_vector<uint8_t> > ret;

	const unsigned c_headerSize = 8;
	png_byte* buffer = new png_byte[c_headerSize];
	_read(buffer, c_headerSize);
	int is_png = !png_sig_cmp(buffer, 0, c_headerSize);
	if (!is_png)
	{
		cwarn << "File format is not PNG.";
		return ret;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		cwarn << "Unable to create PNG structure";
		return ret;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		cwarn << "Unable to create png info";
		return ret;
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		cwarn << "Unable to create png end info";
		return ret;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		cwarn << "Error during setjmp";
		return ret;
	}

	png_set_read_fn(png_ptr, (void*)&_read, &readPngAux);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	int bit_depth, color_type;
	png_uint_32 twidth, theight;
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);
	ret.first.setW(twidth);
	ret.first.setH(theight);

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block.
	{
		auto d = new vector<png_byte>(rowbytes * theight);
		ret.second = foreign_vector<uint8_t>(d).tied(shared_ptr<vector<png_byte> >(d));
	}
	png_byte* image_data = ret.second.data();
	if (!image_data)
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		cwarn << "Unable to allocate image_data while loading.";
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[theight];
	if (!row_pointers)
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		cwarn << "Unable to allocate row_pointer while loading.";
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (unsigned i = 0; i < theight; ++i)
		row_pointers[theight - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] row_pointers;

	return ret;
}
#endif
