#include "freetype/freetype.h"

#include "./mem.h"
#include "./renderer.h"
#include "./types.h"

// This has been mostly yanked from https://github.com/Cakez77/CelesteClone
void load_font(char *filePath, int fontSize, RenderData *renderData, GLContext *glContext,
               BumpAllocator *tempStorage) {
    FT_Library fontLibrary;
    FT_Init_FreeType(&fontLibrary);

    FT_Face fontFace;
    FT_New_Face(fontLibrary, filePath, 0, &fontFace);
    FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

    int padding = 2;
    int row = 0;
    int col = padding;

    const int textureWidth = 512;
    char *textureBuffer = (char *)tempStorage->alloc(textureWidth * textureWidth);

    for (FT_ULong glyphIdx = 32; glyphIdx < 127; ++glyphIdx) {
        FT_UInt glyphIndex = FT_Get_Char_Index(fontFace, glyphIdx);
        FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT);
        FT_Error error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL);

        if (col + fontFace->glyph->bitmap.width + padding >= 512) {
            col = padding;
            row += fontSize;
        }

        long height = (fontFace->size->metrics.ascender - fontFace->size->metrics.descender) >> 6;
        renderData->fontHeight = height > renderData->fontHeight ? height : renderData->fontHeight;

        for (unsigned int y = 0; y < fontFace->glyph->bitmap.rows; ++y) {
            for (unsigned int x = 0; x < fontFace->glyph->bitmap.width; ++x) {
                textureBuffer[(row + y) * textureWidth + col + x] =
                    fontFace->glyph->bitmap.buffer[y * fontFace->glyph->bitmap.width + x];
            }
        }

        Glyph *glyph = &renderData->glyphs[glyphIdx];
        glyph->textureCoords = {col, row};
        glyph->size = {(int)fontFace->glyph->bitmap.width, (int)fontFace->glyph->bitmap.rows};
        glyph->advance = {(float)(fontFace->glyph->advance.x >> 6),
                          (float)(fontFace->glyph->advance.y >> 6)};
        glyph->offset = {
            (float)fontFace->glyph->bitmap_left,
            (float)fontFace->glyph->bitmap_top,
        };

        col += fontFace->glyph->bitmap.width + padding;
    }

    FT_Done_Face(fontFace);
    FT_Done_FreeType(fontLibrary);

    {
        glGenTextures(1, &glContext->fontAtlasID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext->fontAtlasID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureWidth, 0, GL_RED,
                     GL_UNSIGNED_BYTE, (char *)textureBuffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}
