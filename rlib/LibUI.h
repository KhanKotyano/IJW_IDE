#ifndef LIB_UI_MODULE
#define  LIB_UI_MODULE
#include "uni_util.h"
#include "memory.h"

#include "raylib.h"
#include "raymath.h"

#if defined(__cplusplus)
extern "C" {
#endif
#define  CLAY_IMPLEMENTATION
#include "clay_dynamic.h"
#include "clay_renderer_raylib.h"



//typedef struct UI_FreeNames {
//  char buffer[MAX_FREE_NAMES][5];
//  u16 last_free;
//}UI_FreeNames;
//#define GENERATE_FREE_NAMES(free_names)  for (u16 i = 0; i < MAX_FREE_NAMES; ++i) snprintf(free_names.buffer[i], sizeof(free_names.buffer[i]), "%d", i);
//UI_FreeNames __ui_free_names = {0};


#include "ui_free_names.c"


typedef struct UIContext {
  bool_s8 reinitializeClay;
  bool_s8 debugEnabled;
  Font *fonts;
  float def_width;
  float def_height;
  float scaler;
  float scaler_h;
}UIContext;
static UIContext ui_context = {0};
static void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s\n", errorData.errorText.chars);
  if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
    ui_context.reinitializeClay = 1;
    Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
  }
  else if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
    ui_context.reinitializeClay = 1;
    Clay_SetMaxMeasureTextCacheWordCount(Clay_GetMaxMeasureTextCacheWordCount() * 2);
  }
}
//#define FONT_FOLDER  PATH_MERGE_STR(ASSET_PATH,"/font/")

static Font UI_LoadFont(const char *name, int font_size) {
  Font f = LoadFontEx(StringFormat("%s%s", FONT_FOLDER, name), font_size, 0, 8226);
  //SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
  return f;
}
typedef struct Layout{
  void(*function)(void);
}Layout;
ARRAY_DEC(Layout);
ARRAY_FUNCTIONS(Layout);
ARRAY_T(Layout) layout_stack = {0};
static void UI_Init(float w,float h, InternalMemoryArena *arena) {
  ui_context.fonts = (Font *)MemoryRequest( sizeof(Font) * 100, arena);
  ui_context.fonts[0] = UI_LoadFont("LanaPixel.ttf", 72);
  ui_context.def_width = w;
  ui_context.def_height = h;
  ui_context.scaler =1;

  Clay_SetMaxElementCount(8192 * 2);
  uint64_t totalMemorySize = Clay_MinMemorySize();
  Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
  Clay_Initialize(clayMemory, TYPEDEC(Clay_Dimensions) { w, h }, TYPEDEC(Clay_ErrorHandler) { HandleClayErrors, 0 });
  Clay_SetMeasureTextFunction(Raylib_MeasureText, ui_context.fonts);

  Array_Layout_init_arena(&layout_stack,1000,arena);
}
#define _INIT(type,name, ...)         \
        type name = {};      \
        {__VA_ARGS__;}  
#define _init_var(parent, name, val) parent.name = val



static bool_s8 UI_Box(const char *name , u32 color ,Clay_SizingAxis w, Clay_SizingAxis h) {
  if(name == NULL)name = GET_FREE_NAME;
  Clay_ElementId id = ui_el_begin_config(name);
  bool_s8 on_hover = (bool_s8)Clay_Hovered();
  ui_el_layout_config(UI_SIZING(UI_SIZING_FIXED, 64, 64), UI_SIZING(UI_SIZING_FIXED, 64, 64), UI_CALIGN_0);
  ui_el_set_color(on_hover ? GREEN_UNI(UniColor).uni : YELLOW_UNI(UniColor).uni);
  ui_el_end_config();ui_el_close();
  return on_hover;
}

static bool_s8 UI_Element(UI_UniConfig uni_config) {
  const char *name = uni_config.name;
  if(name == NULL)name = GET_FREE_NAME;
  bool_s8 on_hover = 0;
  Clay_ElementId id = ui_el_begin_config(name);
  ui_el_layout_config(uni_config.sizing_w, uni_config.sizing_h, uni_config.child_aling);
  ui_el_floating_config(uni_config.floating_base, uni_config.floating_interaction);
  on_hover = Clay_PointerOver(id);
  ui_el_set_color(on_hover  ? uni_config.color[1] : uni_config.color[0]);
  ui_el_border_config(uni_config.border_config);
  ui_el_clip_config(uni_config.clip_config);
  ui_el_image_config(uni_config.image_config, uni_config.aspect_ratio);

  ui_el_end_config();
  return on_hover;
}
static void UI_ElementEnd() {
  ui_el_close();
}
static bool_s8 UI_ElementStandalone(UI_UniConfig uni_config) {
  const char* name = uni_config.name;
  bool_s8 on_hover = 0;
  if (name == NULL)name = GET_FREE_NAME;
  Clay_ElementId id = ui_el_begin_config(name);
  ui_el_layout_config(uni_config.sizing_w, uni_config.sizing_h, uni_config.child_aling);
  ui_el_floating_config(uni_config.floating_base, uni_config.floating_interaction);
  

  on_hover = Clay_PointerOver(id);
  ui_el_set_color(on_hover ? uni_config.color[1] : uni_config.color[0]);
  ui_el_border_config(uni_config.border_config);
  ui_el_clip_config(uni_config.clip_config);
  ui_el_image_config(uni_config.image_config, uni_config.aspect_ratio);
  ui_el_end_config();
  if (uni_config.text_config.text != NULL) {
    ui_el_set_text_advanced(uni_config.text_config.f_space, uni_config.text_config.f_line_h, uni_config.text_config.f_wrapmode);
    ui_el_set_text_basics(uni_config.text_config.f_id, uni_config.text_config.f_size, uni_config.text_config.color);
    ui_el_set_text(uni_config.text_config.text, uni_config.text_config.is_static);
  }
  ui_el_close();


  return (bool_s8)on_hover;
}
static Clay_RenderCommandArray ClayGetLayout() {
  UI_UniConfig config = {0};
  config.color[0] = RED_UNI(UniColor).uni;
  config.sizing_w = UI_SIZING(UI_SIZING_FIXED, 200, 200);
  config.sizing_h = UI_SIZING(UI_SIZING_FIXED, 200, 200);

  Clay_BeginLayout();
  UI_ElementStandalone(config);
  //UI_Element();
  return Clay_EndLayout();
}


static void UI_SceduleLayout(void(*function)(void)){
  Layout layout = {0};
  layout.function = function;
  Array_Layout_push(&layout_stack,layout);
}

static void UI_Update(v2 mouse_pos, float gui_w, float gui_h) {
  if(gui_w > 0)
    ui_context.scaler = ui_context.def_width/ gui_w;
  if (gui_h > 0)
    ui_context.scaler = ui_context.def_height / gui_h;
  Vector2 mouseWheelDelta = GetMouseWheelMoveV();
  if (IsKeyPressed(KEY_D)) {
    ui_context.debugEnabled = ~ui_context.debugEnabled;
    Clay_SetDebugModeEnabled(ui_context.debugEnabled);
  }
  Clay_Vector2 mousePosition = V2TOV2(Clay_Vector2, mouse_pos);
  Clay_SetPointerState(mousePosition, IsMouseButtonDown(MOUSE_BUTTON_LEFT) );//&& !scrollbarData.mouseDown
  Clay_SetLayoutDimensions(TYPEDEC(Clay_Dimensions) { gui_w, gui_h});
  Clay_UpdateScrollContainers(true, V2TOV2(Clay_Vector2, mouseWheelDelta), GetFrameTime());
  Clay_BeginLayout();
    ui_el_begin_config("MainWindow");
    ui_el_layout_config(UI_SIZING(UI_SIZING_GROW,0,0), UI_SIZING(UI_SIZING_GROW,0,0), UI_CALIGN(0, 0, 0,UI_DIR_BOTTOM));
    ui_el_set_color(DARKGRAY_UNI(UniColor).uni);
    ui_el_end_config();
      while(layout_stack.size){
        Layout *layout = Array_Layout_pop(&layout_stack);
        layout->function();
      }
    ui_el_close();
  Clay_RenderCommandArray renderCommands =  Clay_EndLayout();//ClayGetLayout();
  Clay_Raylib_Render(renderCommands, ui_context.fonts);
  RESET_FREE_NAMES;
}
#if defined(__cplusplus)
}
#endif
#endif