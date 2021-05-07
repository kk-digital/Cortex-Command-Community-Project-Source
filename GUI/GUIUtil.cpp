//////////////////////////////////////////////////////////////////////////////////////////
// File:            GUIUtil.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     GUIUtil class
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jackal@shplorb.com
//                  www.shplorb.com/~jackal

//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "GUI.h"

#include <SDL2/SDL_clipboard.h>

using namespace RTE;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          TrimString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes the preceeding and ending spaces from a c type string.

char *GUIUtil::TrimString(char *String)
{
    char *ptr = String;

    // Find the first non-space character
    while(*ptr) {
        if (*ptr != ' ')
            break;
        ptr++;
    }

    // Add a null terminator after the last character
    for(int i=strlen(ptr)-1; i>=0; i--) {
        if (ptr[i] != ' ') {
            ptr[i+1] = '\0';
            break;
        }
    }

    return ptr;
}

char* GUIUtil::SafeOverlappingStrCpy(char* dst, char* src)
{
	memmove(dst, src, strlen(src) + 1);
	return dst;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetClipboardText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the text from the clipboard.

bool GUIUtil::GetClipboardText(string *Text)
{

	*Text = std::string(SDL_GetClipboardText());
	return !Text->empty();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetClipboardText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the text in the clipboard.

bool GUIUtil::SetClipboardText(string Text)
{
	return SDL_SetClipboardText(Text.c_str());
}
