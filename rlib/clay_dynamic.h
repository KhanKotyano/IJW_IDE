#include "clay.h"
#include "uni_util.h"

void Clay__ConfigureOpenElementDynamic(Clay_ElementDeclaration* declaration) {
  Clay_Context* context = Clay_GetCurrentContext();
  Clay_LayoutElement* openLayoutElement = Clay__GetOpenLayoutElement();
  openLayoutElement->layoutConfig = Clay__StoreLayoutConfig(declaration->layout);
  if ((declaration->layout.sizing.width.type == CLAY__SIZING_TYPE_PERCENT && declaration->layout.sizing.width.size.percent > 1) || (declaration->layout.sizing.height.type == CLAY__SIZING_TYPE_PERCENT && declaration->layout.sizing.height.size.percent > 1)) {
    context->errorHandler.errorHandlerFunction(CLAY__INIT(Clay_ErrorData) {
      .errorType = CLAY_ERROR_TYPE_PERCENTAGE_OVER_1,
        .errorText = CLAY_STRING("An element was configured with CLAY_SIZING_PERCENT, but the provided percentage value was over 1.0. Clay expects a value between 0 and 1, i.e. 20% is 0.2."),
        .userData = context->errorHandler.userData
    });
  }

  Clay_ElementId openLayoutElementId = declaration->id;

  openLayoutElement->elementConfigs.internalArray = &context->elementConfigs.internalArray[context->elementConfigs.length];
  Clay_SharedElementConfig* sharedConfig = NULL;
  if (declaration->backgroundColor.a > 0) {
    sharedConfig = Clay__StoreSharedElementConfig(CLAY__INIT(Clay_SharedElementConfig) { .backgroundColor = declaration->backgroundColor });
    Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .sharedElementConfig = sharedConfig }, CLAY__ELEMENT_CONFIG_TYPE_SHARED);
  }
  if (!Clay__MemCmp((char*)(&declaration->cornerRadius), (char*)(&Clay__CornerRadius_DEFAULT), sizeof(Clay_CornerRadius))) {
    if (sharedConfig) {
      sharedConfig->cornerRadius = declaration->cornerRadius;
    }
    else {
      sharedConfig = Clay__StoreSharedElementConfig(CLAY__INIT(Clay_SharedElementConfig) { .cornerRadius = declaration->cornerRadius });
      Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .sharedElementConfig = sharedConfig }, CLAY__ELEMENT_CONFIG_TYPE_SHARED);
    }
  }
  if (declaration->userData != 0) {
    if (sharedConfig) {
      sharedConfig->userData = declaration->userData;
    }
    else {
      sharedConfig = Clay__StoreSharedElementConfig(CLAY__INIT(Clay_SharedElementConfig) { .userData = declaration->userData });
      Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .sharedElementConfig = sharedConfig }, CLAY__ELEMENT_CONFIG_TYPE_SHARED);
    }
  }
  if (declaration->image.imageData) {
    Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .imageElementConfig = Clay__StoreImageElementConfig(declaration->image) }, CLAY__ELEMENT_CONFIG_TYPE_IMAGE);
  }
  if (declaration->aspectRatio.aspectRatio > 0) {
    Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .aspectRatioElementConfig = Clay__StoreAspectRatioElementConfig(declaration->aspectRatio) }, CLAY__ELEMENT_CONFIG_TYPE_ASPECT);
    Clay__int32_tArray_Add(&context->aspectRatioElementIndexes, context->layoutElements.length - 1);
  }
  if (declaration->floating.attachTo != CLAY_ATTACH_TO_NONE) {
    Clay_FloatingElementConfig floatingConfig = declaration->floating;
    // This looks dodgy but because of the auto generated root element the depth of the tree will always be at least 2 here
    Clay_LayoutElement* hierarchicalParent = Clay_LayoutElementArray_Get(&context->layoutElements, Clay__int32_tArray_GetValue(&context->openLayoutElementStack, context->openLayoutElementStack.length - 2));
    if (hierarchicalParent) {
      uint32_t clipElementId = 0;
      if (declaration->floating.attachTo == CLAY_ATTACH_TO_PARENT) {
        // Attach to the element's direct hierarchical parent
        floatingConfig.parentId = hierarchicalParent->id;
        if (context->openClipElementStack.length > 0) {
          clipElementId = Clay__int32_tArray_GetValue(&context->openClipElementStack, (int)context->openClipElementStack.length - 1);
        }
      }
      else if (declaration->floating.attachTo == CLAY_ATTACH_TO_ELEMENT_WITH_ID) {
        Clay_LayoutElementHashMapItem* parentItem = Clay__GetHashMapItem(floatingConfig.parentId);
        if (parentItem == &Clay_LayoutElementHashMapItem_DEFAULT) {
          context->errorHandler.errorHandlerFunction(CLAY__INIT(Clay_ErrorData) {
            .errorType = CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND,
              .errorText = CLAY_STRING("A floating element was declared with a parentId, but no element with that ID was found."),
              .userData = context->errorHandler.userData
          });
        }
        else {
          clipElementId = Clay__int32_tArray_GetValue(&context->layoutElementClipElementIds, (int32_t)(parentItem->layoutElement - context->layoutElements.internalArray));
        }
      }
      else if (declaration->floating.attachTo == CLAY_ATTACH_TO_ROOT) {
        floatingConfig.parentId = Clay__HashString(CLAY_STRING("Clay__RootContainer"), 0, 0).id;
      }
      if (!openLayoutElementId.id) {
        openLayoutElementId = Clay__HashString(CLAY_STRING("Clay__FloatingContainer"), context->layoutElementTreeRoots.length, 0);
      }
      if (declaration->floating.clipTo == CLAY_CLIP_TO_NONE) {
        clipElementId = 0;
      }
      int32_t currentElementIndex = Clay__int32_tArray_GetValue(&context->openLayoutElementStack, context->openLayoutElementStack.length - 1);
      Clay__int32_tArray_Set(&context->layoutElementClipElementIds, currentElementIndex, clipElementId);
      Clay__int32_tArray_Add(&context->openClipElementStack, clipElementId);
      Clay__LayoutElementTreeRootArray_Add(&context->layoutElementTreeRoots, CLAY__INIT(Clay__LayoutElementTreeRoot) {
        .layoutElementIndex = Clay__int32_tArray_GetValue(&context->openLayoutElementStack, context->openLayoutElementStack.length - 1),
          .parentId = floatingConfig.parentId,
          .clipElementId = clipElementId,
          .zIndex = floatingConfig.zIndex,
      });
      Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .floatingElementConfig = Clay__StoreFloatingElementConfig(floatingConfig) }, CLAY__ELEMENT_CONFIG_TYPE_FLOATING);
    }
  }
  if (declaration->custom.customData) {
    Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .customElementConfig = Clay__StoreCustomElementConfig(declaration->custom) }, CLAY__ELEMENT_CONFIG_TYPE_CUSTOM);
  }

  if (openLayoutElementId.id != 0) {
    Clay__AttachId(openLayoutElementId);
  }
  else if (openLayoutElement->id == 0) {
    openLayoutElementId = Clay__GenerateIdForAnonymousElement(openLayoutElement);
  }

  if (declaration->clip.horizontal | declaration->clip.vertical) {
    Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .clipElementConfig = Clay__StoreClipElementConfig(declaration->clip) }, CLAY__ELEMENT_CONFIG_TYPE_CLIP);
    Clay__int32_tArray_Add(&context->openClipElementStack, (int)openLayoutElement->id);
    // Retrieve or create cached data to track scroll position across frames
    Clay__ScrollContainerDataInternal* scrollOffset = CLAY__NULL;
    for (int32_t i = 0; i < context->scrollContainerDatas.length; i++) {
      Clay__ScrollContainerDataInternal* mapping = Clay__ScrollContainerDataInternalArray_Get(&context->scrollContainerDatas, i);
      if (openLayoutElement->id == mapping->elementId) {
        scrollOffset = mapping;
        scrollOffset->layoutElement = openLayoutElement;
        scrollOffset->openThisFrame = true;
      }
    }
    if (!scrollOffset) {
      scrollOffset = Clay__ScrollContainerDataInternalArray_Add(&context->scrollContainerDatas, CLAY__INIT(Clay__ScrollContainerDataInternal){.layoutElement = openLayoutElement, .scrollOrigin = { -1,-1 }, .elementId = openLayoutElement->id, .openThisFrame = true});
    }
    if (context->externalScrollHandlingEnabled) {
      scrollOffset->scrollPosition = Clay__QueryScrollOffset(scrollOffset->elementId, context->queryScrollOffsetUserData);
    }
  }
  if (!Clay__MemCmp((char*)(&declaration->border.width), (char*)(&Clay__BorderWidth_DEFAULT), sizeof(Clay_BorderWidth))) {
    Clay__AttachElementConfig(CLAY__INIT(Clay_ElementConfigUnion) { .borderElementConfig = Clay__StoreBorderElementConfig(declaration->border) }, CLAY__ELEMENT_CONFIG_TYPE_BORDER);
  }
}

//COMMON
static Clay_ElementDeclaration current_elemet = { 0 };
static u32 __internal_open_count = 0;
static u32 __internal_close_count = 0;

static Clay_ElementId ui_el_begin_config(const char* name) {
  ZeroMemory_Util(&current_elemet, sizeof(Clay_ElementDeclaration));
  Clay_String _string = { 0 };
  if (name != NULL) {
    _string.length = strlen(name);
    _string.chars = name;
    current_elemet.id = Clay__HashString(_string, 0, Clay__GetParentElementId());
  }
  Clay__OpenElement();
  __internal_open_count++;
  return current_elemet.id;
}
static void ui_el_end_config() {
  Clay__ConfigureOpenElementDynamic(&current_elemet);
}
static void ui_el_close() {
  Clay__CloseElement();
  __internal_close_count++;
}

#define  UI_SIZING_GROW CLAY__SIZING_TYPE_GROW
#define  UI_SIZING_FIT CLAY__SIZING_TYPE_FIT,
#define  UI_SIZING_PERCENT CLAY__SIZING_TYPE_PERCENT
#define  UI_SIZING_FIXED CLAY__SIZING_TYPE_FIXED
#define UI_CALIGN_TOP CLAY_ALIGN_Y_TOP
#define UI_CALIGN_BOTTOM CLAY_ALIGN_Y_BOTTOM
#define UI_CALIGN_CENTER CLAY_ALIGN_X_CENTER
#define UI_CALIGN_RIGHT CLAY_ALIGN_X_RIGHT
#define UI_CALIGN_LEFT CLAY_ALIGN_X_LEFT

#define UI_DIR_BOTTOM CLAY_TOP_TO_BOTTOM
#define UI_DIR_RIGHT CLAY_LEFT_TO_RIGHT

#define UI_SIZING(_TYPE,_min, _max) (TYPEDEC(Clay_SizingAxis) { .size = { .minMax = { .min = (float)_min, .max = (float)_max } }, .type = (Clay__SizingType)_TYPE })
#define UI_SIZING_S(_sizing) UI_SIZING(_sizing.type, _sizing.size.minMax.min, _sizing.size.minMax.max)
#define UI_CHILD_ALLIGN(_x,_y)TYPEDEC(Clay_ChildAlignment){.x = _x, .y = _y}
#define UI_CHILD_ALLIGN_S(_callign)UI_CHILD_ALLIGN(_callign.x, _callign.y)
typedef struct {
  Clay_LayoutAlignmentX x;
  Clay_LayoutAlignmentY y;
  u16 child_gap;
  Clay_LayoutDirection ldir;
  Clay_Padding padding;
}UICaling;
#define UI_CALIGN(_x, _y, _child_gap,_ldir)TYPEDEC(UICaling){.x = (Clay_LayoutAlignmentX)_x, .y = (Clay_LayoutAlignmentY)_y, .child_gap = (u16)_child_gap, .ldir = (Clay_LayoutDirection)_ldir}
#define UI_CALIGN_0 UI_CALIGN(0,0,0,0)


static void ui_el_layout_config(Clay_SizingAxis w_sizing, Clay_SizingAxis h_sizing , UICaling callign) {
  current_elemet.layout.sizing.height = UI_SIZING_S(h_sizing);
  current_elemet.layout.sizing.width = UI_SIZING_S(w_sizing);
  current_elemet.layout.childAlignment = UI_CHILD_ALLIGN(callign.x, callign.y);
  current_elemet.layout.childGap = callign.child_gap;
  current_elemet.layout.layoutDirection = callign.ldir;
  current_elemet.layout.padding.left = callign.padding.left;
  current_elemet.layout.padding.right = callign.padding.right;
  current_elemet.layout.padding.top = callign.padding.top;
  current_elemet.layout.padding.bottom = callign.padding.bottom;
}


static void ui_el_set_color(u32 color) {
  UniColor _col = { .uni = (u32)color };
  current_elemet.backgroundColor = UNI_COLOR_TO_CLAY_COLOR(_col);
}
static void ui_el_set_ratio(float ratio) {
  current_elemet.aspectRatio.aspectRatio = ratio;
}

typedef struct UIFloatingBase {
  Clay_FloatingAttachToElement attach_to;
  Clay_FloatingAttachPointType attach_x;
  Clay_FloatingAttachPointType attach_y;
  Clay_Vector2 offset;
}UIFloatingBase;
typedef struct UIFloatingInteraction {
  Clay_PointerCaptureMode capture_mode;
  Clay_FloatingClipToElement clipping_mode;
  int16_t zIndex;
}UIFloatingInteraction;
#define UI_FLOATING_BASE(_attach_mode,_attach,_offset_v2)TYPEDEC(UIFloatingBase){.attach_to = (Clay_FloatingAttachToElement)_attach_mode, .attach_x = (Clay_FloatingAttachPointType)_attach, .attach_y = (Clay_FloatingAttachPointType)_attach, .offset = { _offset_v2.x, _offset_v2.y}}
#define UI_FLOATING_BASE_DIF(_attach_mode,_attach_x, _attach_y,_offset_v2)TYPEDEC(UIFloatingBase){.attach_to = (Clay_FloatingAttachToElement)_attach_mode, .attach_x = (Clay_FloatingAttachPointType)_attach_x, .attach_y = (Clay_FloatingAttachPointType)_attach_y, .offset = { _offset_v2.x, _offset_v2.y}}
#define UI_FLOATING_INTERACTION(_capture_mode, _clipping_mode, _z)TYPEDEC(UIFloatingInteraction){.capture_mode = (Clay_PointerCaptureMode)_capture_mode, .clipping_mode = (Clay_FloatingClipToElement)_clipping_mode,.zIndex = _z }
#define UI_FLOATING_BASE_0 UI_FLOATING_BASE(0,0,0)
#define UI_FLOATING_INTERACTION_0 UI_FLOATING_INTERACTION(0,0,0)
typedef struct UI_ClipConfig {
  bool_s8 horizontal; 
  bool_s8 vertical; 
  v2 child_offset;
}UI_ClipConfig;
static void ui_el_clip_config(UI_ClipConfig config) {
  current_elemet.clip.horizontal =(bool)config.horizontal;
  current_elemet.clip.vertical = (bool)config.vertical;
  current_elemet.clip.childOffset.x = config.child_offset.x;
  current_elemet.clip.childOffset.y = config.child_offset.y;
}
static void ui_el_floating_config(UIFloatingBase fbase, UIFloatingInteraction finteract) {
  current_elemet.floating.attachTo = (Clay_FloatingAttachToElement)fbase.attach_to;
  current_elemet.floating.attachPoints = TYPEDEC(Clay_FloatingAttachPoints){ fbase.attach_x, fbase.attach_y };
  current_elemet.floating.offset = TYPEDEC(Clay_Vector2){ fbase.offset.x, fbase.offset.y} ;
  current_elemet.floating.zIndex = finteract.zIndex;
  current_elemet.floating.clipTo = finteract.clipping_mode;
  current_elemet.floating.pointerCaptureMode = finteract.capture_mode;
}
static inline void ui_el_attach_to(Clay_FloatingAttachToElement attach_to, Clay_FloatingAttachPointType attach_point){
  current_elemet.floating.attachTo = attach_to;
  current_elemet.floating.attachPoints = TYPEDEC(Clay_FloatingAttachPoints){attach_point,attach_point };
}
//BORDER
//#define UI_BORDER(enum_type, _min, _max)TYPEDEC(Clay_BorderWidth){.type = enum_type, .smin = _min, .smax = _max}
typedef struct UI_BorderConfig{
  Clay_BorderWidth border_w;
  float corner_radius;
  u32 color;
}UI_BorderConfig;
static void ui_el_border_config(UI_BorderConfig config) {
  current_elemet.cornerRadius = CLAY_CORNER_RADIUS(config.corner_radius);
  UniColor _col = { .uni = (u32)config.color };
  current_elemet.border.width.bottom = config.border_w.bottom;
  current_elemet.border.width.right = config.border_w.right;
  current_elemet.border.width.left = config.border_w.left;
  current_elemet.border.width.top = config.border_w.top;
  current_elemet.border.color = UNI_COLOR_TO_CLAY_COLOR(_col);
};
static inline void ui_el_border_config_flat(u16 size, u32 color, float radius) {
  UI_BorderConfig config = {.border_w = {.bottom = size, .left = size, .right = size, .top = size }, .color = color, .corner_radius = radius};
  ui_el_border_config(config);
};
//TEXT
static Clay_TextElementConfig current_text_config = { 0 };

static void ui_el_set_text_advanced(u16 f_space, u16 f_line_h, Clay_TextElementConfigWrapMode f_wrapmode) {
  current_text_config.letterSpacing = (u16)f_space;
  current_text_config.lineHeight = (u16)f_line_h;
  current_text_config.wrapMode = (Clay_TextElementConfigWrapMode)f_wrapmode;
}
static void ui_el_set_text_basics(u16 f_id, u16 f_size, u32 color) {
  current_text_config.fontId = (u16)f_id;
  current_text_config.fontSize = (u16)f_size;
  UniColor _col = { .uni = (u32)color };
  current_text_config.textColor = UNI_COLOR_TO_CLAY_COLOR(_col);
}
static void ui_el_set_text(const char* string , bool_s8 is_static) {
  s32 length = strlen(string);
  Clay_String clay_str = { .isStaticallyAllocated = (bool)is_static, .length = (s32)length, .chars = string };
  CLAY_TEXT(clay_str, Clay__StoreTextElementConfig(current_text_config));
  //ZeroMemory_Util(&current_text_config, sizeof(current_text_config));
}
typedef struct UITextConfig{
  const char* text;
  bool_s8 is_static;
  u16 f_id; u16 f_size; u32 color;
  u16 f_space; u16 f_line_h; Clay_TextElementConfigWrapMode f_wrapmode;
}UITextConfig;
typedef struct UIImageConfig {
  u32 image_id;
}UIImageConfig;
static void ui_el_image_config(UIImageConfig image_id, float aspect_ratio) {
  current_elemet.image.imageData = image_id.image_id;
  current_elemet.aspectRatio.aspectRatio = aspect_ratio;
}

typedef struct UI_UniConfig{
  const char *name;
  u32 color[2];
  UICaling child_aling;
  Clay_SizingAxis sizing_w;
  Clay_SizingAxis sizing_h;
  UIFloatingBase floating_base;
  UIFloatingInteraction floating_interaction;
  UI_BorderConfig border_config;
  UI_ClipConfig clip_config;
  UITextConfig text_config;
  UIImageConfig image_config;
  float aspect_ratio;
}UI_UniConfig;
#define UI_CONFIG_UNI(_name, _text, _child_aling, _sizing_w, _sizing_h, _floating_base, _floating_interaction)TYPEDEC(UI_UniConfig){.name = _name, .text = _text, .child_aling = _child_aling, .sizing_w = _sizing_w. .sizing_h = _sizing_h, .floating_base = _floating_base, .floating_interaction = _floating_interaction}

//meh
void ui_begin_layout() {
  __internal_open_count = 0;
  __internal_close_count = 0;
  Clay_BeginLayout();
}
Clay_RenderCommandArray ui_end_layout() {
  Clay_RenderCommandArray l = {0};
  if (__internal_open_count == __internal_close_count) 
    l = Clay_EndLayout();
  else ShowMessageModule("CLAY_D", ML_ERROR, "UI Element tree is unbalanced (Element : ElementEnd)");
  return l;
}
