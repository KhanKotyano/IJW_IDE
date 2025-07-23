#ifndef UNI_UTIL_LIB
#define UNI_UTIL_LIB

#ifndef RAYLIB_PATH
#define RAYLIB_PATH "raylib.h"
#define RAYMATH_PATH "raymath.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <vadefs.h>
#include <stdarg.h>
#include <float.h>

//#ifdef _WIN32
//#include <tchar.h>
//#include <strsafe.h>
//#endif

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef int_fast32_t fast_int;
typedef uint_fast32_t fast_uint;
typedef s8 bool_s8;
typedef union string_8c {
  u64 id;
  struct {
    char string[8];
  };
}string_8c;
typedef union string_16c {
  u64 id[2];
  struct {
    char string[16];
  };
}string_16c;
typedef enum ENGINE_TYPE {
  ET_INSTANCE = 0,
  ET_ANIMATION,
  ET_COLLISION
}ENGINE_TYPE;
typedef u8 engine_inst_type;
typedef union InstanceId {
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
}InstanceId;
static uint_fast64_t __loop_cntr64 = 0;
#define repeat64(COUNT) for(__loop_cntr64 = 0; __loop_cntr64 < COUNT; __loop_cntr64++)
#define loop64(name,COUNT) for(uint_fast64_t name = 0; name < COUNT; name++)

#define loop(name, count) for(fast_int name = 0;name < count;name++)
#define loopa(name, count) for(fast_uint name = 0;name < count;name++)
#define loopr(name, count) for(fast_int name = count;name >= count;name--)
#define loop_small(name, count) for(u8 name = 0;name < (u8)count;name++)

static fast_uint __REPEAT_COUNTER__ = 0;
#define repeat(count) for(__REPEAT_COUNTER__ = 0; __REPEAT_COUNTER__< count;__REPEAT_COUNTER__++)

#define foreach(name, a) for(fast_uint name = 0;name < a.size;name++)
#define foreach_lua(name, a) for(fast_uint name = 1;name <= a.size;name++)


#if defined(__cplusplus)
#define TYPEDEC(type)      type
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define TYPEDEC(type)      (type)
#define DLLEXPORT __declspec(dllexport)
#endif
#define COLOR_UNI(type, r,g,b,a)  TYPEDEC(type){ r,g,b,a }   // Zero alpha
#define COLOR_NEW_TYPE(type, ctype,c_struct)  TYPEDEC(type){ (ctype)c_struct.r,(ctype)c_struct.g,(ctype)c_struct.b,(ctype)c_struct.a }   // Zero alpha

#define ALPHAZERO_UNI(type)  TYPEDEC(type){ 0, 0, 0, 0 }   // Zero alpha
#define LIGHTGRAY_UNI(type)  TYPEDEC(type){ 200, 200, 200, 255 }   // Light Gray
#define GRAY_UNI(type)       TYPEDEC(type){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY_UNI(type)   TYPEDEC(type){ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW_UNI(type)     TYPEDEC(type){ 253, 249, 0, 255 }     // Yellow
#define GOLD_UNI(type)       TYPEDEC(type){ 255, 203, 0, 255 }     // Gold
#define ORANGE_UNI(type)     TYPEDEC(type){ 255, 161, 0, 255 }     // Orange
#define PINK_UNI(type)       TYPEDEC(type){ 255, 109, 194, 255 }   // Pink
#define RED_UNI(type)        TYPEDEC(type){ 230, 41, 55, 255 }     // Red
#define MAROON_UNI(type)     TYPEDEC(type){ 190, 33, 55, 255 }     // Maroon
#define GREEN_UNI(type)      TYPEDEC(type){ 0, 228, 48, 255 }      // Green
#define LIME_UNI(type)       TYPEDEC(type){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN_UNI(type)  TYPEDEC(type){ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE_UNI(type)    TYPEDEC(type){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE_UNI(type)       TYPEDEC(type){ 0, 121, 241, 255 }     // Blue
#define DARKBLUE_UNI(type)   TYPEDEC(type){ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE_UNI(type)     TYPEDEC(type){ 200, 122, 255, 255 }   // Purple
#define VIOLET_UNI(type)     TYPEDEC(type){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE_UNI(type) TYPEDEC(type){ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE_UNI(type)      TYPEDEC(type){ 211, 176, 131, 255 }   // Beige
#define BROWN_UNI(type)      TYPEDEC(type){ 127, 106, 79, 255 }    // Brown
#define DARKBROWN_UNI(type)  TYPEDEC(type){ 76, 63, 47, 255 }      // Dark Brown

#define WHITE_UNI(type)      TYPEDEC(type){ 255, 255, 255, 255 }   // White
#define BLACK_UNI(type)      TYPEDEC(type){ 0, 0, 0, 255 }         // Black
#define BLANK_UNI(type)      TYPEDEC(type){ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA_UNI(type)    TYPEDEC(type){ 255, 0, 255, 255 }     // Magenta

#define EPSILON 0.000001f

#define EqualMemory_Util(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define MoveMemory_Util(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define CopyMemory_Util(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define FillMemory_Util(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define ZeroMemory_Util(Destination,Length) memset((Destination),0,(Length))

#define V2TOV2(ntype, v)TYPEDEC(ntype){.x = v.x, .y = v.y}
#define V3TOV3(ntype, v)TYPEDEC(ntype){.x = v.x, .y = v.y, .z = v.z}
#define V4TOV4(ntype, v)TYPEDEC(ntype){.x = v.x, .y = v.y, .z = v.z, .w = v.z}

#define DEC_ZERO_C(_type)TYPEDEC(_type){0}
#define DEC_ZERO_CPP(_type)TYPEDEC(_type){}

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertf(A, M, ...) if(!(A)) {log_error(M, ##__VA_ARGS__); assert(A); }

#define get_percent(val, max_val) (float)((float)val / (float) max_val)



//binary

#define FLAG_SET(value, flag) ((value) | (flag))
#define FLAG_GET(value, flag)  ((value) & (flag))
#define FLAG_CLEAR(flag)  (&~(flag))
#define FLAG_GET_NONE(flag)  (~(flag))









#ifdef __cplusplus
#define CPP_IS_GAY(type, name, ...)type name = {};
#else
#define CPP_IS_GAY(type, name, ...)type name = {__VA_ARGS__};
#endif

#define STRING_EQUAL(str1, str2) (!(strcmp((str1), (str2))))
//typedefs

#define ARRAY_DEC(type) typedef struct Array_##type { type *buffer; u32 size; u32 alloc_size; u32 last_free; }Array_##type;
#define ARRAY_T(type) Array_##type




static inline void swap(int* a, int* b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
#define ARRAY_SORT_FUNCTION(type, MAX, Z)\
static inline int __internal_##type##_partition(type *arr, int low, int high) {\
  type pivot = arr[high];\
  int i = low - 1;\
  for (int j = low; j < high; ++j) {\
    if ((arr[j].Z - pivot.Z) < 0) {\
      ++i;\
      type tmp = arr[i];\
      arr[i] = arr[j];\
      arr[j] = tmp;\
    }\
  }\
  type tmp = arr[i + 1];\
  arr[i + 1] = arr[high];\
  arr[high] = tmp;\
  return i + 1;\
}\
static inline void Array_##type##_sort(ARRAY_T(type) a) {\
  int *stack = (int *)MemoryRequestEphemeral(MAX);\
  int top = -1;\
  stack[++top] = 0;\
  stack[++top] = a.size - 1;\
  while (top >= 0) {\
    int high = stack[top--];\
    int low = stack[top--];\
    while (low < high) {\
      int pi = __internal_##type##_partition(a.buffer, low, high);\
      if (pi - 1 - low > high - (pi + 1)) {\
        stack[++top] = low;\
        stack[++top] = pi - 1;\
        low = pi + 1;\
      }\
      else {\
        stack[++top] = pi + 1;\
        stack[++top] = high;\
        high = pi - 1;\
      }\
    }\
  }\
}


static inline bool_s8 is_string_equal_16c( const char *tstring ,string_16c string) {
  #ifdef DEBUG_MODE 
    assertf(tstring != NULL, "String should not be NULL!\n")
  #endif
  if(tstring == NULL)return 0;
  string_16c new_string = {0};
  loop(i, 16) {
    string.string[i] = tstring[i];
  }
  return string.id[0] == new_string.id[0] && string.id[1] == new_string.id[1];
}
static inline bool_s8 is_string_equal_8c(const char* tstring, string_8c string) {
#ifdef DEBUG_MODE 
  assertf(tstring != NULL, "String should not be NULL!\n")
#endif
    if (tstring == NULL)return 0;
  string_8c new_string = { 0 };
  loop(i, 8) {
    string.string[i] = tstring[i];
  }
  return string.id == new_string.id;
}
static inline bool_s8 is_8c_equal_8c(string_8c tstring, string_8c string) {
  return string.id == tstring.id;
}
static inline bool_s8 is_16c_equal_16c(string_16c tstring, string_16c string) {
  return string.id[0] == tstring.id[0] && string.id[1] == tstring.id[1];
}

#define ARRAY_FUNCTIONS(type) \
static inline u32 Array_##type##_push(ARRAY_T(type) *a, type el) {\
  u32 id = FAILED_U32;\
  if (a->buffer != NULL && a->size < a->alloc_size) {\
    memcpy(&(a->buffer[a->size]), &el, sizeof(type));\
    id = a->size;\
    a->size++;\
  }\
  return id;\
};\
static void Array_##type##_set(ARRAY_T(type) *a, type el, u32 id) {\
  if (a->buffer != NULL && id < a->alloc_size) {\
    if(id >= a->size)a->size = id+1;\
    memcpy(&(a->buffer[id]), &el, sizeof(type));\
  }\
};\
static type *Array_##type##_get(ARRAY_T(type) a, u32 id) {\
  if (a.buffer != NULL && id >= 0 && id < a.size) {\
    return &(a.buffer[id]);\
  }\
  return NULL;\
};\
static type *Array_##type##_pop(ARRAY_T(type)* a) {\
  type *_ret = NULL;\
  if (a->buffer != NULL && a->size) {\
    a->size--;\
    _ret = &(a->buffer[a->size]);\
  }\
  return _ret;\
};\
static void Array_##type##_init(ARRAY_T(type)* a, u32 size,void *memory) {\
  printf("[INFO::UTILITIES]Array of type " #type " initialized with %d elements\n", size);\
  a->alloc_size = size;\
  a->size = 0;\
  a->last_free = FAILED_U32;\
  a->buffer = (type*)memory;\
};\
static void Array_##type##_init_arena(ARRAY_T(type)* a, u32 size, ArenaC *arena) {\
  printf("[INFO::UTILITIES]Array of type " #type " initialized with %d elements\n", size);\
  a->alloc_size = size;\
  a->size = 0;\
  a->last_free = FAILED_U32;\
  a->buffer = (type*)MemoryRequest( sizeof(type) * size, arena);\
};\
static void Array_##type##_copy(ARRAY_T(type)* to, type* from, size_t from_size) {\
  memcpy(to->buffer, from, from_size * sizeof(type));\
  to->size = from_size;\
}\
static void Array_##type##_free_el(ARRAY_T(type)* array, u32 el_id) {\
  if (array->buffer != NULL && el_id < array->size) {\
    ZeroMemory_Util(&(array->buffer[el_id]), sizeof(type));\
    if (el_id < array->last_free)array->last_free = el_id;\
  }\
};\
static inline u32 Array_##type##_set_free(ARRAY_T(type) *a, type el, u32(*func)(ARRAY_T(type))) {\
  u32 id = FAILED_U32;\
  if (a->buffer != NULL ) {\
    a->last_free = func(*a);\
    if (a->last_free >= a->size) {\
      a->last_free = a->size;\
      if(a->last_free >= a->alloc_size)return id;\
      a->size++; \
    }\
    id = a->last_free;\
    memcpy(&(a->buffer[a->last_free]), &el, sizeof(type));\
  }\
  return id;\
};\
static inline bool_s8 Array_##type##_is_full(ARRAY_T(type) a) {\
  bool_s8 r = 0;\
  if(a.size >= a.alloc_size)r = 1; \
  return r ;\
};\
static inline type* Array_##type##_find_bytes(ARRAY_T(type) a, void* b, size_t bsize) {\
  /*assertf(a.buffer != NULL, "buffer is NULL");\*/\
  loopa(i, a.size) { if (memcmp(&a.buffer[i], b, bsize) == 0) return &a.buffer[i]; }\
  return NULL;\
}\
static inline void Array_##type##_clear(ARRAY_T(type) *a) {\
  ZeroMemory_Util(a->buffer, sizeof(type) * a->alloc_size);\
  a->size = 0;\
}





#define ARRAY_FUNCTIONS_N(type, failed_val) \
static u32 Array_##type##_push(ARRAY_T(type) *a, type el) {\
  u32 id = FAILED_U32; \
  if (a->buffer != NULL && a->size < a->alloc_size) {\
    memcpy(&(a->buffer[a->size]), &el, sizeof(type));\
    id = a->size;\
    a->size++;\
  }\
  return id;\
};\
static void Array_##type##_set(ARRAY_T(type) *a, type el, u32 id) {\
  if (a->buffer != NULL && id < a->alloc_size) {\
    if(id >= a->size)a->size = id+1;\
    memcpy(&(a->buffer[id]), &el, sizeof(type));\
  }\
};\
static type Array_##type##_get(ARRAY_T(type) a, u32 id) {\
  if (a.buffer != NULL && id >= 0 && id < a.size) {\
    return (a.buffer[id]);\
  }\
  return failed_val;\
};\
static type Array_##type##_pop(ARRAY_T(type)* a) {\
  type _ret = failed_val;\
  if (a->buffer != NULL && a->size) {\
    a->size--;\
    _ret = (a->buffer[a->size]);\
  }\
  return _ret;\
};\
static void Array_##type##_init(ARRAY_T(type)* a, u32 size,void *memory) {\
  a->alloc_size = size;\
  a->size = 0;\
  a->last_free = 0;\
  a->buffer = (type*)memory;\
};\
static void Array_##type##_copy(ARRAY_T(type)* to, type* from, type from_size) {\
  memcpy(to->buffer, from, from_size * sizeof(type));\
  to->size = from_size;\
}\
static void Array_##type##_free_el(ARRAY_T(type)* array, type el_id) {\
  if (array->buffer != NULL && el_id < array->size) {\
    array->buffer[el_id] = failed_val;\
    if (el_id < array->last_free)array->last_free = el_id;\
  }\
};\
static inline u32 Array_##type##_find_free(ARRAY_T(type) a) {\
  u32 id = 0;\
  loopa(i, a.size) {\
    if (a.buffer[i] == failed_val)id = i;\
  }\
  return id = 0;\
}\
static inline void Array_##type##_set_free(ARRAY_T(type) *a, type el) {\
  if (a->buffer != NULL && a->size < a->alloc_size) {\
    if (a->last_free == 0 || a->last_free >= a->size) {\
      a->last_free = a->size;\
      a->size++;\
    }\
    memcpy(&(a->buffer[a->last_free]), &el, sizeof(type));\
    a->last_free = Array_##type##_find_free(*a);\
  }\
};



#define ARRAY_ADD(type, array_v,value)bool_8 __array_##type##_add( array_v, value ){ if() }

#define FAILED_S8   (INT8_MIN)
#define FAILED_S16  (INT16_MIN)
#define FAILED_S32  (INT32_MIN)
#define FAILED_S64  (INT64_MIN)
#define FAILED_U8   ((u8)-1)
#define FAILED_U16  ((u16)-1)
#define FAILED_U32  ((u32)-1)
#define FAILED_U64  ((u64)-1)
#define FAILED_PTR  (NULL)

#define STR_NOT_FOUND ((u32)-1)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
ARRAY_DEC(u32);
ARRAY_FUNCTIONS_N(u32, FAILED_U32)
ARRAY_DEC(u64);
ARRAY_FUNCTIONS_N(u64, FAILED_U64);
#pragma clang diagnostic pop

#define ARRAY_FUNCTION_FREE(type, var) \
static inline u32 Array_##type##_get_free_id(ARRAY_T(type) a) {\
  u32 id = FAILED_U32;\
  if (a.last_free < a.alloc_size && a.buffer[a.last_free].var <= 0)return a.last_free;\
  for( ; a.last_free < a.alloc_size;a.last_free++)if(a.buffer[a.last_free].var <= 0)return  a.last_free;\
  if (a.size < a.alloc_size)return a.size;\
  return id;\
}

static inline u32 StringMatch(const char* string, const char** mlist, u32 mlist_size, bool_s8 exclude) {
  u32 ret = STR_NOT_FOUND;
  loopa(i, mlist_size) {
    if (mlist[i] != NULL && STRING_EQUAL(string, mlist[i])) {
      ret = (u32)i;
      break;
    }
  }
  if (exclude && ret != STR_NOT_FOUND) mlist[ret] = NULL;
  return ret;
}

typedef u32 EventID;

typedef union {
  u64 num;
  u32 au32[2];
  u16 au16[4];
  u8  au8[8];

}UniNum64;

typedef union {
  u8 array[4];
  u8 array_u24[3];
  struct { u8 r; u8 g; u8 b; u8 a; };
  u32 uni;
}UniColor;
#define U32_TO_UNICOLOR(_col)(UniColor){.uni = _col}

typedef struct {
  const u8* buffer;
  size_t size;
} StringConstU;


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#define memsetv __memset64
#ifdef UNI_UTIL_LIB_IMPLEMENTATION



static void __memset64(void* dest, uint64_t value, uintptr_t size) {
  uintptr_t i;
  for (i = 0; i < (size & (~7)); i += 8)memcpy(((char*)dest) + i, &value, 8);
  for (; i < size; i++) ((char*)dest)[i] = ((char*)&value)[i & 7];
}
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define MAX max

typedef enum {
  ML_INFO,
  ML_WARNING,
  ML_ERROR,
  ML_CRASH,
  ML_SIZE
}MESSAGE_LEVEL;

static inline void ShowMessageModule(const char* module, MESSAGE_LEVEL level, const char* msg) {
  static const char* message_level[ML_SIZE] = { "INFO" , "WARNING" , "ERROR", "CRITICAL_ERROR" };
  if (level >= 0 && level < ML_SIZE)
    printf("[%s::%s]:%s\n", message_level[level], module, msg);
}

#define InRange(val,min,max)(bool_s8)(((val ) >= (min)) && ((val) <=(max)))

///-------------------TEMP------------------------//
static const char* StringFormat(const char* text, ...) {
#ifndef MAX_TEXTFORMAT_BUFFERS
#define MAX_TEXTFORMAT_BUFFERS      4        // Maximum number of static buffers for text formatting
#endif
#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH   1024        // Maximum size of static text buffer
#endif
  static char buffers[MAX_TEXTFORMAT_BUFFERS][MAX_TEXT_BUFFER_LENGTH] = { 0 };
  static int index = 0;
  char* currentBuffer = buffers[index];
  memset(currentBuffer, 0, MAX_TEXT_BUFFER_LENGTH);   // Clear buffer before using
  va_list args;
  va_start(args, text);
  int requiredByteCount = vsnprintf(currentBuffer, MAX_TEXT_BUFFER_LENGTH, text, args);
  va_end(args);
  if (requiredByteCount >= MAX_TEXT_BUFFER_LENGTH)
  {
    char* truncBuffer = buffers[index] + MAX_TEXT_BUFFER_LENGTH - 4; // Adding 4 bytes = "...\0"
    sprintf(truncBuffer, "...");
  }

  index += 1;     // Move to next buffer for next function call
  if (index >= MAX_TEXTFORMAT_BUFFERS) index = 0;

  return currentBuffer;
}
static inline const char* SaveConstString(const char* string, u16 length) {
  static char buffers[2000][256] = { 0 };
  static u16 index = 0;
  u16 str_size = length;
  if (str_size == 0) str_size = (u16)strlen(string);
  if (str_size > 256 || index >= 1000 || string == NULL) {
#ifdef DEBUG_MODE
    printf("[CRITICAL_ERROR:SaveConstString]: Function failed, size of the string is too big\n");
    exit(1);
#endif
    return NULL;
  }
  char* current_buffer = buffers[index];
  loop(i, str_size) {
    current_buffer[i] = string[i];
  }
  index++;
  return current_buffer;
}

static time_t get_file_last_modtime(const char* filename) {
  struct stat file_stat;

  if (stat(filename, &file_stat) != 0) {
    perror("Could not retrieve file info");
    return (time_t)-1;
  }

  return file_stat.st_mtime;
}
static const char* file_to_string(const char* filename, char* memory, size_t* memorysize) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    perror(StringFormat("Failed to open file: [%s]", filename));
    return NULL;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek file");
    fclose(file);
    return NULL;
  }
  size_t filesize = ftell(file);
  if (filesize < 0) {
    perror("Failed to get file size");
    fclose(file);
    return NULL;
  }
  if (filesize >= *memorysize) {
    printf("[Error]: there is not enought memory to be allocated\n");
    fclose(file);
    return NULL;
  }
  rewind(file);
  size_t read_size = fread(memory, 1, filesize, file);
  if (read_size != filesize) {
    perror("Failed to read complete file");
    fclose(file);
    return NULL;
  }
  memory[filesize] = '\0';
  fclose(file);
  return memory;
}
static size_t file_size(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    perror("Failed to open file");
    return 0;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek file");
    fclose(file);
    return 0;
  }
  size_t filesize = ftell(file);
  if (filesize < 0) {
    perror("Failed to get file size");
    fclose(file);
    return 0;
  }
  fclose(file);
  return filesize;
}



static inline uintmax_t ConvertStringToNumber(char* s) {
  uintmax_t num = strtoumax(s, NULL, 10);
  if (num == UINTMAX_MAX && errno == ERANGE) { printf("Conversion error has occured\n"); }
  return num;
}
static inline uintmax_t ConvertHexStringToNumber(char* s) {
  uintmax_t num = strtoumax(s, NULL, 16);
  if (num == UINTMAX_MAX && errno == ERANGE) { printf("Conversion error has occured\n"); }
  return num;
}


#endif

#ifdef RAYLIB_PATH
#include RAYLIB_PATH
#include RAYMATH_PATH

#define DEBUG_MODE 1
#define DIRECT_PATH 1
#ifndef RESOURCES_PATH
#define RESOURCES_PATH "\\resources"
#endif
#if DIRECT_PATH
#define ASSET_PATH RESOURCES_PATH
#else
#define ASSET_PATH RESOURCES_PATH
#endif

#define SPRITE_EXTENSION ".png"
#define LOAD_PATH_SPRITE ASSET_PATH "sprites/"
#define PATH_MERGE(root,name) root #name
#define CONST_STRING(string) #string
#define PATH_MERGE_STR(root,name) root name

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))
#define _JOIN(prefix, name) PASTE(_, PASTE(prefix, PASTE(_, name)))

#define TEXTURE_GROUP_INIT(name) static name JOIN(TG,name) = {0}
#define TEXTURE_GROUP_GET_NAME(name) JOIN(TG,name)
#define TEXTURE_GROUP_START(name) typedef struct JOIN(TS,name){
#define TEXTURE_GROUP_END(name) }JOIN(TS,name); \
        typedef union name { \
          JOIN(TS,name) tstruct;  \
          Texture2D tarray[sizeof(JOIN(TS,name)) / sizeof(Texture2D)];  \
        };\
        static name* PASTE(Get, name)(){ \
          TEXTURE_GROUP_INIT(name); \
           return &TEXTURE_GROUP_GET_NAME(name); \
        }
#define SELF_UNION(name, type) typedef union JOIN(name,u) {name tstruct;type tarray[sizeof(name)/sizeof(type)];} JOIN(name,u)

#define GET_TG_FUNCTION(name) PASTE(Get, name)()
#define GET_TEXTURE_BY_NAME(tgroup, tname) (GET_TG_FUNCTION(tgroup)->tstruct.tname)
#define GET_TEXTURE_BY_ID(tgroup, tID) (GET_TG_FUNCTION(tgroup)->tarray[tID])


#define TEXTURE_PRELOAD(name) Texture2D name 


#define LOAD_TEXTURE_2D(name) Texture2D name = LoadTexture(PATH_MERGE(LOAD_PATH_SPRITE, name)SPRITE_EXTENSION)
#define LOAD_TEXTURE_IG(g_name, t_name) GET_TG_FUNCTION(g_name)->tstruct.t_name = LoadTexture(PATH_MERGE(LOAD_PATH_SPRITE, t_name)SPRITE_EXTENSION)
#define LOAD_TEXTURE_IG_ID(g_name, t_number) GET_TG_FUNCTION(g_name)->tarray[t_number] = LoadTexture(PATH_MERGE(LOAD_PATH_SPRITE, t_name)SPRITE_EXTENSION)


#define CLAY_COLOR_TO_UNI_COLOR(color) TYPEDEC(UniColor) { .r = (unsigned char)roundf(color.r), .g = (unsigned char)roundf(color.g), .b = (unsigned char)roundf(color.b), .a = (unsigned char)roundf(color.a) }
#define UNI_COLOR_TO_CLAY_COLOR(color) TYPEDEC(Clay_Color) { .r = (float)(color.r), .g = (float)(color.g), .b = (float)(color.b), .a = (float)(color.a) }

#define inline_assembly __asm
#endif
#ifndef KMODULE_V2
#define KMODULE_V2
#if defined(__cplusplus)
extern "C" {
#endif

#pragma region INCLUDE_HMM
#define HMM_PI 3.14159265358979323846
#define HMM_PI32 3.14159265359f
#define HMM_DEG180 180.0
#define HMM_DEG18032 180.0f
#define HMM_TURNHALF 0.5
#define HMM_TURNHALF32 0.5f
#define HMM_RadToDeg ((float)(HMM_DEG180/HMM_PI))
#define HMM_RadToTurn ((float)(HMM_TURNHALF/HMM_PI))
#define HMM_DegToRad ((float)(HMM_PI/HMM_DEG180))
#define HMM_DegToTurn ((float)(HMM_TURNHALF/HMM_DEG180))
#define HMM_TurnToRad ((float)(HMM_PI/HMM_TURNHALF))
#define HMM_TurnToDeg ((float)(HMM_DEG180/HMM_TURNHALF))

  typedef union HMM_Vec2 {
    struct { float X, Y; };
    struct { float U, V; };
    struct { float Left, Right; };
    struct { float Width, Height; };
    struct { float x, y; };
    float Elements[2];
#ifdef __cplusplus
    inline float& operator[](int Index) { return Elements[Index]; }
    inline const float& operator[](int Index) const { return Elements[Index]; }
#endif
  } HMM_Vec2;

  typedef union HMM_Vec3 {
    struct { float X, Y, Z; };
    struct { float U, V, W; };
    struct { float R, G, B; };
    struct { HMM_Vec2 XY; float _Ignored0; };
    struct { float _Ignored1; HMM_Vec2 YZ; };
    struct { HMM_Vec2 UV; float _Ignored2; };
    struct { float _Ignored3; HMM_Vec2 VW; };
    float Elements[3];
#ifdef __cplusplus
    inline float& operator[](int Index) { return Elements[Index]; }
    inline const float& operator[](int Index) const { return Elements[Index]; }
#endif
  } HMM_Vec3;

  typedef union HMM_Vec4 {
    struct { union { HMM_Vec3 XYZ; struct { float X, Y, Z; }; }; float W; };
    struct { union { HMM_Vec3 RGB; struct { float R, G, B; }; }; float A; };
    struct { HMM_Vec2 XY; float _Ignored0; float _Ignored1; };
    struct { float _Ignored2; HMM_Vec2 YZ; float _Ignored3; };
    struct { float _Ignored4; float _Ignored5; HMM_Vec2 ZW; };
    float Elements[4];
#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSE;
#endif
#ifdef HANDMADE_MATH__USE_NEON
    float32x4_t NEON;
#endif
#ifdef __cplusplus
    inline float& operator[](int Index) { return Elements[Index]; }
    inline const float& operator[](int Index) const { return Elements[Index]; }
#endif
  } HMM_Vec4;

  typedef union HMM_Mat2 {
    float Elements[2][2];
    HMM_Vec2 Columns[2];
#ifdef __cplusplus
    inline HMM_Vec2& operator[](int Index) { return Columns[Index]; }
    inline const HMM_Vec2& operator[](int Index) const { return Columns[Index]; }
#endif
  } HMM_Mat2;

  typedef union HMM_Mat3 {
    float Elements[3][3];
    HMM_Vec3 Columns[3];
#ifdef __cplusplus
    inline HMM_Vec3& operator[](int Index) { return Columns[Index]; }
    inline const HMM_Vec3& operator[](int Index) const { return Columns[Index]; }
#endif
  } HMM_Mat3;

  typedef union HMM_Mat4 {
    float Elements[4][4];
    HMM_Vec4 Columns[4];
#ifdef __cplusplus
    inline HMM_Vec4& operator[](int Index) { return Columns[Index]; }
    inline const HMM_Vec4& operator[](int Index) const { return Columns[Index]; }
#endif
  } HMM_Mat4;

  typedef union HMM_Quat {
    struct { union { HMM_Vec3 XYZ; struct { float X, Y, Z; }; }; float W; };
    float Elements[4];
#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSE;
#endif
#ifdef HANDMADE_MATH__USE_NEON
    float32x4_t NEON;
#endif
  } HMM_Quat;

#pragma endregion


#define DRAWING_DIRECTION_OFFSET 90
#define ZERO_V2 CLITERAL(v2){0.0f,0.0f}

#ifndef PI_32f
#define PI_32f 3.14159265359f
#endif
#define DTR ((float)(PI_32f/180.0f))
#define DegInRad(a) (a * (DTR))

#define DEC_V2(X,Y) CLITERAL(v2){X, Y}
#define DEC_Vector2(X,Y) CLITERAL(Vector2){X, Y}
#define MATH_SQRTF sqrtf


  typedef union {
    struct {
      u32 x;
      u32 y;
    };
    u32 element[2];
    u64 xy;
  }GridVector2D;
  typedef union {
    struct {
      u16 x;
      u16 y;
    };
    u16 element[2];
    u32 xy;
  }iV2_16;
  typedef union {
    struct {
      u8 x;
      u8 y;
    };
    u8 element[2];
    u16 xy;
  }iV2_8;


#define iV2 GridVector2D
  typedef iV2_16 iv2;
  typedef u32 iv2_id;
  typedef u16 iv2_s;
  //#define iv2 iV2_16
  //#define iv2_id u32
  //#define iv2_s u16

#define distance(X1,Y1,X2,Y2)(sqrtf((float)(((s32)X2 - (s32)X1) * ((s32)X2 - (s32)X1)) + (float)(((s32)Y2 - (s32)Y1) * ((s32)Y2 - (s32)Y1))))
#define distance_v(V1,V2)(sqrtf((float)(((float)((V2).x) - (float)((V1).x)) * ((float)((V2).x) - (float)((V1).x))) + (float)(((float)((V2).y) - (float)((V1).y)) * ((float)((V2).y )- (float)((V1).y)))))
  static inline iV2_16 id_to_xy_16(u32 id, u16 w) {
    iV2_16 v = { 0 };
    v.y = (u16)(id / w);
    v.x = id - (v.y * w);
    return v;
  }
  static inline u32 xy_to_id_16(iV2_16 v, u16 w) {
    u32 id = 0;
    id = v.y * w + v.x;
    return id;
  }static inline iV2 id_to_xy_32(u32 id, u16 w) {
    iV2 v = { 0 };
    v.y = (u32)(id / w);
    v.x = id - (v.y * w);
    return v;
  }
  static inline u32 xy_to_id_32(iV2 v, u16 w) {
    u32 id = 0;
    id = v.y * w + v.x;
    return id;
  }
#define xy_to_id(v, w) xy_to_id_16(v,w)
#define id_to_xy(v, w) id_to_xy_16(v,w)

  typedef struct {
    u32* data;
    u32 w;
    u32 h;
  }Grid2D;

  typedef struct {
    iV2* data;
    u32 w;
    u32 h;
  }GridV2;

  typedef union v2 {
    HMM_Vec2 hmm;
    Vector2 raylib;
    struct { float x, y; };
    float a[2];
    double xy;
    u64 xy_u;
  }v2;

  typedef union v3 {
    HMM_Vec3 hmm;
    v2 v2;
    Vector3 raylib;
    struct { float x, y, z; };
    float a[3];
  }v3;

  typedef union v4 {
    HMM_Vec4 hmm;
    Vector4 raylib;
    struct { float x, y, z, w; };
    float a[4];
    v2 v[2];
    v3 v3;
  }v4;

#ifdef KM_V2_IMPLEMENTATION


  static inline float MathClamp(float Min, float Value, float Max) {
    float Result = Value;
    if (Result < Min)
    {
      Result = Min;
    }
    if (Result > Max)
    {
      Result = Max;
    }
    return Result;
  }

  static inline v2 AddV2(v2 a, v2 b) {
    return DEC_V2(a.x + b.x, a.y + b.y);
  }




  static inline v2 MultV2(v2 a, v2 b) {
    return DEC_V2(a.x * b.x, a.y * b.y);
  }
  static inline v2 MultV2F(v2 a, float f) {
    return DEC_V2(a.x * f, a.y * f);
  }
  static inline v2 LerpV2(v2 A, float Time, v2 B) {
    return AddV2(MultV2F(A, 1.0f - Time), MultV2F(B, Time));
  }
  static inline v2 SubV2(v2 a, v2 b) {
    return DEC_V2(a.x - b.x, a.y - b.y);
  }
  static inline v2 V2Sign(v2 v) {
    v2 result = ZERO_V2;
    if (v.x > 0.0f) {
      result.x = 1.0f;
    }
    else if (v.x < 0.0f) {
      result.x = -1.0f;
    }
    if (v.y > 0.0f) {
      result.y = 1.0f;
    }
    else if (v.y < 0.0f) {
      result.y = -1.0f;
    }
    return result;
  }
  static inline float SignF(float a) {
    float  result = 0.0f;
    if (a > 0.0f) {
      result = 1.0f;
    }
    else if (a < 0.0f) {
      result = -1.0f;
    }
    return result;
  }

  static inline float V2Length(v2 v) {
    return sqrtf((v.x * v.x) + (v.y * v.y));
  }
  static inline v2 V2Negative(v2 v) {
    return DEC_V2((v.x * -1), (v.y * -1));
  }
  static inline int V2Equal(v2 a, v2 b) {
    return (fabsf(a.x - b.x) < FLT_EPSILON) && (fabsf(a.y - b.y) < FLT_EPSILON);
  }
  static inline Vector2 AddV2Raylib(v2 a, v2 b) {
    return DEC_Vector2(a.x + b.x, a.y + b.y);
  }
  static inline v2 AddRaylibV2(Vector2 a, Vector2 b) {
    return DEC_V2(a.x + b.x, a.y + b.y);
  }
  static inline float KCleanF(float a) {
    int b = a * 100;
    a = (float)b / 100.0f;
    return a;
  }
  static inline v2 KCleanV2(v2 vector) {
    int x = vector.x * 100;
    int y = vector.y * 100;
    vector.y = (float)y / 100.0f;
    vector.x = (float)x / 100.0f;
    return vector;
  }
  static inline float V2LineAngle(v2 start, v2 end) {
    return -atan2f(start.y - end.y, start.x - end.x);
  }
  static inline float DegreeOpposite(float _deg) {
    return 360 - _deg;
  }
  static inline float RotationInDirection(v2 from, v2 to) {
    return (DRAWING_DIRECTION_OFFSET - (V2LineAngle(to, from) * HMM_RadToDeg));
  }

  static inline v2 KRotateV2(v2 v0, float f) {
    v2 result = ZERO_V2;
    f = DegInRad(f);
    float cs = cosf(f);
    float sn = sinf(f);
    result.x = v0.x * cs - v0.y * sn;
    result.y = v0.x * sn + v0.y * cs;
    return result;
  }
  static inline float V2Dot(v2 a, v2 b) {
    return (a.x * b.x) + (a.y * b.y);
  }
  static inline v2 V2Project(v2 a, v2 b) {
    float s = V2Dot(a, b) / (V2Dot(b, b));
    return DEC_V2((b.x * s), (b.y * s));
  }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstatic-in-inline"

  static unsigned int g_seed;
  //static float f_seed;

  // Used to seed the generator.           
  static inline void fast_srand(int seed) {
    g_seed = seed;
  }

  // Compute a pseudorandom integer.
  // Output value in range [0, 32767]
  static inline int fast_rand(void) {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
  }
  static float fi_rsqrt(float number) {
    union {
      float    f;
      uint32_t i;
    } conv;//= { .f = number };
    conv.f = number;
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
    return conv.f;
  }
  static inline v2 V2Normalize(v2 v) {
    float length = fi_rsqrt((v.x * v.x) + (v.y * v.y));
    v.x = v.x * length;
    v.y = v.y * length;
    return v;
  }

  static inline int F32Equals(float x, float y) {
    return (fabsf(x - y)) <= (EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));;
  }

  static inline float SignF32(float a) {
    float  result = 0.0f;
    if (a > 0.0f) { result = 1.0f; }
    else if (a < 0.0f) { result = -1.0f; }
    return result;
  }
  static inline float ClampF32(float Min, float Value, float Max) {
    float Result = Value;
    if (Result < Min)Result = Min;
    if (Result > Max)Result = Max;
    return Result;
  }

  static iV2 GetGridOnHover_32(float _mouse_world_pos_x, float _mouse_world_pos_y, unsigned int grid_width, unsigned int grid_height,
    float grid_world_offset_x, float grid_world_offset_y, int pixel_size) {
    iV2 on_hover_grid = { 0 };
    on_hover_grid.x = (((unsigned int)_mouse_world_pos_x - (unsigned int)grid_world_offset_x) / pixel_size) * ((unsigned int)_mouse_world_pos_x >= 0);
    on_hover_grid.y = (((unsigned int)_mouse_world_pos_y - (unsigned int)grid_world_offset_y) / pixel_size) * ((unsigned int)_mouse_world_pos_y >= 0);
    if (on_hover_grid.x >= grid_width) on_hover_grid.x = grid_width - 1;
    if (on_hover_grid.y >= grid_height) on_hover_grid.y = grid_height - 1;
    return on_hover_grid;
  }
  static iV2_16 GetGridOnHover_16(float _mouse_world_pos_x, float _mouse_world_pos_y, u16 grid_width, u16 grid_height,
    float grid_world_offset_x, float grid_world_offset_y, int pixel_size) {
    iV2_16 on_hover_grid = { 0 };
    on_hover_grid.x = (((u16)_mouse_world_pos_x - (u16)grid_world_offset_x) / pixel_size) * ((u16)_mouse_world_pos_x >= 0);
    on_hover_grid.y = (((u16)_mouse_world_pos_y - (u16)grid_world_offset_y) / pixel_size) * ((u16)_mouse_world_pos_y >= 0);
    if (on_hover_grid.x >= grid_width) on_hover_grid.x = grid_width - 1;
    if (on_hover_grid.y >= grid_height) on_hover_grid.y = grid_height - 1;
    return on_hover_grid;
  }
#define GetGridOnHover(mx, my, gw, gh, ps)GetGridOnHover_16((float)mx, (float)my, (u16)gw, (u16)gh, 0, 0, (int)ps)
#define GetGridOnHoverV(_v1, _v2, ps)GetGridOnHover(_v1.x,_v1.y, _v2.x, _v2.y, ps)
  static bool_s8 InRangeiV2(iV2 value, u32 min, u32 max) {
    bool_s8 ret = 0;
    ret = (bool_s8)(((value.x >= min) && (value.y >= min)) && ((value.x <= max) && (value.y <= max)));
    return ret;
  }
  static inline v2 make_normalized_vector(float _x1, float _y1, float _x2, float _y2) {
    v2 v = { 0 };
    v.x = _x2 - _x1;
    v.y = _y2 - _y1;
    v = V2Normalize(v);
    return v;
  }
  static inline v2 move_to_pos(v2 pos, v2 dest, float speed) {
    v2 npos = { .xy = pos.xy };
    v2 velocity = make_normalized_vector(pos.x, pos.y, dest.x, dest.y);
    npos.x += velocity.x * speed;
    npos.y += velocity.y * speed;
    return npos;
  }
#endif
#pragma clang diagnostic pop
#if defined(__cplusplus)
}
#endif
#endif 
#ifndef __AC_KHASH_H
#define __AC_KHASH_H

/*!
  @header

  Generic hash table library.
 */

#define AC_VERSION_KHASH_H "0.2.8"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

 /* compiler specific configuration */

#if UINT_MAX == 0xffffffffu
typedef unsigned int khint32_t;
#elif ULONG_MAX == 0xffffffffu
typedef unsigned long khint32_t;
#endif

#if ULONG_MAX == ULLONG_MAX
typedef unsigned long khint64_t;
#else
typedef unsigned long long khint64_t;
#endif

#ifndef kh_inline
#ifdef _MSC_VER
#define kh_inline __inline
#else
#define kh_inline inline
#endif
#endif /* kh_inline */

#ifndef klib_unused
#if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
#define klib_unused __attribute__ ((__unused__))
#else
#define klib_unused
#endif
#endif /* klib_unused */

typedef khint32_t khint_t;
typedef khint_t khiter_t;

#define __ac_isempty(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&2)
#define __ac_isdel(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&1)
#define __ac_iseither(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&3)
#define __ac_set_isdel_false(flag, i) (flag[i>>4]&=~(1ul<<((i&0xfU)<<1)))
#define __ac_set_isempty_false(flag, i) (flag[i>>4]&=~(2ul<<((i&0xfU)<<1)))
#define __ac_set_isboth_false(flag, i) (flag[i>>4]&=~(3ul<<((i&0xfU)<<1)))
#define __ac_set_isdel_true(flag, i) (flag[i>>4]|=1ul<<((i&0xfU)<<1))

#define __ac_fsize(m) ((m) < 16? 1 : (m)>>4)

#ifndef kroundup32
#define kroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#ifndef kcalloc
#define kcalloc(N,Z) calloc(N,Z)
#endif
#ifndef kmalloc
#define kmalloc(Z) malloc(Z)
#endif
#ifndef krealloc
#define krealloc(P,Z) realloc(P,Z)
#endif
#ifndef kfree
#define kfree(P) free(P)
#endif

static const double __ac_HASH_UPPER = 0.77;

#define __KHASH_TYPE(name, khkey_t, khval_t) \
	typedef struct kh_##name##_s { \
		khint_t n_buckets, size, n_occupied, upper_bound; \
		khint32_t *flags; \
		khkey_t *keys; \
		khval_t *vals; \
	} kh_##name##_t;

#define __KHASH_PROTOTYPES(name, khkey_t, khval_t)	 					\
	extern kh_##name##_t *kh_init_##name(void);							\
	extern void kh_destroy_##name(kh_##name##_t *h);					\
	extern void kh_clear_##name(kh_##name##_t *h);						\
	extern khint_t kh_get_##name(const kh_##name##_t *h, khkey_t key); 	\
	extern int kh_resize_##name(kh_##name##_t *h, khint_t new_n_buckets); \
	extern khint_t kh_put_##name(kh_##name##_t *h, khkey_t key, int *ret); \
	extern void kh_del_##name(kh_##name##_t *h, khint_t x);

#define __KHASH_IMPL(name, SCOPE, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal) \
	SCOPE kh_##name##_t *kh_init_##name(void) {							\
		return (kh_##name##_t*)kcalloc(1, sizeof(kh_##name##_t));		\
	}																	\
	SCOPE void kh_destroy_##name(kh_##name##_t *h)						\
	{																	\
		if (h) {														\
			kfree((void *)h->keys); kfree(h->flags);					\
			kfree((void *)h->vals);										\
			kfree(h);													\
		}																\
	}																	\
	SCOPE void kh_clear_##name(kh_##name##_t *h)						\
	{																	\
		if (h && h->flags) {											\
			memset(h->flags, 0xaa, __ac_fsize(h->n_buckets) * sizeof(khint32_t)); \
			h->size = h->n_occupied = 0;								\
		}																\
	}																	\
	SCOPE khint_t kh_get_##name(const kh_##name##_t *h, khkey_t key) 	\
	{																	\
		if (h->n_buckets) {												\
			khint_t k, i, last, mask, step = 0; \
			mask = h->n_buckets - 1;									\
			k = __hash_func(key); i = k & mask;							\
			last = i; \
			while (!__ac_isempty(h->flags, i) && (__ac_isdel(h->flags, i) || !__hash_equal(h->keys[i], key))) { \
				i = (i + (++step)) & mask; \
				if (i == last) return h->n_buckets;						\
			}															\
			return __ac_iseither(h->flags, i)? h->n_buckets : i;		\
		} else return 0;												\
	}																	\
	SCOPE int kh_resize_##name(kh_##name##_t *h, khint_t new_n_buckets) \
	{ /* This function uses 0.25*n_buckets bytes of working space instead of [sizeof(key_t+val_t)+.25]*n_buckets. */ \
		khint32_t *new_flags = 0;										\
		khint_t j = 1;													\
		{																\
			kroundup32(new_n_buckets); 									\
			if (new_n_buckets < 4) new_n_buckets = 4;					\
			if (h->size >= (khint_t)(new_n_buckets * __ac_HASH_UPPER + 0.5)) j = 0;	/* requested size is too small */ \
			else { /* hash table size to be changed (shrink or expand); rehash */ \
				new_flags = (khint32_t*)kmalloc(__ac_fsize(new_n_buckets) * sizeof(khint32_t));	\
				if (!new_flags) return -1;								\
				memset(new_flags, 0xaa, __ac_fsize(new_n_buckets) * sizeof(khint32_t)); \
				if (h->n_buckets < new_n_buckets) {	/* expand */		\
					khkey_t *new_keys = (khkey_t*)krealloc((void *)h->keys, new_n_buckets * sizeof(khkey_t)); \
					if (!new_keys) { kfree(new_flags); return -1; }		\
					h->keys = new_keys;									\
					if (kh_is_map) {									\
						khval_t *new_vals = (khval_t*)krealloc((void *)h->vals, new_n_buckets * sizeof(khval_t)); \
						if (!new_vals) { kfree(new_flags); return -1; }	\
						h->vals = new_vals;								\
					}													\
				} /* otherwise shrink */								\
			}															\
		}																\
		if (j) { /* rehashing is needed */								\
			for (j = 0; j != h->n_buckets; ++j) {						\
				if (__ac_iseither(h->flags, j) == 0) {					\
					khkey_t key = h->keys[j];							\
					khval_t val;										\
					khint_t new_mask;									\
					new_mask = new_n_buckets - 1; 						\
					if (kh_is_map) val = h->vals[j];					\
					__ac_set_isdel_true(h->flags, j);					\
					while (1) { /* kick-out process; sort of like in Cuckoo hashing */ \
						khint_t k, i, step = 0; \
						k = __hash_func(key);							\
						i = k & new_mask;								\
						while (!__ac_isempty(new_flags, i)) i = (i + (++step)) & new_mask; \
						__ac_set_isempty_false(new_flags, i);			\
						if (i < h->n_buckets && __ac_iseither(h->flags, i) == 0) { /* kick out the existing element */ \
							{ khkey_t tmp = h->keys[i]; h->keys[i] = key; key = tmp; } \
							if (kh_is_map) { khval_t tmp = h->vals[i]; h->vals[i] = val; val = tmp; } \
							__ac_set_isdel_true(h->flags, i); /* mark it as deleted in the old hash table */ \
						} else { /* write the element and jump out of the loop */ \
							h->keys[i] = key;							\
							if (kh_is_map) h->vals[i] = val;			\
							break;										\
						}												\
					}													\
				}														\
			}															\
			if (h->n_buckets > new_n_buckets) { /* shrink the hash table */ \
				h->keys = (khkey_t*)krealloc((void *)h->keys, new_n_buckets * sizeof(khkey_t)); \
				if (kh_is_map) h->vals = (khval_t*)krealloc((void *)h->vals, new_n_buckets * sizeof(khval_t)); \
			}															\
			kfree(h->flags); /* free the working space */				\
			h->flags = new_flags;										\
			h->n_buckets = new_n_buckets;								\
			h->n_occupied = h->size;									\
			h->upper_bound = (khint_t)(h->n_buckets * __ac_HASH_UPPER + 0.5); \
		}																\
		return 0;														\
	}																	\
	SCOPE khint_t kh_put_##name(kh_##name##_t *h, khkey_t key, int *ret) \
	{																	\
		khint_t x;														\
		if (h->n_occupied >= h->upper_bound) { /* update the hash table */ \
			if (h->n_buckets > (h->size<<1)) {							\
				if (kh_resize_##name(h, h->n_buckets - 1) < 0) { /* clear "deleted" elements */ \
					*ret = -1; return h->n_buckets;						\
				}														\
			} else if (kh_resize_##name(h, h->n_buckets + 1) < 0) { /* expand the hash table */ \
				*ret = -1; return h->n_buckets;							\
			}															\
		} /* TODO: to implement automatically shrinking; resize() already support shrinking */ \
		{																\
			khint_t k, i, site, last, mask = h->n_buckets - 1, step = 0; \
			x = site = h->n_buckets; k = __hash_func(key); i = k & mask; \
			if (__ac_isempty(h->flags, i)) x = i; /* for speed up */	\
			else {														\
				last = i; \
				while (!__ac_isempty(h->flags, i) && (__ac_isdel(h->flags, i) || !__hash_equal(h->keys[i], key))) { \
					if (__ac_isdel(h->flags, i)) site = i;				\
					i = (i + (++step)) & mask; \
					if (i == last) { x = site; break; }					\
				}														\
				if (x == h->n_buckets) {								\
					if (__ac_isempty(h->flags, i) && site != h->n_buckets) x = site; \
					else x = i;											\
				}														\
			}															\
		}																\
		if (__ac_isempty(h->flags, x)) { /* not present at all */		\
			h->keys[x] = key;											\
			__ac_set_isboth_false(h->flags, x);							\
			++h->size; ++h->n_occupied;									\
			*ret = 1;													\
		} else if (__ac_isdel(h->flags, x)) { /* deleted */				\
			h->keys[x] = key;											\
			__ac_set_isboth_false(h->flags, x);							\
			++h->size;													\
			*ret = 2;													\
		} else *ret = 0; /* Don't touch h->keys[x] if present and not deleted */ \
		return x;														\
	}																	\
	SCOPE void kh_del_##name(kh_##name##_t *h, khint_t x)				\
	{																	\
		if (x != h->n_buckets && !__ac_iseither(h->flags, x)) {			\
			__ac_set_isdel_true(h->flags, x);							\
			--h->size;													\
		}																\
	}

#define KHASH_DECLARE(name, khkey_t, khval_t)		 					\
	__KHASH_TYPE(name, khkey_t, khval_t) 								\
	__KHASH_PROTOTYPES(name, khkey_t, khval_t)

#define KHASH_INIT2(name, SCOPE, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal) \
	__KHASH_TYPE(name, khkey_t, khval_t) 								\
	__KHASH_IMPL(name, SCOPE, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal)

#define KHASH_INIT(name, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal) \
	KHASH_INIT2(name, static kh_inline klib_unused, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal)

/* --- BEGIN OF HASH FUNCTIONS --- */

/*! @function
  @abstract     Integer hash function
  @param  key   The integer [khint32_t]
  @return       The hash value [khint_t]
 */
#define kh_int_hash_func(key) (khint32_t)(key)
 /*! @function
   @abstract     Integer comparison function
  */
#define kh_int_hash_equal(a, b) ((a) == (b))
  /*! @function
    @abstract     64-bit integer hash function
    @param  key   The integer [khint64_t]
    @return       The hash value [khint_t]
   */
#define kh_int64_hash_func(key) (khint32_t)((key)>>33^(key)^(key)<<11)
   /*! @function
     @abstract     64-bit integer comparison function
    */
#define kh_int64_hash_equal(a, b) ((a) == (b))
    /*! @function
      @abstract     const char* hash function
      @param  s     Pointer to a null terminated string
      @return       The hash value
     */
static kh_inline khint_t __ac_X31_hash_string(const char* s)
{
  khint_t h = (khint_t)*s;
  if (h) for (++s; *s; ++s) h = (h << 5) - h + (khint_t)*s;
  return h;
}
/*! @function
  @abstract     Another interface to const char* hash function
  @param  key   Pointer to a null terminated string [const char*]
  @return       The hash value [khint_t]
 */
#define kh_str_hash_func(key) __ac_X31_hash_string(key)
 /*! @function
   @abstract     Const char* comparison function
  */
#define kh_str_hash_equal(a, b) (strcmp(a, b) == 0)

static kh_inline khint_t __ac_Wang_hash(khint_t key)
{
  key += ~(key << 15);
  key ^= (key >> 10);
  key += (key << 3);
  key ^= (key >> 6);
  key += ~(key << 11);
  key ^= (key >> 16);
  return key;
}
#define kh_int_hash_func2(key) __ac_Wang_hash((khint_t)key)

/* --- END OF HASH FUNCTIONS --- */

/* Other convenient macros... */

/*!
  @abstract Type of the hash table.
  @param  name  Name of the hash table [symbol]
 */
#define khash_t(name) kh_##name##_t

 /*! @function
   @abstract     Initiate a hash table.
   @param  name  Name of the hash table [symbol]
   @return       Pointer to the hash table [khash_t(name)*]
  */
#define kh_init(name) kh_init_##name()

  /*! @function
    @abstract     Destroy a hash table.
    @param  name  Name of the hash table [symbol]
    @param  h     Pointer to the hash table [khash_t(name)*]
   */
#define kh_destroy(name, h) kh_destroy_##name(h)

   /*! @function
     @abstract     Reset a hash table without deallocating memory.
     @param  name  Name of the hash table [symbol]
     @param  h     Pointer to the hash table [khash_t(name)*]
    */
#define kh_clear(name, h) kh_clear_##name(h)

    /*! @function
      @abstract     Resize a hash table.
      @param  name  Name of the hash table [symbol]
      @param  h     Pointer to the hash table [khash_t(name)*]
      @param  s     New size [khint_t]
     */
#define kh_resize(name, h, s) kh_resize_##name(h, s)

     /*! @function
       @abstract     Insert a key to the hash table.
       @param  name  Name of the hash table [symbol]
       @param  h     Pointer to the hash table [khash_t(name)*]
       @param  k     Key [type of keys]
       @param  r     Extra return code: -1 if the operation failed;
                     0 if the key is present in the hash table;
                     1 if the bucket is empty (never used); 2 if the element in
             the bucket has been deleted [int*]
       @return       Iterator to the inserted element [khint_t]
      */
#define kh_put(name, h, k, r) kh_put_##name(h, k, r)

      /*! @function
        @abstract     Retrieve a key from the hash table.
        @param  name  Name of the hash table [symbol]
        @param  h     Pointer to the hash table [khash_t(name)*]
        @param  k     Key [type of keys]
        @return       Iterator to the found element, or kh_end(h) if the element is absent [khint_t]
       */
#define kh_get(name, h, k) kh_get_##name(h, k)

       /*! @function
         @abstract     Remove a key from the hash table.
         @param  name  Name of the hash table [symbol]
         @param  h     Pointer to the hash table [khash_t(name)*]
         @param  k     Iterator to the element to be deleted [khint_t]
        */
#define kh_del(name, h, k) kh_del_##name(h, k)

        /*! @function
          @abstract     Test whether a bucket contains data.
          @param  h     Pointer to the hash table [khash_t(name)*]
          @param  x     Iterator to the bucket [khint_t]
          @return       1 if containing data; 0 otherwise [int]
         */
#define kh_exist(h, x) (!__ac_iseither((h)->flags, (x)))

         /*! @function
           @abstract     Get key given an iterator
           @param  h     Pointer to the hash table [khash_t(name)*]
           @param  x     Iterator to the bucket [khint_t]
           @return       Key [type of keys]
          */
#define kh_key(h, x) ((h)->keys[x])

          /*! @function
            @abstract     Get value given an iterator
            @param  h     Pointer to the hash table [khash_t(name)*]
            @param  x     Iterator to the bucket [khint_t]
            @return       Value [type of values]
            @discussion   For hash sets, calling this results in segfault.
           */
#define kh_val(h, x) ((h)->vals[x])

           /*! @function
             @abstract     Alias of kh_val()
            */
#define kh_value(h, x) ((h)->vals[x])

            /*! @function
              @abstract     Get the start iterator
              @param  h     Pointer to the hash table [khash_t(name)*]
              @return       The start iterator [khint_t]
             */
#define kh_begin(h) (khint_t)(0)

             /*! @function
               @abstract     Get the end iterator
               @param  h     Pointer to the hash table [khash_t(name)*]
               @return       The end iterator [khint_t]
              */
#define kh_end(h) ((h)->n_buckets)

              /*! @function
                @abstract     Get the number of elements in the hash table
                @param  h     Pointer to the hash table [khash_t(name)*]
                @return       Number of elements in the hash table [khint_t]
               */
#define kh_size(h) ((h)->size)

               /*! @function
                 @abstract     Get the number of buckets in the hash table
                 @param  h     Pointer to the hash table [khash_t(name)*]
                 @return       Number of buckets in the hash table [khint_t]
                */
#define kh_n_buckets(h) ((h)->n_buckets)

                /*! @function
                  @abstract     Iterate over the entries in the hash table
                  @param  h     Pointer to the hash table [khash_t(name)*]
                  @param  kvar  Variable to which key will be assigned
                  @param  vvar  Variable to which value will be assigned
                  @param  code  Block of code to execute
                 */
#define kh_foreach(h, kvar, vvar, code) { khint_t __i;		\
	for (__i = kh_begin(h); __i != kh_end(h); ++__i) {		\
		if (!kh_exist(h,__i)) continue;						\
		(kvar) = kh_key(h,__i);								\
		(vvar) = kh_val(h,__i);								\
		code;												\
	} }

                 /*! @function
                   @abstract     Iterate over the values in the hash table
                   @param  h     Pointer to the hash table [khash_t(name)*]
                   @param  vvar  Variable to which value will be assigned
                   @param  code  Block of code to execute
                  */
#define kh_foreach_value(h, vvar, code) { khint_t __i;		\
	for (__i = kh_begin(h); __i != kh_end(h); ++__i) {		\
		if (!kh_exist(h,__i)) continue;						\
		(vvar) = kh_val(h,__i);								\
		code;												\
	} }

                  /* More convenient interfaces */

                  /*! @function
                    @abstract     Instantiate a hash set containing integer keys
                    @param  name  Name of the hash table [symbol]
                   */
#define KHASH_SET_INIT_INT(name)										\
	KHASH_INIT(name, khint32_t, char, 0, kh_int_hash_func, kh_int_hash_equal)

                   /*! @function
                     @abstract     Instantiate a hash map containing integer keys
                     @param  name  Name of the hash table [symbol]
                     @param  khval_t  Type of values [type]
                    */
#define KHASH_MAP_INIT_INT(name, khval_t)								\
	KHASH_INIT(name, khint32_t, khval_t, 1, kh_int_hash_func, kh_int_hash_equal)

                    /*! @function
                      @abstract     Instantiate a hash set containing 64-bit integer keys
                      @param  name  Name of the hash table [symbol]
                     */
#define KHASH_SET_INIT_INT64(name)										\
	KHASH_INIT(name, khint64_t, char, 0, kh_int64_hash_func, kh_int64_hash_equal)

                     /*! @function
                       @abstract     Instantiate a hash map containing 64-bit integer keys
                       @param  name  Name of the hash table [symbol]
                       @param  khval_t  Type of values [type]
                      */
#define KHASH_MAP_INIT_INT64(name, khval_t)								\
	KHASH_INIT(name, khint64_t, khval_t, 1, kh_int64_hash_func, kh_int64_hash_equal)

typedef const char* kh_cstr_t;
/*! @function
  @abstract     Instantiate a hash map containing const char* keys
  @param  name  Name of the hash table [symbol]
 */
#define KHASH_SET_INIT_STR(name)										\
	KHASH_INIT(name, kh_cstr_t, char, 0, kh_str_hash_func, kh_str_hash_equal)

 /*! @function
   @abstract     Instantiate a hash map containing const char* keys
   @param  name  Name of the hash table [symbol]
   @param  khval_t  Type of values [type]
  */
#define KHASH_MAP_INIT_STR(name, khval_t)								\
	KHASH_INIT(name, kh_cstr_t, khval_t, 1, kh_str_hash_func, kh_str_hash_equal)

#endif /* __AC_KHASH_H */

#ifdef __AC_KHASH_H
#define string_hash(key) kh_str_hash_func(key)
typedef u64 hash_id;
typedef khint_t hashed_string;

KHASH_MAP_INIT_INT(hash_id, hash_id);
#define MAKE_HASH_TABLE(name) static khash_t(hash_id)* name;
#define HASH_TABLE_INIT(name)name = kh_init(hash_id);
MAKE_HASH_TABLE(htable);

#define FAILED_HASH_ID 0

static inline hash_id Table_GetHashID(kh_hash_id_t* hash_table, const char* name) {
  hash_id ret = FAILED_HASH_ID;
  khint_t key = kh_str_hash_func(name);
  int er = 0;
  khint_t  k = kh_put_hash_id(hash_table, key, &er);
  if ((k != kh_end(hash_table))) ret = (hash_id)kh_value(hash_table, k);
  return ret;
}
static inline hash_id Table_GetHashViaKey(kh_hash_id_t *hash_table,khint_t key) {
  hash_id ret = FAILED_HASH_ID;
  int er = 0;
  khint_t  k = kh_put_hash_id(hash_table, key, &er);
  if ((k != kh_end(hash_table))) ret = (hash_id)kh_value(hash_table, k);
  return ret;
}

static inline void Table_SetHashID(kh_hash_id_t* hash_table, const char* name, hash_id value) {
  khint_t key = kh_str_hash_func(name);
  int er = 0;
  khint_t  k = kh_put_hash_id(htable, key, &er);
  kh_value(hash_table, k) = value;
}
static inline void Table_SetHashViaKey(kh_hash_id_t *hash_table,khint_t key, hash_id value) {
  int er = 0;
  khint_t  k = kh_put_hash_id(hash_table, key, &er);
  kh_value(hash_table, k) = value;
}
#define GetHashID(key_string) Table_GetHashID(htable,key_string)
#define SetHashID(key_string, value) Table_SetHashID(htable,key_string, value)
#define SetHashViaKey(key_string, value) Table_SetHashViaKey(htable,key_string, value)
#define GetHashViaKey(key_string) Table_GetHashViaKey(htable,key_string)

#endif
#endif