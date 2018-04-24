#include <string.h>
#include <stdio.h>
#include <switch.h>

#include <ft2build.h>
#include FT_FREETYPE_H

//See also libnx pl.h.

//TODO: Finish this.

//This requires the switch-freetype package.
//Freetype code here is based on the example code from freetype docs.

int main(int argc, char **argv)
{
    Result rc=0;
    //u32* framebuf;
    u32 i;

    u64 LanguageCode=0;
    PlFontData fonts[PlSharedFontType_Total];
    size_t total_fonts=0;
    FT_Error ret;
    FT_Library library;
    FT_Face face;
    FT_UInt glyph_index;

    gfxInitDefault();
    consoleInit(NULL);

    rc = setInitialize();
    if (R_SUCCEEDED(rc)) rc = setGetSystemLanguage(&LanguageCode);
    setExit();

    if (R_FAILED(rc)) printf("Failed to get system-language: 0x%x\n", rc);

    if (R_SUCCEEDED(rc))
    {
        rc = plInitialize();
        if (R_FAILED(rc)) printf("plInitialize() faild: 0x%x\n", rc);

        if (R_SUCCEEDED(rc))
        {
            rc = plGetSharedFont(LanguageCode, fonts, PlSharedFontType_Total, &total_fonts);

            if (R_FAILED(rc)) printf("plGetSharedFont() failed: 0x%x\n", rc);

            if (R_SUCCEEDED(rc))
            {
                printf("total_fonts: %lu\n", total_fonts);

                for (i=0; i<total_fonts; i++) printf("[%u] type=%u\n", i, fonts[i].type);
            }

            ret = FT_Init_FreeType(&library);
            if (ret) printf("FT_Init_FreeType() failed: %d\n", ret);

            if (ret==0)
            {
                ret = FT_New_Memory_Face( library,
                                          fonts[0].address,    /* first byte in memory */
                                          fonts[0].size,       /* size in bytes        */
                                          0,                   /* face_index           */
                                          &face);

                if (ret) printf("FT_New_Memory_Face() failed: %d\n", ret);

                if (ret==0)
                {
                    ret = FT_Set_Char_Size(
                            face,    /* handle to face object           */
                            0,       /* char_width in 1/64th of points  */
                            16*64,   /* char_height in 1/64th of points */
                            300,     /* horizontal device resolution    */
                            300);    /* vertical device resolution      */

                    if (ret) printf("FT_Set_Char_Size() failed: %d\n", ret);

                    if (ret==0) glyph_index = FT_Get_Char_Index(face, 'A');//TODO: Load charcode from string.

                    FT_Done_Face(face);
                }

                FT_Done_FreeType(library);
            }

            plExit();
        }
    }

    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        //u32 width, height;
        //framebuf = (u32*) gfxGetFramebuffer((u32*)&width, (u32*)&height);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}
