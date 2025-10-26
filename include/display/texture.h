#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "errors.h"
#include "glyph.h"

/// \brief Generate a texture from a glyph.
///
/// This returns a buffer of bytes such that each byte represents a greyscale
/// value. Each byte is a pixel. This means that if you use OpenGL to display
/// this texutre, the format should be GL_RED.
ROLFRError build_texture(ROLGlyph *glyph, unsigned char **texture_data);

#endif // __TEXTURE_H__
