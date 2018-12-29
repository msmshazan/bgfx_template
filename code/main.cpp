
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>
#include <bgfx/defines.h>
#include <bgfx/c99/platform.h>
#include <bgfx/c99/bgfx.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <intrin.h>
#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ARRAYCOUNT(arr) sizeof(arr)/sizeof(arr[0])

typedef struct
{
    void *Data;
    int Width;
    int Height;
    int Pitch;
    int Size;
}BitmapBuffer;

typedef struct
{
    void *Data;
    size_t Size;
}FileHandle;

typedef struct VertexPostionTexture
{
    float X;
    float Y;
    float Z;
    float U;
    float V; 
}VertexPostionTexture;


typedef struct VertexPostionColor
{
    float X;
    float Y;
    float Z;
    uint32_t color;
}VertexPostionColor;

typedef struct VertexPostionColorTexture
{
    float X;
    float Y;
    float Z;
    float U;
    float V;
    uint32_t color;
}VertexPostionColorTexture;

typedef enum RenderCommandType
{
    RenderCommand_DrawRect,
    RenderCommand_DrawBitmap
}RenderCommandType;

typedef struct
{
    uint32_t Width;
    uint32_t Height;
}RenderSettings;

typedef struct
{
    float x;
    float y;
    bool Right;
    bool Left;
    bool Middle;
}MouseState;

typedef struct
{
    RenderCommandType Type;
    hmm_vec2 Vertices[4];
    hmm_vec4 Color[4];        
    float Depth;
    union{
        BitmapBuffer* Texture;
        void* __unused;
    };
}RenderCommand;

typedef struct
{
    bool Initialized;
    RenderSettings Settings;
    size_t MaxCount;
    RenderCommand *Commands;
    size_t Count;
    BitmapBuffer Pixel;
}RendererContext;

bgfx_vertex_decl_t
VertexPostionTextureDeclaration()
{
    bgfx_vertex_decl_t Declaration = {};
    bgfx_vertex_decl_begin(&Declaration,BGFX_RENDERER_TYPE_NOOP);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_POSITION, 3, BGFX_ATTRIB_TYPE_FLOAT,false,false);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_TEXCOORD0, 2, BGFX_ATTRIB_TYPE_FLOAT,false, false);
    bgfx_vertex_decl_end(&Declaration);
    return Declaration;
};


bgfx_vertex_decl_t
VertexPostionColorDeclaration()
{
    bgfx_vertex_decl_t Declaration = {};
    bgfx_vertex_decl_begin(&Declaration,BGFX_RENDERER_TYPE_NOOP);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_POSITION, 3, BGFX_ATTRIB_TYPE_FLOAT,false,false);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_COLOR0, 4, BGFX_ATTRIB_TYPE_UINT8, true,true);
    bgfx_vertex_decl_end(&Declaration);
    return Declaration;
};

bgfx_vertex_decl_t
VertexPostionColorTextureDeclaration()
{
    bgfx_vertex_decl_t Declaration = {};
    bgfx_vertex_decl_begin(&Declaration,BGFX_RENDERER_TYPE_NOOP);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_POSITION, 3, BGFX_ATTRIB_TYPE_FLOAT,false,false);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_TEXCOORD0, 2, BGFX_ATTRIB_TYPE_FLOAT,false,false);
    bgfx_vertex_decl_add(&Declaration,BGFX_ATTRIB_COLOR0,  4, BGFX_ATTRIB_TYPE_UINT8, true,true);
    bgfx_vertex_decl_end(&Declaration);
    return Declaration;
};


void
UpdateTexture(BitmapBuffer *Bitmap , bgfx_texture_handle_t *Handle)
{
    bgfx_update_texture_2d(*Handle,0,0,0,0,Bitmap->Width,Bitmap->Height,bgfx_make_ref(Bitmap->Data,Bitmap->Size),Bitmap->Pitch);
}

BitmapBuffer
CreateBitmapBuffer(int w, int h)
{
    BitmapBuffer Result = {};
    Result.Width = w;
    Result.Height = h;
    Result.Pitch = w * sizeof(uint32_t);
    Result.Size = w*h * sizeof(uint32_t);
    Result.Data = calloc(h*w*sizeof(uint32_t),1);
    return Result;
} 


BitmapBuffer
CreateWhitePixel()
{

    BitmapBuffer Result = CreateBitmapBuffer(1,1);
    *((uint32_t *)Result.Data) =0xffffffff;
    return Result;
}



MouseState
GetMouseState()
{
    MouseState Result = {};
    int x = 0;
    int y = 0;
    uint32_t State = SDL_GetMouseState(&x,&y);
    Result.x = (float) x;
    Result.y = (float) y;
    if(State & SDL_BUTTON_LMASK) Result.Left = true;
    if(State & SDL_BUTTON_MMASK) Result.Middle = true;
    if(State & SDL_BUTTON_RMASK) Result.Right = true;
    
    return Result;
}

void
RendererInit(uint32_t Width,uint32_t Height,RendererContext *Renderer)
{
    *Renderer = {};
    RenderSettings Settings = {Width,Height};
    Renderer->Settings = Settings;
    Renderer->MaxCount = 4096;
    Renderer->Initialized = true;
    Renderer->Commands = (RenderCommand *) calloc(sizeof(RenderCommand)*Renderer->MaxCount,1);
    Renderer->Count = 0;
    Renderer->Pixel = CreateWhitePixel();
}

void
RendererFlushBuffer(RendererContext *Renderer,bgfx_program_handle_t *Programs,bgfx_uniform_handle_t *Uniforms,bgfx_texture_handle_t *Textures)
{
    hmm_vec2 screenbounds = HMM_Vec2(Renderer->Settings.Width,Renderer->Settings.Height);
    for(int i = 0; i< Renderer->Count;i++){
        RenderCommand Command = Renderer->Commands[i];
        hmm_vec4 vertices[4] = {};
        hmm_vec4 Color = {0.5f,0.5f,1,1};
        switch(Command.Type){
            case RenderCommand_DrawBitmap:
                vertices[0].XY = HMM_Vec2(0,0);
                vertices[1].XY = HMM_Vec2(1,0);
                vertices[2].XY = HMM_Vec2(0,1);
                vertices[3].XY = HMM_Vec2(1,1);
                vertices[0].ZW = HMM_Vec2(Command.Vertices[0].X ,screenbounds.Y - Command.Vertices[0].Y )/(screenbounds/2);
                vertices[1].ZW = HMM_Vec2(Command.Vertices[1].X ,screenbounds.Y - Command.Vertices[1].Y )/(screenbounds/2);
                vertices[2].ZW = HMM_Vec2(Command.Vertices[2].X ,screenbounds.Y - (Command.Vertices[2].Y ))/(screenbounds/2);
                vertices[3].ZW = HMM_Vec2(Command.Vertices[3].X ,screenbounds.Y - (Command.Vertices[3].Y ))/(screenbounds/2);
                break;
            case RenderCommand_DrawRect:
                
                break;
            default:
                break;
        }
        Renderer->Commands[i] = {};
    }
    Renderer->Count = 0;
}

bool
RendererAddCommand(RendererContext *Renderer, RenderCommand Command)
{

    if(Renderer->Count < Renderer->MaxCount){
        Renderer->Commands[Renderer->Count] = Command;
        Renderer->Count++;
        return true;
    }
    return false;
}

bool
DrawRectangle(RendererContext *Renderer,float x,float y,float w,float h,float depth,float r,float g,float b,float a)
{
    hmm_vec2 Vertices[4] = {};
    Vertices[0] = HMM_Vec2(x,y);
    Vertices[1] = HMM_Vec2(x+w,y);
    Vertices[2] = HMM_Vec2(x,y+h);
    Vertices[3] = HMM_Vec2(x+w,y+h);
    RenderCommand Command = {};
    Command.Type =RenderCommand_DrawRect;
    Command.Vertices[0] = Vertices[0];
    Command.Vertices[1] = Vertices[1];
    Command.Vertices[2] = Vertices[2];
    Command.Vertices[3] = Vertices[3];
    Command.Depth = depth;
    Command.Color[0] = HMM_Vec4(r,g,b,a);
    Command.Color[1] = HMM_Vec4(r,g,b,a);
    Command.Color[2] = HMM_Vec4(r,g,b,a);
    Command.Color[3] = HMM_Vec4(r,g,b,a);
    return RendererAddCommand(Renderer,Command);    
}


bool
DrawBitmap(RendererContext *Renderer,float x,float y,float depth,BitmapBuffer *Bitmap)
{
    float w = (float)Bitmap->Width;
    float h = (float)Bitmap->Height;
    hmm_vec2 Vertices[4] = {};
    Vertices[0] = HMM_Vec2(x,y);
    Vertices[1] = HMM_Vec2(x+w,y);
    Vertices[2] = HMM_Vec2(x,y+h);
    Vertices[3] = HMM_Vec2(x+w,y+h);
    RenderCommand Command = {};
    Command.Type =RenderCommand_DrawBitmap;
    Command.Vertices[0] = Vertices[0];
    Command.Vertices[1] = Vertices[1];
    Command.Vertices[2] = Vertices[2];
    Command.Vertices[3] = Vertices[3];
    Command.Depth = depth;
    Command.Texture = Bitmap;
    return RendererAddCommand(Renderer,Command);    
}

FileHandle
ReadEntireFile(char *file)
{

    FileHandle Result = {};
    Result.Data =  SDL_LoadFile(file, &(Result.Size));
    return Result;
}


double
GetHiResolutionTime()
{

    return (double) SDL_GetPerformanceCounter()/SDL_GetPerformanceFrequency();
}



bgfx_program_handle_t
LoadShaderProgram(char *shadername)
{
    char buf[256];
    strcpy(buf,"shader\vs_");
    strcat(buf,shadername);
    strcat(buf,".bin");
    FileHandle VSFile = ReadEntireFile(buf);
    buf[0] =0;
    strcat(buf,"shader\fs_");
    strcat(buf,shadername);
    strcat(buf,".bin");
    FileHandle FSFile = ReadEntireFile(buf);
    bgfx_shader_handle_t VS = bgfx_create_shader(bgfx_make_ref(VSFile.Data,VSFile.Size));
    bgfx_shader_handle_t FS = bgfx_create_shader(bgfx_make_ref(FSFile.Data,FSFile.Size));
    bgfx_program_handle_t Program = bgfx_create_program(VS,FS,true);
    return Program;
}

bgfx_texture_handle_t
LoadTexture(char *name)
{
    char buf[256];
    strcpy(buf,"texture\\");
    strcat(buf,name);
    FileHandle BitmapFile = ReadEntireFile(buf);
    int x = 0;
    int y = 0;
    int channel = 0;
    stbi_uc *bitmapdata = stbi_load_from_memory((stbi_uc *)BitmapFile.Data,BitmapFile.Size,&x,&y,&channel,4);  
    bgfx_texture_handle_t Texture = bgfx_create_texture_2d(x,y,false,1,BGFX_TEXTURE_FORMAT_RGBA8,BGFX_TEXTURE_SRGB |BGFX_SAMPLER_U_CLAMP| BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_SAMPLER_MIN_POINT|BGFX_SAMPLER_MAG_POINT,bgfx_make_ref(bitmapdata,x*y*4));
    return Texture;
}


static bgfx_vertex_decl_t VertexPostionColorTextureDecl = VertexPostionColorTextureDeclaration();
static bgfx_vertex_decl_t VertexPostionColorDecl = VertexPostionColorDeclaration();
static bgfx_vertex_decl_t VertexPostionTextureDecl = VertexPostionTextureDeclaration();

int
main(int argc,char **argv)
{

    RendererContext *Renderer = (RendererContext *)calloc(sizeof(RendererContext),1);
    RendererInit(WINDOW_WIDTH,WINDOW_HEIGHT,Renderer);
    
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_DisplayMode CurrentDisplay;
    SDL_GetCurrentDisplayMode(0, &CurrentDisplay);
    
    SDL_Window *Window = SDL_CreateWindow("Game SDL",10,10,Renderer->Settings.Width,Renderer->Settings.Height,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_SysWMinfo info = {};
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(Window,&info);
   
    BitmapBuffer backBuffer = CreateBitmapBuffer(WINDOW_WIDTH,WINDOW_HEIGHT);

    {
        uint32_t *Pixel = (uint32_t *)backBuffer.Data;
        for(int i =0;i < backBuffer.Width * backBuffer.Height;i++ ){

            *Pixel = 0xff0000ff;
            Pixel++;
        }
    }

    bgfx_texture_handle_t Buffer = {};
    int64_t TimeOffset = 0;
    bgfx_platform_data_t pd = {};
    pd.nwh = info.info.win.window;
    bgfx_set_platform_data(&pd);
    uint32_t reset  =  BGFX_RESET_VSYNC ;
    bgfx_init_t InitData = {};
    InitData.type = BGFX_RENDERER_TYPE_DIRECT3D11;
    uint32_t debug  = BGFX_DEBUG_TEXT;
    bgfx_init_ctor(&InitData);
    bgfx_init(&InitData);
    bgfx_reset(Renderer->Settings.Width, Renderer->Settings.Height, reset, InitData.resolution.format);
    bgfx_set_debug(debug);
    bgfx_set_view_rect(0, 0, 0, Renderer->Settings.Width, Renderer->Settings.Height);    
    bgfx_set_view_clear(0,BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH,0x303030ff,1,0);       
    
    VertexPostionColor QuadVertices[] =
        {
            { -1,  -1,  0.f, 0xffffffff },
            { 1,  -1,  0.f, 0xffffffff },
            { -1, 1,  0.f,0xffffffff },
            { 1, 1,  0.f,0xffffffff },
        };

    
    VertexPostionTexture QuadVerticesTextured[] =
        {
            { -1,  -1,  0.f, 0 ,0 },
            { 1,  -1,  0.f, 1,0 },
            { -1, 1,  0.f, 0,1 },
            { 1, 1,  0.f, 1,1 },
        };

    VertexPostionColorTexture QuadVerticesColorTextured[] =
        {
            { -0.9,-0.9,0.9, 0,0 ,0xffffffff},
            { 0.9,-0.9,0.9, 1,0 ,0xffffffff},
            { -0.9,0.9,0.9, 0,1 ,0xffffffff},
            { 0.9,0.9,0.9, 1,1 ,0xffffffff},
        };


    const uint16_t QuadTriStrip[] =
        {
            0, 3, 2,
            0, 1, 3
        };
    
    bgfx_texture_handle_t Texture = LoadTexture("map_02.png");
    bgfx_vertex_buffer_handle_t VertexBuffer = bgfx_create_vertex_buffer(bgfx_make_ref(QuadVerticesColorTextured, sizeof(QuadVerticesColorTextured)),
                                             &VertexPostionColorTextureDecl,BGFX_BUFFER_NONE);
    bgfx_index_buffer_handle_t IndexBuffer = bgfx_create_index_buffer(bgfx_make_ref(QuadTriStrip, sizeof(QuadTriStrip)),BGFX_BUFFER_NONE);
    bgfx_uniform_handle_t s_texColor = bgfx_create_uniform("s_texColor", BGFX_UNIFORM_TYPE_INT1,1);
    bgfx_program_handle_t Program = LoadShaderProgram("texquadcolor");    
        double t = 0;
    double dt = 0.016f;
    double CurrentTime = GetHiResolutionTime();
    double Accumulator = 0;
    bool running = true;
    SDL_Event event = {};
    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    running = false;
                }break;

                case SDL_MOUSEMOTION:
                {

                }break;
                case SDL_WINDOWEVENT:
                {
                    switch (event.window.event)
                    {

                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            Renderer->Settings.Width = event.window.data1;
                            Renderer->Settings.Height = event.window.data2;
                            bgfx_init_ctor(&InitData);
                            bgfx_init(&InitData);
                            bgfx_reset(Renderer->Settings.Width, Renderer->Settings.Height, reset, InitData.resolution.format);
                        }break;
                        default:
                            break;
                    }break;
                    default:
                        break;
                }
            }
        }

        double NewTime = GetHiResolutionTime();
        double FrameTime = NewTime - CurrentTime;

        if( FrameTime > 0.25f )
        {
            FrameTime = 0.25f;
        }

        CurrentTime = NewTime;
        Accumulator += FrameTime;

        while( Accumulator >= dt )
        {
            t += dt;
            Accumulator -= dt;
            
        }
        MouseState MouseData = GetMouseState();
        const double tAlpha = Accumulator / dt;

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_SPACE])
        {
            
        }
        if(state[SDL_SCANCODE_ESCAPE])
        {
            running = false;
        }
        bgfx_touch(0);

        bgfx_set_transform(HMM_Mat4d(1).Elements,1);
        
        bgfx_set_texture(0,s_texColor,Texture,UINT32_MAX);
        bgfx_set_vertex_buffer(0,VertexBuffer,0,ARRAYCOUNT(QuadVerticesColorTextured));
        bgfx_set_index_buffer(IndexBuffer,0,ARRAYCOUNT(QuadTriStrip));
        uint64_t drawstate = (BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_ALWAYS);
        bgfx_set_state(drawstate,0);
        bgfx_submit(0,Program,0,false);
        
        bgfx_dbg_text_clear(0,false);
        bgfx_dbg_text_printf(0,0,0x4f,"FrameTime:  %fms FPS: %f",FrameTime*1000,1/FrameTime);
        const bgfx_stats_t* stats = bgfx_get_stats();
        bgfx_dbg_text_printf(0, 1, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
        bgfx_dbg_text_printf(0, 2, 0x0f, "numDraw :%u maxGpuLatency:%u numDynamicIndexBuffers :%u numDynamicVertexBuffers :%u numFrameBuffers :%u numIndexBuffers :%u numOcclusionQueries :%u numPrograms :%u",stats->numDraw,stats->maxGpuLatency,stats->numDynamicIndexBuffers,stats->numDynamicVertexBuffers,stats->numFrameBuffers,stats->numIndexBuffers,stats->numOcclusionQueries,
                             stats->numPrograms);
        bgfx_dbg_text_printf(0,3,0x0f,"numShaders :%u numTextures :%u numUniforms :%u numVertexBuffers :%u numVertexDecls :%u",stats->numShaders,
                             stats->numTextures,
                             stats->numUniforms,
                             stats->numVertexBuffers,
                             stats->numVertexDecls);
        bgfx_dbg_text_printf(0, 4, 0x0f, "Mouse Pos: X:%.f,Y:%.f",MouseData.x,MouseData.y);
        bgfx_dbg_text_printf(0, 5, 0x0f, "Mouse Button: Left:%s, Right:%s Middle:%s",MouseData.Left== true ? "true" : "false",MouseData.Right== true ? "true" : "false",MouseData.Middle== true ? "true" : "false");
        bgfx_frame(false);
       
    }

    bgfx_shutdown();

    SDL_Quit();
    
    return 0;
}
