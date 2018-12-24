#if !defined(BGFX_EMBED_SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */

#define BX_COUNTOF(_x) sizeof(_x)/sizeof(_x[0])
///
#define BX_CONCATENATE(_x, _y) BX_CONCATENATE_(_x, _y)
#define BX_CONCATENATE_(_x, _y) _x ## _y

#include <bgfx/c99/bgfx.h>

#define BGFX_EMBEDDED_SHADER_DXBC(...)
#define BGFX_EMBEDDED_SHADER_DX9BC(...)
#define BGFX_EMBEDDED_SHADER_PSSL(...)
#define BGFX_EMBEDDED_SHADER_ESSL(...)
#define BGFX_EMBEDDED_SHADER_GLSL(...)
#define BGFX_EMBEDDED_SHADER_SPIRV(...)
#define BGFX_EMBEDDED_SHADER_METAL(...)

#define BGFX_PLATFORM_SUPPORTS_DX9BC (0 \
        || BX_PLATFORM_WINDOWS          \
        )
#define BGFX_PLATFORM_SUPPORTS_DXBC (0  \
        || BX_PLATFORM_WINDOWS          \
        || BX_PLATFORM_WINRT            \
        || BX_PLATFORM_XBOXONE          \
        )
#define BGFX_PLATFORM_SUPPORTS_PSSL (0  \
        || BX_PLATFORM_PS4              \
        )
#define BGFX_PLATFORM_SUPPORTS_ESSL (0  \
        || BX_PLATFORM_ANDROID          \
        || BX_PLATFORM_EMSCRIPTEN       \
        || BX_PLATFORM_IOS              \
        || BX_PLATFORM_LINUX            \
        || BX_PLATFORM_OSX              \
        || BX_PLATFORM_RPI              \
        || BX_PLATFORM_STEAMLINK        \
        || BX_PLATFORM_WINDOWS          \
        )
#define BGFX_PLATFORM_SUPPORTS_GLSL (0  \
        || BX_PLATFORM_BSD              \
        || BX_PLATFORM_LINUX            \
        || BX_PLATFORM_OSX              \
        || BX_PLATFORM_WINDOWS          \
        )
#define BGFX_PLATFORM_SUPPORTS_METAL (0 \
        || BX_PLATFORM_IOS              \
        || BX_PLATFORM_OSX              \
        )
#define BGFX_PLATFORM_SUPPORTS_SPIRV (0 \
        || BX_PLATFORM_ANDROID          \
        || BX_PLATFORM_LINUX            \
        || BX_PLATFORM_WINDOWS          \
        )

#if BGFX_PLATFORM_SUPPORTS_DX9BC
#   undef  BGFX_EMBEDDED_SHADER_DX9BC
#   define BGFX_EMBEDDED_SHADER_DX9BC(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _dx9 ), BX_COUNTOF(BX_CONCATENATE(_name, _dx9 ) ) },
#endif // BGFX_PLATFORM_SUPPORTS_DX9BC

#if BGFX_PLATFORM_SUPPORTS_DXBC
#   undef  BGFX_EMBEDDED_SHADER_DXBC
#   define BGFX_EMBEDDED_SHADER_DXBC(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _dx11), BX_COUNTOF(BX_CONCATENATE(_name, _dx11) ) },
#endif // BGFX_PLATFORM_SUPPORTS_DXBC

#if BGFX_PLATFORM_SUPPORTS_PSSL
#   undef  BGFX_EMBEDDED_SHADER_PSSL
#   define BGFX_EMBEDDED_SHADER_PSSL(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _pssl), BX_CONCATENATE(_name, _pssl_size) },
#endif // BGFX_PLATFORM_SUPPORTS_PSSL

#if BGFX_PLATFORM_SUPPORTS_ESSL
#   undef  BGFX_EMBEDDED_SHADER_ESSL
#   define BGFX_EMBEDDED_SHADER_ESSL(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _glsl), BX_COUNTOF(BX_CONCATENATE(_name, _glsl) ) },
#endif // BGFX_PLATFORM_SUPPORTS_ESSL

#if BGFX_PLATFORM_SUPPORTS_GLSL
#   undef  BGFX_EMBEDDED_SHADER_GLSL
#   define BGFX_EMBEDDED_SHADER_GLSL(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _glsl), BX_COUNTOF(BX_CONCATENATE(_name, _glsl) ) },
#endif // BGFX_PLATFORM_SUPPORTS_GLSL

#if BGFX_PLATFORM_SUPPORTS_SPIRV
#   undef  BGFX_EMBEDDED_SHADER_SPIRV
#   define BGFX_EMBEDDED_SHADER_SPIRV(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _spv), BX_COUNTOF(BX_CONCATENATE(_name, _spv) ) },
#endif // BGFX_PLATFORM_SUPPORTS_SPIRV

#if BGFX_PLATFORM_SUPPORTS_METAL
#   undef  BGFX_EMBEDDED_SHADER_METAL
#   define BGFX_EMBEDDED_SHADER_METAL(_renderer, _name) \
        { _renderer, BX_CONCATENATE(_name, _mtl), BX_COUNTOF(BX_CONCATENATE(_name, _mtl) ) },
#endif // BGFX_PLATFORM_SUPPORTS_METAL

#define BGFX_EMBEDDED_SHADER(_name)                                                                \
            {                                                                                      \
                #_name,                                                                            \
                {                                                                                  \
                    BGFX_EMBEDDED_SHADER_DX9BC(BGFX_RENDERER_TYPE_DIRECT3D9,  _name)              \
                    BGFX_EMBEDDED_SHADER_DXBC (BGFX_RENDERER_TYPE_DIRECT3D11, _name)              \
                    BGFX_EMBEDDED_SHADER_DXBC (BGFX_RENDERER_TYPE_DIRECT3D12, _name)              \
                    BGFX_EMBEDDED_SHADER_PSSL (BGFX_RENDERER_TYPE_GNM,        _name)              \
                    BGFX_EMBEDDED_SHADER_METAL(BGFX_RENDERER_TYPE_METAL,      _name)              \
                    BGFX_EMBEDDED_SHADER_ESSL (BGFX_RENDERER_TYPE_OPENGLES,   _name)              \
                    BGFX_EMBEDDED_SHADER_GLSL (BGFX_RENDERER_TYPE_OPENGL,     _name)              \
                    BGFX_EMBEDDED_SHADER_SPIRV(BGFX_RENDERER_TYPE_VULKAN,     _name)              \
                    { BGFX_RENDERER_TYPE_Noop,  (const uint8_t*)"VSH\x5\x0\x0\x0\x0\x0\x0", 10 }, \
                    { BGFX_RENDERER_TYPE_Count, NULL, 0 }                                         \
                }                                                                                  \
            }

#define BGFX_EMBEDDED_SHADER_END()                         \
            {                                              \
                NULL,                                      \
                {                                          \
                    { BGFX_RENDERER_TYPE_COUNT, NULL, 0 } \
                }                                          \
            }

    struct EmbeddedShader
    {
        struct Data
        {
            bgfx_renderer_type type;
            const uint8_t* data;
            uint32_t size;
        };

        const char* name;
        Data data[BGFX_RENDERER_TYPE_COUNT];
    };

    ShaderHandle bgfx_createEmbeddedShader(EmbeddedShader* _es, bgfx_renderer_type _type, const char* _name)
    {
        for (const EmbeddedShader* es = _es; NULL != es->name; ++es)
        {
            if (0 == bx::strCmp(_name, es->name) )
            {
                for (EmbeddedShader.Data* esd = es->data; BGFX_RENDERER_TYPE_COUNT != esd->type; ++esd)
                {
                    if (_type == esd->type
                    &&  1 < esd->size)
                    {
                        bgfx_shader_handle_t handle = bgfx_create_shader(bgfx_make_ref(esd->data, esd->size) );
                        if (isValid(handle) )
                        {
                            setName(handle, _name);
                        }

                        return handle;
                    }
                }
            }
        }

        ShaderHandle handle = BGFX_INVALID_HANDLE;
        return handle;
    }



#define BGFX_EMBED_SHADER_H
#endif
