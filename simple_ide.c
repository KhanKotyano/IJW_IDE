#include "rlib/raylib.h"
#include "rlib/raymath.h"
#define UNI_UTIL_LIB_IMPLEMENTATION
#include "rlib/uni_util.h"
#include "rlib/memory.h"


#include "raylib_win32.h"

//#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include "rlib/csv_singleh.h"
//#define DEBUG_MODE
#define FONT_FOLDER ""
#include "rlib/LibUI.h"

typedef struct Files {
    char path[100][2048];
    u16 size;
} Files;
static Files files = {0};
static inline Files *App_GetFiles(){
    return &files;
}
static inline void App_ClearFiles(){
    ZeroMemory_Util(App_GetFiles(), sizeof(Files));
}

typedef struct Input{
  u32 down_time;
  bool_s8 released;
  bool_s8 pressed;

}Input;

typedef struct InputMouse {
  union {
    Input a[3];
    struct {
      Input right;
      Input middle;
      Input left;
    };
  };
  v3 mouse_world_pos;
  v2 mouse_gui_pos;
  u16 mouse_id[3];
}InputMouse;
typedef union MouseInputId{
  u16 id;
  u8 a[2];
}MouseInputId;
static InputMouse mouse_imput_handle = {0};

static inline void input_clear_mouse_id() {
  loop(i,3)
    mouse_imput_handle.mouse_id[i] = 0;
}
typedef enum MOUSE_INPUT_ID {
  LEFT_PRESSED= 11,
  LEFT_RELEASED = 12,
  LEFT_DOWN = 13,
  RIGHT_PRESSED = 21,
  RIGHT_RELEASED = 22,
  RIGHT_DOWN = 23,
  CENTER_PRESSED = 31,
  CENTER_RELEASED = 32,
  CENTER_DOWN = 33,
}MOUSE_INPUT_ID;

static void Input_HandleMouse(){
  input_clear_mouse_id();
  loop(i, 3) {
    if (IsMouseButtonPressed(i)) { 
      mouse_imput_handle.mouse_id[i] = 1 + ((i + 1) * 10); 
      continue; 
    }
    if (IsMouseButtonReleased(i)) { mouse_imput_handle.mouse_id[i] = 2 + ((i + 1) * 10); continue; }
    if (IsMouseButtonDown(i)) { mouse_imput_handle.mouse_id[i] = 3 + ((i + 1) * 10); continue; }
  }
  mouse_imput_handle.mouse_gui_pos = TYPEDEC(v2) { .raylib = GetMousePosition() };
}

static inline v2 get_mouse_pos_gui() {
  return mouse_imput_handle.mouse_gui_pos;
}
static inline v2 get_mouse_pos_world() {
  return mouse_imput_handle.mouse_world_pos.v2;
}
static inline u16 get_mouse_input_id() {
  u16 ret_id = 0;
  loop(i,3) {
    ret_id = mouse_imput_handle.mouse_id[i];
    if(ret_id){
      break;
    }
  }
  return ret_id;
}
static u32 input_ui_shared_dispatch[3] = {0};
static inline void dispatch_mouse_input_id(u32 id) {
  if(id >= 0 && id <3){
    mouse_imput_handle.mouse_id[id] = 0;
  }
}
















#define MAX_LANGUAGES 16
#define MAX_ROWS 2048

typedef struct {
    char *key;
    char **translations;  
} LocalizationEntry;

typedef struct {
    char **languages;         
    int language_count;

    LocalizationEntry *entries;  
    int entry_count;
} LocalizationTable;

typedef struct {
    LocalizationTable *table;
    int current_field;
    int current_row;
    char *fields[MAX_LANGUAGES + 1]; 
} ParseContext;

typedef struct {
    const char *key;
    const char *value;
} LocalizedEntryPair;

void field_cb(void *s, size_t len, void *data) {
    ParseContext *ctx = (ParseContext *)data;
    char *field = malloc(len + 1);
    memcpy(field, s, len);
    field[len] = '\0';
    ctx->fields[ctx->current_field++] = field;
}

void row_cb(int c, void *data) {
    ParseContext *ctx = (ParseContext *)data;

    if (ctx->current_row == 0) {
        // Header row: extract languages
        ctx->table->language_count = ctx->current_field - 1;
        ctx->table->languages = malloc(sizeof(char*) * ctx->table->language_count);

        for (int i = 1; i < ctx->current_field; ++i) {
            ctx->table->languages[i - 1] = ctx->fields[i];
        }
    } else {
        // Regular row
        LocalizationEntry *entry = &ctx->table->entries[ctx->table->entry_count++];
        entry->key = ctx->fields[0];

        entry->translations = malloc(sizeof(char*) * ctx->table->language_count);
        for (int i = 1; i < ctx->current_field; ++i) {
            entry->translations[i - 1] = ctx->fields[i];
        }
    }

    ctx->current_field = 0;
    ctx->current_row++;
}


LocalizationTable *load_localization_csv(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open CSV file");
        return NULL;
    }

    struct csv_parser parser;
    if (csv_init(&parser, 0) != 0) {
        fprintf(stderr, "Failed to init libcsv\n");
        fclose(fp);
        return NULL;
    }

    LocalizationTable *table = calloc(1, sizeof(LocalizationTable));
    table->entries = malloc(sizeof(LocalizationEntry) * MAX_ROWS);

    ParseContext ctx = { 0 };
    ctx.table = table;

    char buf[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (csv_parse(&parser, buf, bytes_read, field_cb, row_cb, &ctx) != bytes_read) {
            fprintf(stderr, "Parse error: %s\n", csv_strerror(csv_error(&parser)));
            break;
        }
    }

    csv_fini(&parser, field_cb, row_cb, &ctx);
    csv_free(&parser);
    fclose(fp);
    return table;
}


const char *get_localized(LocalizationTable *table, const char *key, const char *lang) {
    int lang_index = -1;
    for (int i = 0; i < table->language_count; ++i) {
        if (strcmp(table->languages[i], lang) == 0) {
            lang_index = i;
            break;
        }
    }

    if (lang_index == -1)
        return NULL;

    for (int i = 0; i < table->entry_count; ++i) {
        if (strcmp(table->entries[i].key, key) == 0) {
            return table->entries[i].translations[lang_index];
        }
    }

    return NULL;
}

LocalizedEntryPair get_localized_entry(const LocalizationTable *table, const char *lang, int index) {
    LocalizedEntryPair result = { NULL, NULL };

    if (!table || index < 0 || index >= table->entry_count)
        return result;

    int lang_index = -1;
    for (int i = 0; i < table->language_count; ++i) {
        if (strcmp(table->languages[i], lang) == 0) {
            lang_index = i;
            break;
        }
    }

    if (lang_index == -1)
        return result;

    result.key = table->entries[index].key;
    result.value = table->entries[index].translations[lang_index];
    return result;
}
void make_lua_key(const char *src, char *dst, size_t max_len) {
    size_t i;
    for (i = 0; i < max_len - 1 && src[i]; ++i) {
        dst[i] = (src[i] == '.') ? '_' : src[i];
    }
    dst[i] = '\0';
}
void escape_lua_string(const char *src, char *dst, size_t dst_size) {
    const unsigned char *s = (const unsigned char *)src;
    char *d = dst;
    size_t left = dst_size - 1;

    while (*s && left > 0) {
        if (*s == '\n' && left >= 2) {
            *d++ = '\\'; *d++ = 'n'; left -= 2;
        } else if (*s == '\t' && left >= 2) {
            *d++ = '\\'; *d++ = 't'; left -= 2;
        } else if (*s == '\\' && left >= 2) {
            *d++ = '\\'; *d++ = '\\'; left -= 2;
        } else if (*s == '\"' && left >= 2) {
            *d++ = '\\'; *d++ = '\"'; left -= 2;
        } else if (*s < 0x20 || *s == 0x7F) {
            // control characters that are NOT printable
            if (left >= 4) {
                snprintf(d, left, "\\x%02X", *s);
                d += 4; left -= 4;
            } else break;
        } else {
            // UTF-8 safe: just copy byte
            *d++ = *s++; left--;
            continue;
        }
        s++;
    }

    *d = '\0';
}

void export_language_lua(const LocalizationTable *table, const char *lang, const char *output_dir) {
    if (!table || !lang || !output_dir) return;

    // Build filename: output_dir/lang.lua
    char filename[512];
    int needs_slash = output_dir[strlen(output_dir) - 1] != '/' && output_dir[strlen(output_dir) - 1] != '\\';
    snprintf(filename, sizeof(filename), "%s%s%s.lua",
             output_dir,
             needs_slash ? "/" : "",
             lang);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for writing");
        return;
    }

    // Write UTF-8 BOM
    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    fwrite(bom, sizeof(bom), 1, fp);

    fprintf(fp, "localization = {\n");

    // Find language column
    int lang_index = -1;
    for (int i = 0; i < table->language_count; ++i) {
        if (strcmp(table->languages[i], lang) == 0) {
            lang_index = i;
            break;
        }
    }

    if (lang_index == -1) {
        fprintf(stderr, "Language not found: %s\n", lang);
        fclose(fp);
        return;
    }

    // Write key = "value" pairs
    for (int i = 0; i < table->entry_count; ++i) {
        const char *key = table->entries[i].key;
        const char *value = table->entries[i].translations[lang_index];
        if (!key || !value) continue;

        char sanitized_key[128];
        char escaped_value[2048];

        make_lua_key(key, sanitized_key, sizeof(sanitized_key));
        escape_lua_string(value, escaped_value, sizeof(escaped_value));

        fprintf(fp, "    %s = \"%s\",\n", sanitized_key, escaped_value);
    }

    fprintf(fp, "}\n");
    fclose(fp);
}
void export_all_languages(const LocalizationTable *table, const char *path) {
    if(!DirectoryExists(path)){
        MakeDirectory(path);
    }
    for (int i = 0; i < table->language_count; ++i) {
        export_language_lua(table, table->languages[i], path);
    }
}

#define MAX_FILEPATH_RECORDED 99

u32 GetDroppedFiles(Files *files){
    u32 new_files_count = 0;
    if (IsFileDropped()){
        FilePathList droppedFiles = LoadDroppedFiles();
        new_files_count = droppedFiles.count;
        loopa (i,droppedFiles.count){
            if(files->size >=MAX_FILEPATH_RECORDED)break;
            TextCopy(files->path[files->size++], droppedFiles.paths[i]);
        }
        UnloadDroppedFiles(droppedFiles);    
    }
    return new_files_count;
}
u32 GetDroppedFilesExt(Files *files, const char *file_extension){
    if(file_extension == NULL){ShowMessageModule("LOCALIZATION",ML_ERROR, " (GetDroppedFilesExt):File format is NULL!");return 0;}
    u32 new_files_count = 0;
    if (IsFileDropped()){
        FilePathList droppedFiles = LoadDroppedFiles();
        printf("%d new files has been dropped\n", droppedFiles.count);
        loopa (i,droppedFiles.count){
            if(files->size >=MAX_FILEPATH_RECORDED)break;
            if(IsFileExtension(droppedFiles.paths[i], file_extension)){
                new_files_count++;
                TextCopy(files->path[files->size++], droppedFiles.paths[i]);
            }
        }
        UnloadDroppedFiles(droppedFiles);    
    }
    return new_files_count;
}


typedef enum APP_PAGE {
    PAGE_NONE,
    PAGE_LOCALIZATOIN,
}APP_PAGE;
static APP_PAGE app_page = PAGE_NONE;
static APP_PAGE app_next_page = PAGE_NONE;
static APP_PAGE app_prev_page = PAGE_NONE;
static inline APP_PAGE App_GetPage(){
    return app_page;
}
static inline void App_SetPage(APP_PAGE page){
    app_next_page = page;
}
static inline void App_ChangePage(){
    if(app_page != app_next_page){
        App_ClearFiles();
        app_prev_page = app_page;
        app_page = app_next_page;
    }
}
typedef union ColorBundle{
    u32 colors[2];
    UniColor uni_color[2];
    struct {
      UniColor main_color;
      UniColor second_color;  
    };
}ColorBundle;
#define COLORB(...)TYPEDEC(ColorBundle){__VA_ARGS__}






static u16 GUI_ButtonText(const char *text,u16 text_size ,int min_w, int min_h, ColorBundle color, u16 boder_size, float border_rad  ){
    Clay_ElementId id = ui_el_begin_config(GET_FREE_NAME);
    UICaling cal = UI_CALIGN(UI_CALIGN_CENTER, UI_CALIGN_CENTER, 0,UI_DIR_RIGHT);
    cal.padding =TYPEDEC(Clay_Padding) {.left = boder_size +10,.right= boder_size +10,.bottom = boder_size,.top = boder_size};
    ui_el_layout_config(UI_SIZING(UI_SIZING_FIT,min_w,0), UI_SIZING(UI_SIZING_FIT,min_h,0), cal);
    bool on_hover = Clay_PointerOver(id);
    ui_el_set_color(on_hover ? color.colors[1] : color.colors[0]);
    ui_el_border_config_flat(boder_size, on_hover ? color.colors[0] : color.colors[1], border_rad);
    
    ui_el_end_config();
        if(text != NULL){
            ui_el_set_text_basics(0,text_size,on_hover ? color.colors[0] : color.colors[1]);
            ui_el_set_text(text, (bool_s8)1);
        }
    ui_el_close();
    return on_hover * get_mouse_input_id();
}

static inline u16 GUI_Button(int min_w, int min_h, ColorBundle color, u16 boder_size, float border_rad ){
    return GUI_ButtonText(NULL, 0, min_w, min_h, color , boder_size, border_rad);
}
static inline bool_s8 GUI_TextExt(const char *text, u16 text_size,u32 text_color, bool_s8 is_static , bool_s8 attach ){
    Clay_ElementId id = ui_el_begin_config(GET_FREE_NAME);
    bool_s8 on_hover = Clay_PointerOver(id);
    UICaling cal = UI_CALIGN(UI_CALIGN_CENTER, UI_CALIGN_CENTER, 0,UI_DIR_RIGHT);
    ui_el_layout_config(UI_SIZING(UI_SIZING_FIT,0,0), UI_SIZING(UI_SIZING_FIT,0,0), cal);
    if(attach > 0)ui_el_attach_to(CLAY_ATTACH_TO_PARENT, CLAY_ATTACH_POINT_CENTER_CENTER);
    ui_el_end_config();
        if(text != NULL){
            ui_el_set_text_basics(0,text_size,text_color);
            ui_el_set_text(text, (bool_s8)is_static);
        }
    ui_el_close();
    return on_hover;
}
static inline bool_s8 GUI_Text(const char *text, u16 text_size,u32 text_color  ){
    return GUI_TextExt(text, text_size, text_color, 1,1);
}
static ColorBundle color_button = {0};
static ColorBundle color_button_lua = {0};

static ColorBundle color_button_selected = {0};
static ColorBundle color_button_none = {0};
static void GUI_CustomMenuUp(){
   
    Clay_ElementId id = ui_el_begin_config(GET_FREE_NAME);
    UICaling cal = UI_CALIGN(UI_CALIGN_LEFT, UI_CALIGN_CENTER, 16,UI_DIR_RIGHT);
    cal.padding =TYPEDEC(Clay_Padding) {16,16,16,16};
    ui_el_layout_config(UI_SIZING(UI_SIZING_GROW,0,0), UI_SIZING(UI_SIZING_FIXED,100,0), cal);
    ui_el_set_color(DARKGRAY_UNI(UniColor).uni);
    ui_el_end_config();
        ColorBundle cur_bundle = color_button;
        if(App_GetPage() == PAGE_LOCALIZATOIN)
            cur_bundle = color_button_selected;
        u16 button_ret = GUI_ButtonText("Localization",32,200, 60, cur_bundle, 4, 14);
        if (button_ret == LEFT_PRESSED)
            App_SetPage(PAGE_LOCALIZATOIN);
        if (button_ret == RIGHT_PRESSED)
            App_SetPage(PAGE_NONE);
    ui_el_close();
}
static void GUI_PageNone(){
    Clay_ElementId id = ui_el_begin_config(GET_FREE_NAME);
    UICaling cal = UI_CALIGN(UI_CALIGN_LEFT, UI_CALIGN_CENTER, 16,UI_DIR_RIGHT);
    ui_el_layout_config(UI_SIZING(UI_SIZING_GROW,0,0), UI_SIZING(UI_SIZING_GROW,0,0), cal);
    ui_el_set_color(BLACK_UNI(UniColor).uni);
    ui_el_end_config();
       GUI_Text("Please select tool page", 72, WHITE_UNI(UniColor).uni);
    ui_el_close();
}
static bool_s8 convert_temp_flag = 0;

static void GUI_LocalizationDropFile(u16 file_id){
    Files *files = App_GetFiles();

    Clay_ElementId id = ui_el_begin_config(GET_FREE_NAME);
    UICaling cal = UI_CALIGN(UI_CALIGN_LEFT, UI_CALIGN_CENTER, 16,UI_DIR_RIGHT);
    cal.padding =TYPEDEC(Clay_Padding) {16,16,16,16};
    ui_el_layout_config(UI_SIZING(UI_SIZING_GROW,0,0), UI_SIZING(UI_SIZING_FIT,50,0), cal);
    ui_el_set_color(BLACK_UNI(UniColor).uni);
    ui_el_border_config_flat(4, DARKGRAY_UNI(UniColor).uni, 1);
    ui_el_end_config();
        GUI_TextExt(files->path[file_id], 24, WHITE_UNI(UniColor).uni, 0,0);
        
        id = ui_el_begin_config(GET_FREE_NAME);
        ui_el_layout_config(UI_SIZING(UI_SIZING_GROW,0,0), UI_SIZING(UI_SIZING_GROW,0,0),UI_CALIGN(UI_CALIGN_RIGHT, UI_CALIGN_CENTER, 0,0));
        ui_el_end_config( );
            u16 button_ret = GUI_ButtonText("Convert to lua", 32, 100, 50, color_button_lua, 3, 6);
            if(button_ret == LEFT_PRESSED)convert_temp_flag = 1;
           
        ui_el_close();
    ui_el_close();
}

static void GUI_PageLocalization(){
    Clay_ElementId id = ui_el_begin_config(GET_FREE_NAME);
    UICaling cal = UI_CALIGN(UI_CALIGN_LEFT, UI_CALIGN_TOP, 16,UI_DIR_RIGHT);
    ui_el_layout_config(UI_SIZING(UI_SIZING_GROW,0,0), UI_SIZING(UI_SIZING_GROW,0,0), cal);
    ui_el_set_color(BLACK_UNI(UniColor).uni);
    ui_el_end_config();
        if(App_GetFiles()->size == 0){
            GUI_Text("Drop an CSV file", 72, WHITE_UNI(UniColor).uni);
        }else {
            GUI_LocalizationDropFile(0);
        }
    ui_el_close();
}

static void GUI_PageLogic(){
    switch(App_GetPage()){
        case PAGE_NONE:{
            UI_SceduleLayout(GUI_PageNone);
        }break;
        case PAGE_LOCALIZATOIN:{
           u32 new_files =GetDroppedFilesExt(App_GetFiles(), ".CSV");

           UI_SceduleLayout( GUI_PageLocalization);
        }break;
        default:{
            UI_SceduleLayout(GUI_PageNone);
        }
    }
}
//{0xD0, 0x9F ,0xD1 ,0x80, 0xD0, 0xB8,0xD0, 0xB2, 0xD0 ,0xB5 ,0xD1 ,0x82 ,0x20 ,0xD0 ,0xBC ,0xD0, 0xB8 ,0xD1 ,0x80};
int main (){

    color_button = COLORB(.colors[0] = BLACK_UNI(UniColor).uni, .colors[1] = WHITE_UNI(UniColor).uni);
    color_button_selected = COLORB(.colors[0] = BLACK_UNI(UniColor).uni, .colors[1] = GREEN_UNI(UniColor).uni);
    color_button_lua = COLORB(.colors[0] = DARKGRAY_UNI(UniColor).uni, .colors[1] = WHITE_UNI(UniColor).uni);
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);//
    InitWindow(1280, 720, "IDE");
    SetTargetFPS(60);
    
    char local_ru[245] = {0};
    ArenaC arena =MemoryArenaInit(MB(2));
    UI_Init(1280, 720, &arena);
    //Font f = LoadFontEx("LanaPixel.ttf", 108, NULL, 42720);
    //SetTextureFilter(f.texture,TEXTURE_FILTER_BILINEAR);
    while(!WindowShouldClose()){
        Input_HandleMouse();
        GUI_PageLogic();
        UI_SceduleLayout(GUI_CustomMenuUp);
        ClearBackground(BLACK);
        BeginDrawing();
           UI_Update(TYPEDEC(v2) {.raylib = GetMousePosition()} ,  GetScreenWidth(), GetScreenHeight() );
        EndDrawing();
        App_ChangePage();
        if(convert_temp_flag){
            if(IsFileExtension(files.path[0], ".CSV")){
                LocalizationTable *ltable =  load_localization_csv(files.path[0]); 
                export_all_languages(ltable, "localization");
                free(ltable);
            }
            convert_temp_flag= 0;
            App_ClearFiles();
        }
    }
    CloseWindow();
    return 0;
}