#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

typedef union DrawId {
  u64 id;
  struct {
    u32 real_id;
    u16 check_id;
    union {
      u16 type_id;
      struct {
        engine_inst_type type;
        u8 subtype;
      };
    };
  };
}DrawId;
typedef struct CImage {
  Texture texture;
} CImage;
ARRAY_DEC(CImage);
ARRAY_FUNCTIONS(CImage);
static ARRAY_T(CImage) image_array = { 0 };


static inline ARRAY_T(CImage)* get_img_handle_array() {
  return &image_array;
};



static inline CImage* GetImage(const char* name) {
  DrawId draw_id = { 0 };
  draw_id.id = GetHashID(name);
  u32 id = draw_id.real_id;
  if (id == FAILED_HASH_ID)id = 0;
  CImage* img = Array_CImage_get(*get_img_handle_array(), id);
  return img;
};
static inline u32 GetImageID(const char* name) {
  DrawId draw_id = { 0 };
  draw_id.id = GetHashID(name);
  u32 id = draw_id.real_id;
  if(id == FAILED_HASH_ID)id = 0;
  return id;
};
static inline CImage* GetImageViaID(u32 id) {
  CImage* img = Array_CImage_get(*get_img_handle_array(), id);
  return img;
};
static inline CImage* GetImageViaName(const char* name) {
  u32 id = GetImageID(name);
  return GetImageViaID(id);
};

static DrawId AddImage(const char* path, const char* name) {
  static u16 lua_check_id = 0;
  DrawId draw_id = { 0 };
  draw_id.id = GetHashID(name);
  u32 id = draw_id.real_id;
  ARRAY_T(CImage)* image_array = get_img_handle_array();
  assertf(image_array != NULL, "Image handler is not initialized");
  assertf(image_array->buffer != NULL, "Image handler buffer not initialized");
  if (id >= image_array->size || id == FAILED_HASH_ID) {
    draw_id.id = 0;
    id = image_array->size;
    image_array->buffer[id].texture = LoadTexture(path);
    //SetTextureFilter( image_array->buffer[id].texture, TextureFilter::TEXTURE_FILTER_TRILINEAR);
    if (image_array->buffer[id].texture.width <= 0
      || image_array->buffer[id].texture.height <= 0) {
      id = 0;
    }
    else  image_array->size++;
  }
  else {
    UnloadTexture(image_array->buffer[id].texture);
    image_array->buffer[id].texture = LoadTexture(path);
    if (image_array->buffer[id].texture.width <= 0
      || image_array->buffer[id].texture.height <= 0) {
      id = 0;
    }
  }
  draw_id.check_id = lua_check_id;
  lua_check_id++;
  draw_id.type = ET_ANIMATION;
  draw_id.real_id = id;
  SetHashID(name, draw_id.id);
  return draw_id;
};








#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rectangle) TYPEDEC(Rectangle) { .x = rectangle.x, .y = rectangle.y, .width = rectangle.width, .height = rectangle.height }
#define CLAY_COLOR_TO_RAYLIB_COLOR(color) TYPEDEC(Color) { .r = (unsigned char)roundf(color.r), .g = (unsigned char)roundf(color.g), .b = (unsigned char)roundf(color.b), .a = (unsigned char)roundf(color.a) }

Camera Raylib_camera;

typedef enum
{
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL
} CustomLayoutElementType;

typedef struct
{
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct
{
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
    } customData;
} CustomLayoutElement;

// Get a ray trace from the screen position (i.e mouse) within a specific section of the screen
static Ray GetScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance)
{
    Ray ray = { 0 };

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f*position.x)/(float)screenWidth - 1.0f;
    float y = 1.0f - (2.0f*position.y)/(float)screenHeight;
    float z = 1.0f;

    // Store values in a vector
    Vector3 deviceCoords = { x, y, z };

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(camera.fovy*DEG2RAD, ((double)screenWidth/(double)screenHeight), 0.01f, zDistance);
    }
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        double aspect = (double)screenWidth/(double)screenHeight;
        double top = camera.fovy/2.0;
        double right = top*aspect;

        // Calculate projection matrix from orthographic
        matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    Vector3 nearPoint = Vector3Unproject(TYPEDEC(Vector3){ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
    Vector3 farPoint = Vector3Unproject(TYPEDEC(Vector3){ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);

    // Calculate normalized direction vector
    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

    ray.position = farPoint;

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}
int Util_GetCodepointNext(const char* text, int* codepointSize)
{
  const char* ptr = text;
  int codepoint = 0x3f;       // Codepoint (defaults to '?')
  *codepointSize = 1;

  // Get current codepoint and bytes processed
  if (0xf0 == (0xf8 & ptr[0]))
  {
    // 4 byte UTF-8 codepoint
    if (((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80) || ((ptr[3] & 0xC0) ^ 0x80)) { return codepoint; } // 10xxxxxx checks
    codepoint = ((0x07 & ptr[0]) << 18) | ((0x3f & ptr[1]) << 12) | ((0x3f & ptr[2]) << 6) | (0x3f & ptr[3]);
    *codepointSize = 4;
  }
  else if (0xe0 == (0xf0 & ptr[0]))
  {
    // 3 byte UTF-8 codepoint */
    if (((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80)) { return codepoint; } // 10xxxxxx checks
    codepoint = ((0x0f & ptr[0]) << 12) | ((0x3f & ptr[1]) << 6) | (0x3f & ptr[2]);
    *codepointSize = 3;
  }
  else if (0xc0 == (0xe0 & ptr[0]))
  {
    // 2 byte UTF-8 codepoint
    if ((ptr[1] & 0xC0) ^ 0x80) { return codepoint; } // 10xxxxxx checks
    codepoint = ((0x1f & ptr[0]) << 6) | (0x3f & ptr[1]);
    *codepointSize = 2;
  }
  else if (0x00 == (0x80 & ptr[0]))
  {
    // 1 byte UTF-8 codepoint
    codepoint = ptr[0];
    *codepointSize = 1;
  }

  return codepoint;
}
typedef Font FontData ;
bool IsFontDataValid(FontData font)
{
  return ((font.baseSize > 0) &&
    (font.glyphCount > 0) &&
    (font.recs != NULL) &&
    (font.glyphs != NULL));

  // NOTE: Further validations could be done to verify if recs and glyphs contain valid data (glyphs values, metrics...)
}
int GetGlyphDataIndex(FontData font, int codepoint)
{
  int index = 0;
  if (!IsFontDataValid(font)) return index;
  int fallbackIndex = 0;      // Get index of fallback glyph '?'
  for (int i = 0; i < font.glyphCount; i++) {
    if (font.glyphs[i].value == 63) fallbackIndex = i;

    if (font.glyphs[i].value == codepoint)
    {
      index = i;
      break;
    }
  }

  if ((index == 0) && (font.glyphs[0].value != codepoint)) index = fallbackIndex;

  return index;
}
Vector2 MeasureTextEx_ext(FontData font, const char* text, float fontSize, float spacing, int size) {
  Vector2 textSize = { 0 };
  int tempByteCounter = 0;
  int byteCounter = 0;

  float textWidth = 0.0f;
  float tempTextWidth = 0.0f;

  float textHeight = font.baseSize * (fontSize / (float)font.baseSize);
  textHeight = textHeight * 1.0;
  float scaleFactor = fontSize / (float)font.baseSize;

  int letter = 0;
  int index = 0;

  for (int i = 0; i < size;)
  {
    byteCounter++;

    int codepointByteCount = 0;
    letter = Util_GetCodepointNext(&text[i], &codepointByteCount);
    index = GetGlyphDataIndex(font, letter);

    i += codepointByteCount;

    if (letter != '\n')
    {
      if (font.glyphs[index].advanceX > 0) textWidth += font.glyphs[index].advanceX;
      else textWidth += (font.recs[index].width + font.glyphs[index].offsetX);
    }
    else
    {
      if (tempTextWidth < textWidth) tempTextWidth = textWidth;
      byteCounter = 0;
      textWidth = 0;

      // NOTE: Line spacing is a global variable, use SetTextLineSpacing() to setup
      textHeight += (fontSize + 2);
    }

    if (tempByteCounter < byteCounter) tempByteCounter = byteCounter;
  }

  if (tempTextWidth < textWidth) tempTextWidth = textWidth;

  textSize.x = tempTextWidth * scaleFactor + (float)((tempByteCounter - 1) * spacing);
  textSize.y = textHeight;

  return textSize;
}
static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
   // Measure string size for Font
  //double idk = 110000102541E74; 

  Clay_Dimensions textSize = { 0 };
  FontData *fdata = (FontData *)userData;
  FontData fontToUse = fdata[config->fontId];
  if (!fontToUse.glyphs) {
    printf("[ClayUI]: Font fail to be recognized\n");
    return textSize;
  }
  Vector2 v = MeasureTextEx_ext(fontToUse, text.chars, config->fontSize, config->letterSpacing, text.length);
  textSize.width = v.x;
  textSize.height = v.y;
  return textSize;
}




static inline Clay_Dimensions Clay_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) {
  // Measure string size for Font
  Clay_Dimensions textSize = { 0 };

  float maxTextWidth = 0.0f;
  float lineTextWidth = 0;
  int maxLineCharCount = 0;
  int lineCharCount = 0;

  float textHeight = config->fontSize;
  Font* fonts = (Font*)userData;
  Font fontToUse = fonts[config->fontId];
  // Font failed to load, likely the fonts are in the wrong place relative to the execution dir.
  // RayLib ships with a default font, so we can continue with that built in one. 
  if (!fontToUse.glyphs) {
    fontToUse = GetFontDefault();
  }

  float scaleFactor = config->fontSize / (float)fontToUse.baseSize;

  for (int i = 0; i < text.length; ++i, lineCharCount++)
  {
    if (text.chars[i] == '\n') {
      maxTextWidth = fmax(maxTextWidth, lineTextWidth);
      maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);
      lineTextWidth = 0;
      lineCharCount = 0;
      continue;
    }
    int index = text.chars[i] - 32;
    if (fontToUse.glyphs[index].advanceX != 0) lineTextWidth += fontToUse.glyphs[index].advanceX;
    else lineTextWidth += (fontToUse.recs[index].width + fontToUse.glyphs[index].offsetX);
  }

  maxTextWidth = fmax(maxTextWidth, lineTextWidth);
  maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);

  textSize.width = maxTextWidth * scaleFactor + (lineCharCount * config->letterSpacing);
  textSize.height = textHeight;

  return textSize;
}










void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags) {
    SetConfigFlags(flags);
    InitWindow(width, height, title);
//    EnableEventWaiting();
}

// A MALLOC'd buffer, that we keep modifying inorder to save from so many Malloc and Free Calls.
// Call Clay_Raylib_Close() to free
static char *temp_render_buffer = NULL;
static int temp_render_buffer_len = 0;

// Call after closing the window to clean up the render buffer
static void Clay_Raylib_Close()
{
    if(temp_render_buffer) free(temp_render_buffer);
    temp_render_buffer_len = 0;

    CloseWindow();
}


static void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font* fonts)
{
    for (int j = 0; j < renderCommands.length; j++)
    {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
        Clay_BoundingBox boundingBox = {roundf(renderCommand->boundingBox.x), roundf(renderCommand->boundingBox.y), roundf(renderCommand->boundingBox.width), roundf(renderCommand->boundingBox.height)};
        switch (renderCommand->commandType)
        {
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextRenderData *textData = &renderCommand->renderData.text;
                Font fontToUse = fonts[textData->fontId];
    
                int strlen = textData->stringContents.length + 1;
    
                if(strlen > temp_render_buffer_len) {
                    // Grow the temp buffer if we need a larger string
                    if(temp_render_buffer) free(temp_render_buffer);
                    temp_render_buffer = (char *) malloc(strlen);
                    temp_render_buffer_len = strlen;
                }
    
                // Raylib uses standard C strings so isn't compatible with cheap slices, we need to clone the string to append null terminator
                memcpy(temp_render_buffer, textData->stringContents.chars, textData->stringContents.length);
                temp_render_buffer[textData->stringContents.length] = '\0';
                DrawTextEx(fontToUse, temp_render_buffer, TYPEDEC(Vector2){boundingBox.x, boundingBox.y}, (float)textData->fontSize, (float)textData->letterSpacing, CLAY_COLOR_TO_RAYLIB_COLOR(textData->textColor));
    
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                CImage *img = GetImageViaID(renderCommand->renderData.image.imageData);
                if(img == NULL )continue;
                Texture2D imageTexture = img->texture;
                Clay_Color tintColor = renderCommand->renderData.image.backgroundColor;
                if (tintColor.r == 0 && tintColor.g == 0 && tintColor.b == 0 && tintColor.a == 0) {
                    tintColor = TYPEDEC(Clay_Color) { 255, 255, 255, 255 };
                }
                DrawTexturePro(
                    imageTexture,
                  TYPEDEC(Rectangle) { 0, 0, (float)imageTexture.width, (float)imageTexture.height },
                  TYPEDEC(Rectangle){floorf(boundingBox.x), floorf(boundingBox.y), floorf( boundingBox.width), floorf(boundingBox.height)},
                  TYPEDEC(Vector2) {0,0},
                    0,
                    CLAY_COLOR_TO_RAYLIB_COLOR(tintColor));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                BeginScissorMode((int)roundf(boundingBox.x), (int)roundf(boundingBox.y), (int)roundf(boundingBox.width), (int)roundf(boundingBox.height));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                EndScissorMode();
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
                if (config->cornerRadius.topLeft > 0) {
                    float radius = (config->cornerRadius.topLeft * 2) / (float)((boundingBox.width > boundingBox.height) ? boundingBox.height : boundingBox.width);
                    DrawRectangleRounded(TYPEDEC(Rectangle) { boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height }, radius, 8, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                } else {
                    DrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                Clay_BorderRenderData *config = &renderCommand->renderData.border;
                // Left border
                if (config->width.left > 0) {
                    DrawRectangle((int)roundf(boundingBox.x), (int)roundf(boundingBox.y + config->cornerRadius.topLeft), (int)config->width.left, (int)roundf(boundingBox.height - config->cornerRadius.topLeft - config->cornerRadius.bottomLeft), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Right border
                if (config->width.right > 0) {
                    DrawRectangle((int)roundf(boundingBox.x + boundingBox.width - config->width.right), (int)roundf(boundingBox.y + config->cornerRadius.topRight), (int)config->width.right, (int)roundf(boundingBox.height - config->cornerRadius.topRight - config->cornerRadius.bottomRight), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Top border
                if (config->width.top > 0) {
                    DrawRectangle((int)roundf(boundingBox.x + config->cornerRadius.topLeft), (int)roundf(boundingBox.y), (int)roundf(boundingBox.width - config->cornerRadius.topLeft - config->cornerRadius.topRight), (int)config->width.top, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Bottom border
                if (config->width.bottom > 0) {
                    DrawRectangle((int)roundf(boundingBox.x + config->cornerRadius.bottomLeft), (int)roundf(boundingBox.y + boundingBox.height - config->width.bottom), (int)roundf(boundingBox.width - config->cornerRadius.bottomLeft - config->cornerRadius.bottomRight), (int)config->width.bottom, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.topLeft > 0) {
                    DrawRing(TYPEDEC(Vector2) { roundf(boundingBox.x + config->cornerRadius.topLeft), roundf(boundingBox.y + config->cornerRadius.topLeft) }, roundf(config->cornerRadius.topLeft - config->width.top), config->cornerRadius.topLeft, 180, 270, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.topRight > 0) {
                    DrawRing(TYPEDEC(Vector2) { roundf(boundingBox.x + boundingBox.width - config->cornerRadius.topRight), roundf(boundingBox.y + config->cornerRadius.topRight) }, roundf(config->cornerRadius.topRight - config->width.top), config->cornerRadius.topRight, 270, 360, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.bottomLeft > 0) {
                    DrawRing(TYPEDEC(Vector2) { roundf(boundingBox.x + config->cornerRadius.bottomLeft), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomLeft) }, roundf(config->cornerRadius.bottomLeft - config->width.bottom), config->cornerRadius.bottomLeft, 90, 180, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.bottomRight > 0) {
                    DrawRing(TYPEDEC(Vector2) { roundf(boundingBox.x + boundingBox.width - config->cornerRadius.bottomRight), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomRight) }, roundf(config->cornerRadius.bottomRight - config->width.bottom), config->cornerRadius.bottomRight, 0.1, 90, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                Clay_CustomRenderData *config = &renderCommand->renderData.custom;
                CustomLayoutElement *customElement = (CustomLayoutElement *)config->customData;
                if (!customElement) continue;
                switch (customElement->type) {
                    case CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL: {
                        Clay_BoundingBox rootBox = renderCommands.internalArray[0].boundingBox;
                        float scaleValue = CLAY__MIN(CLAY__MIN(1, 768 / rootBox.height) * CLAY__MAX(1, rootBox.width / 1024), 1.5f);
                        Ray positionRay = GetScreenToWorldPointWithZDistance(TYPEDEC(Vector2) { renderCommand->boundingBox.x + renderCommand->boundingBox.width / 2, renderCommand->boundingBox.y + (renderCommand->boundingBox.height / 2) + 20 }, Raylib_camera, (int)roundf(rootBox.width), (int)roundf(rootBox.height), 140);
                        BeginMode3D(Raylib_camera);
                            DrawModel(customElement->customData.model.model, positionRay.position, customElement->customData.model.scale * scaleValue, WHITE);        // Draw 3d model with texture
                        EndMode3D();
                        break;
                    }
                    default: break;
                }
                break;
            }
            default: {
                printf("Error: unhandled render command.");
                exit(1);
            }
        }
    }
}
