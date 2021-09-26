/*
    \file GLFont.h 

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: GLFont.h 183 2010-07-18 15:20:20Z effer $
 */
#ifndef __GL_FONT_H__
#define __GL_FONT_H__

#include <cvars/glplatform.h>

#include <assert.h>
#include <stdio.h>

#include <cstdarg>
#include <string>
#include <string.h>
#include <GLFW/glfw3.h>

#include <utils/shader.h>
#include <ft2build.h>
#include <freetype/freetype.h>


#define MAX_TEXT_LENGTH 512

///
class GLFont
{
    friend inline bool GLFontCheckInit(GLFont* pFont);
	public:
        GLFont()
        {
            m_nNumLists = 96;
            m_nCharWidth = 8;
            m_nCharHeight = 13;
            m_bInitDone = false;
			Init();
        }

		bool Init() {
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			text_program = std::make_shared<Shader>("text.vs", "text.fs");
			std::vector<Vertex> verts{
				{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec4(1.0, 0.0, 0.0, 0.6)},
				{glm::vec3(1.0, -1.0f, 0.0f), glm::vec4(0.0, 1.0, 0.0, 0.6)},
				{glm::vec3(1.0, 1.0, 0.0f), glm::vec4(0.0, 0.0, 1.0, 0.6)},
				{glm::vec3(-1.0f, 1.0, 0.0f), glm::vec4(0.0, 0.0, 1.0, 0.6)}
			};
			std::vector<unsigned int> index{ 0, 1, 2, 0, 2, 3 };
			std::vector<Texture> texs{
				{tex, 0, "tex"}
			};
			text_quad = std::make_shared<Mesh>(verts, index, tex);

			if (FT_Init_FreeType(&ft)) {
				std::cerr << "Could not init freetype library" << std::endl;
				return false;
			}

			if (FT_New_Face(ft, "fonts/ProggyClean.ttf", 0, &face)) {
				std::cerr << "Could not open font" << std::endl;
				return false;
			}

			FT_Set_Pixel_Sizes(face, 0, 48);
		}
        ~GLFont();        

        // printf style function take position to print to as well
        // NB: coordinates start from bottom left
        void glPrintf(int x, int y, const char *fmt, ...);
        void glPrintf(int x, int y, const std::string fmt, ...){ glPrintf(x,y, fmt.c_str()); }
        void glPrintfFast(int x, int y, const char *fmt, ...);
        void glPrintfFast(int x, int y, const std::string fmt, ...){ glPrintfFast(x,y, fmt.c_str()); }

        unsigned int   CharWidth() { return m_nCharWidth; }
        unsigned int   CharHeight() { return m_nCharHeight; }

    private:
        unsigned int   m_nCharWidth; // fixed width
        unsigned int   m_nCharHeight; // fixed width
        int            m_nNumLists;        // number of display lists
        int            m_nDisplayListBase; // base number for display lists
        bool           m_bInitDone;

private:
	FT_Library ft;
	FT_Face face;
	std::shared_ptr<Shader> text_program;
	std::shared_ptr<Mesh> text_quad;
	GLuint tex = 0;

public:
	void render_text(const char *text, float x, float y, float sx, float sy);
};

void GLFont::render_text(const char *text, float x, float y, float sx, float sy) {
	const char *p = text;
	FT_GlyphSlot g = face->glyph;
	// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (*p++) {
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER)) {
			continue;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			g->bitmap.width, g->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		std::vector<Vertex> box = {
			{glm::vec4(x2,     -y2    , 0, 0)},
			{glm::vec4(x2 + w, -y2    , 1, 0)},
			{glm::vec4(x2,     -y2 - h, 0, 1)},
			{glm::vec4(x2 + w, -y2 - h, 1, 1)},
		};

		//glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		text_quad->updateMesh(box);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		text_program->setVec4("color", glm::vec4(1.0, 0.0, 0.0, 0.0));

		x += (g->advance.x / 64) * sx;
		y += (g->advance.y / 64) * sy;
	}
}

////////////////////////////////////////////////////////////////////////////////
///
inline bool GLFontCheckInit(GLFont* pFont = NULL)
{
    // make sure glfwInit has been called
    if(glfwGetTime() <= 0){
        //fprintf(stderr, "WARNING: GLFontCheckInit failed after 'glutGet(GLUT_ELAPSED_TIME) <= 0' check\n");
        return false;
    }

    static int nDisplayListBase = -1;
    if(!pFont->m_bInitDone) {
        assert(pFont != NULL);
        // GLUT bitmapped fonts...  
        //pFont->m_nDisplayListBase = glGenLists(pFont->m_nNumLists);
        if(pFont->m_nDisplayListBase == 0) {
//    hmm, commented out for now because on my linux box w get here sometimes
//    even though glut hasn't been initialized.
//            fprintf(stderr, "%i", pFont->m_nNumLists);
            fprintf(stderr, "GLFontCheckInit() -- out of display lists\n");
            return false;
        }
        for(int nList = pFont->m_nDisplayListBase; 
                nList < pFont->m_nDisplayListBase + pFont->m_nNumLists; nList++) {
            /*glNewList(nList, GL_COMPILE);
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, nList+32-pFont->m_nDisplayListBase);
            glEndList();*/
        }

        nDisplayListBase = pFont->m_nDisplayListBase;
        pFont->m_bInitDone = true;
        return false;
    }
    else{
        assert(nDisplayListBase > 0);
        pFont->m_nDisplayListBase = nDisplayListBase;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
inline GLFont::~GLFont()
{
    if(m_bInitDone && GLFontCheckInit(this)) {
        //glDeleteLists(m_nDisplayListBase, m_nDisplayListBase + m_nNumLists);
    } 
}
 
////////////////////////////////////////////////////////////////////////////////
// printf style print function
// NB: coordinates start from bottom left
inline void GLFont::glPrintf(int x, int y, const char *fmt, ...)   
{
    GLFontCheckInit(this);

    char        text[MAX_TEXT_LENGTH];                  // Holds Our String
    va_list     ap;                                     // Pointer To List Of Arguments

    if(fmt == NULL) {                                 // If There's No Text
        return;                                         // Do Nothing
    }

    va_start(ap, fmt);                                // Parses The String For Variables
    vsnprintf(text, MAX_TEXT_LENGTH, fmt, ap);         // And Converts Symbols To Actual Numbers
    va_end(ap);                                       // Results Are Stored In Text

    glDisable(GL_DEPTH_TEST); //causes text not to clip with geometry
    //position text correctly...

    // This saves our transform (matrix) information and our current viewport information.
    //glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);
    //// Use a new projection and modelview matrix to work with.
    //glMatrixMode(GL_PROJECTION);              
    //glPushMatrix();                                 
    //glLoadIdentity();                               
    //glMatrixMode(GL_MODELVIEW);                   
    //glPushMatrix();                                     
    //glLoadIdentity();                                   
    //create a viewport at x,y, but doesnt have any width (so we end up drawing there...)
    glViewport(x - 1, y - 1, 0, 0);                   
    //This actually positions the text.
    //glRasterPos4f(0, 0, 0, 1);
    ////undo everything
    //glPopMatrix();                                      // Pop the current modelview matrix off the stack
    //glMatrixMode(GL_PROJECTION);                      // Go back into projection mode
    //glPopMatrix();                                      // Pop the projection matrix off the stack
    //glPopAttrib();                                      // This restores our TRANSFORM and VIEWPORT attributes

    ////glRasterPos2f(x, y);

    //glPushAttrib(GL_LIST_BIT);                        // Pushes The Display List Bits
    //glListBase(m_nDisplayListBase - 32);      // Sets The Base Character to 32
    ////glScalef(0.5, 0.5, 0.5); 
    //glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);// Draws The Display List Text
    //glPopAttrib();                                      // Pops The Display List Bits
    glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////
//printf style print function
//NOTE: coordinates start from bottom left
//ASSUMES ORTHOGRAPHIC PROJECTION ALREADY SET UP...
inline void GLFont::glPrintfFast(int x, int y, const char *fmt, ...)   
{
    GLFontCheckInit(this);

    char        text[MAX_TEXT_LENGTH];// Holds Our String
    va_list     ap;                   // Pointer To List Of Arguments

    if(fmt == NULL) {               // If There's No Text
        return;                       // Do Nothing
    }

    va_start(ap, fmt);                            // Parses The String For Variables
    vsnprintf(text, MAX_TEXT_LENGTH, fmt, ap);    // And Converts Symbols To Actual Numbers
    va_end(ap);                                   // Results Are Stored In Text

    glDisable(GL_DEPTH_TEST); // Causes text not to clip with geometry
    //glRasterPos2f(x, y);
    ////glPushAttrib(GL_LIST_BIT);                        // Pushes The Display List Bits
    //glListBase(m_nDisplayListBase - 32);        // Sets The Base Character to 32
    //glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // Draws The Display List Text
    //glPopAttrib();                                      // Pops The Display List Bits
    glEnable(GL_DEPTH_TEST);
}


#endif
