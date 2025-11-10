#ifndef D_TECT_CPP
#define D_TECT_CPP

// TODO: Remove this
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb/stb_image_write.h"

#include "other/image_stuff/image_loader.cpp"

#include "text.h"
#include "base/arena.cpp"
#include "os/core/os_core_win32.h"

/* TODO:
  -- Load a font and create a font atlas to use
  -- Load the font atals to he gpu and store its data to then crop the chars out
  -- Then render the text using it 
  
*/


// void text_load_font()
// {

// }

// // ---

// Vec2_F32 text_get_text_dims(Str8 str)
// {
//   F32 text_height = 


// }



#endif