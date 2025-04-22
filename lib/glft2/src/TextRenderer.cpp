#include "glft2/TextRenderer.hpp"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include FT_FREETYPE_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// All ideas here ripped from a NEHE tutorial
// http://nehe.gamedev.net/tutorial/freetype_fonts_in_opengl/24001/

namespace glft2 {
    // Gets the first power of 2 >= 
    // for the given int  
    inline int next_p2 (int a )
    {
        int rval=1;
        // rval<<=1 Is A Prettier Way Of Writing rval*=2;
        while(rval<a) rval<<=1;
        return rval;
    }

    FT_BitmapGlyph generateBitmapForFace(FT_Face face, char ch)
    {
        // Load the character glyph.
        if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
            throw std::runtime_error("FT_Load_Glyph failed");
     
        // Move into a glyph object
        FT_Glyph glyph;
        if(FT_Get_Glyph( face->glyph, &glyph ))
            throw std::runtime_error("FT_Get_Glyph failed");
     
        // Convert to a bitmap
        FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
        return (FT_BitmapGlyph)glyph;
    }

    void storeTextureData(int const width, 
                          int const height,
                          FT_Bitmap & bitmap,
                          std::vector<GLubyte> & expanded_data)
    {
        // Note: two channel bitmap (One for
        // channel luminosity and one for alpha).
        for(int j = 0; j < height ; j++) {
            for(int i = 0; i < width; i++) {
                expanded_data[2 * (i + j * width)] = 255; // luminosity
                expanded_data[2 * (i + j * width) + 1] =
                (i >= bitmap.width || j >= bitmap.rows) ? 0 :
                bitmap.buffer[i + bitmap.width * j];
            }
        }
    }

    // Create A Display List Corresponding To The Given Character.
    void make_dlist ( FT_Face face, char ch, GLuint list_base, GLuint * tex_base) {
     
        // Move over for next character
        float advance = face->glyph->advance.x >> 6;
        glTranslatef(advance, 0, 0);
        
        // Retrieve a bitmap for the given char glyph.
        FT_BitmapGlyph bitmap_glyph = generateBitmapForFace(face, ch);
     
        // This Reference Will Make Accessing The Bitmap Easier.
        FT_Bitmap& bitmap=bitmap_glyph->bitmap;

        // Get correct dimensions for bitmap
        int width = next_p2( bitmap.width );
        int height = next_p2( bitmap.rows ) ;

        // Use a vector to store texture data (better than a raw array).
        std::vector<GLubyte> expanded_data(2 * width * height, 0);

        // Does what it says
        storeTextureData(width, height, bitmap, expanded_data);

        // Texture parameters.
        glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
         
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // Create the texture Itself
        // GL_LUMINANCE_ALPHA to indicate 2 channel data.
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, &expanded_data.front() );

        // Create the display list
        glNewList(list_base+ch,GL_COMPILE);
     
        glBindTexture(GL_TEXTURE_2D,tex_base[ch]);
     
        glPushMatrix();
     
        // Ensure right amount of space for each new char
        glTranslatef(bitmap_glyph->left,0,0);
        glTranslatef(0,bitmap_glyph->top-(bitmap.rows * 0.9),0);
     
        // Account for padding.
        float x = (float)bitmap.width / (float)width,
              y = (float)bitmap.rows / (float)height;
     
        // Draw the texturemapped quads.
        glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
        glTexCoord2d(0,y); glVertex2f(0,0);
        glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
        glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
        glEnd();
        glPopMatrix();

        // Move over for next character
        glTranslatef(face->glyph->advance.x >> 6 ,0,0);

        // Finish The Display List
        glEndList();
    }

    void font_data::init(const char* fname, unsigned int h) {

        // Allocate Some Memory To Store The Texture Ids.
        textures.resize(128);

        this->h = h;

        // Create And Initialize A FreeType Font Library.
        FT_Library library;
        if (FT_Init_FreeType(&library))
            throw std::runtime_error("FT_Init_FreeType failed");

        // The Object In Which FreeType Holds Information On A Given
        // Font Is Called A "face".
        FT_Face face;

        // This Is Where We Load In The Font Information From The File.
        if (FT_New_Face(library, fname, 0, &face))
            throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

        // FreeType measures font size in 1/64ths of pixels.
        FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

        // Allocate OpenGL resources for textures and display lists.
        list_base = glGenLists(128);
        glGenTextures(128, &textures.front());

        // Create display lists for each of the 128 ASCII characters.
        for (unsigned char i = 0; i < 128; i++) {

            // Load the character glyph for the current ASCII character.
            if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
                // If the character can't be loaded, skip it.
                continue;
            }

            // Calculate the advance for the character in 1/64th of a pixel
            float advance = face->glyph->advance.x >> 6;  // >> 6 is dividing by 64 to get the value in pixels.

            // Move the cursor in OpenGL by the character's advance
            glTranslatef(advance, 0, 0);

            // Save character width (advance in pixels) for future use
            if (i >= 0 && i < 128) {
                char_widths[i] = advance;
            }

            // Now, create the display list for this character.
            make_dlist(face, i, list_base, &textures.front());
        }

        // Free the face resources as they're no longer needed.
        FT_Done_Face(face);

        // Free the FreeType library resources.
        FT_Done_FreeType(library);
    }

    void font_data::clean() {
        glDeleteLists(list_base,128);
        glDeleteTextures(128, &textures.front());
    }

    // A Fairly Straightforward Function That Pushes
    // A Projection Matrix That Will Make Object World
    // Coordinates Identical To Window Coordinates.
    inline void pushScreenCoordinateMatrix() {
        glPushAttrib(GL_TRANSFORM_BIT);
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
        glPopAttrib();
    }
     
    // Pops The Projection Matrix Without Changing The Current
    // MatrixMode.
    inline void pop_projection_matrix() {
        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glPopAttrib();
    }

    // Much Like NeHe's glPrint Function, But Modified To Work
    // With FreeType Fonts.
    void render2D(const font_data &ft_font, float x, float y, std::string const & text)  {
             
        // We Want A Coordinate System Where Distance Is Measured In Window Pixels.
        pushScreenCoordinateMatrix();                                  
             
        GLuint font=ft_font.list_base;
        // We Make The Height A Little Bigger.  There Will Be Some Space Between Lines.
        float h=ft_font.h/.63f;                                                
     
        // Split text into lines
        std::stringstream ss(text);
        std::string to;
        std::vector<std::string> lines;
        while(std::getline(ss,to,'\n')){
            lines.push_back(to);
        }
  
        glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
        glMatrixMode(GL_MODELVIEW);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     
     
        glListBase(font);

        float modelview_matrix[16];    
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
     
        // This Is Where The Text Display Actually Happens.
        // For Each Line Of Text We Reset The Modelview Matrix
        // So That The Line's Text Will Start In The Correct Position.
        // Notice That We Need To Reset The Matrix, Rather Than Just Translating
        // Down By h. This Is Because When Each Character Is
        // Drawn It Modifies The Current Matrix So That The Next Character
        // Will Be Drawn Immediately After It. 
        for(int i=0;i<lines.size();i++) {
            glPushMatrix();
            glLoadIdentity();
            glTranslatef(x,y-h*i,0);
            glMultMatrixf(modelview_matrix);
     
            // The Commented Out Raster Position Stuff Can Be Useful If You Need To
            // Know The Length Of The Text That You Are Creating.
            // If You Decide To Use It Make Sure To Also Uncomment The glBitmap Command
            // In make_dlist().
            // glRasterPos2f(0,0);
            glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
            // float rpos[4];
            // glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
            // float len=x-rpos[0]; (Assuming No Rotations Have Happend)
            glPopMatrix();
        }
     
        glPopAttrib();         
     
        pop_projection_matrix();
    }

	void render3D(const glft2::font_data& ft_font,
		const std::string& text,
		float scale)
	{
		// split into lines
		std::vector<std::string> lines;
		{
			std::stringstream ss(text);
			std::string line;
			while (std::getline(ss, line)) lines.push_back(line);
		}

		glPushAttrib(GL_LIST_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		// use pixel zoom to shrink bitmaps
		glScalef(scale, scale, scale);

		// common state
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);

		glListBase(ft_font.list_base);

		// Render each line of text
		for (size_t i = 0; i < lines.size(); ++i) {
			glCallLists(lines[i].length(),
				GL_UNSIGNED_BYTE,
				lines[i].c_str());

			// Move down for each line. If you want line spacing, you could adjust this, but now it's simple.
			glTranslatef(0, -1.0f, 0); // Adjust this for space between lines
		}

		glPixelZoom(1.0f, 1.0f);

		glPopMatrix();
		glPopAttrib();
	}

    // Measures the pixel extents of a (multi‑line) string
    void measureText(const glft2::font_data& ft_font,
        const std::string& text,
        float* out_width,
        float* out_height,
        float scale)
    {
        std::stringstream ss(text);
        std::string line;
        float maxWidth = 0.0f;
        int numLines = 0;

        while (std::getline(ss, line)) {
            float lineWidth = 0;
            for (unsigned char ch : line) {
                if (ch < 128)
                    lineWidth += ft_font.char_widths[ch];
            }
            maxWidth = std::max(maxWidth, lineWidth);
            ++numLines;
        }

        if (out_width)  *out_width = maxWidth * scale;
        if (out_height) *out_height = numLines * ft_font.h * scale;
    }
}