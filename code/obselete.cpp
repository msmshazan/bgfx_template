/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */

// TODO(Shazan): Why have a code dump in the first place?
// NOTE(Shazan): ALL UNUSED CODE GO HERE



typedef struct{
    TTF_Font *Handle;
    size_t Size;
}FontData;

typedef struct{
    int advance;
    int w;
}GlyphData;

typedef struct{
    uint32_t GlyphCount;
    uint32_t GlyphStart;
    BitmapBuffer Bitmap;
    GlyphData *Glyphs;
    uint32_t Size;
}FontBuffer;


FontBuffer GetCharBitmapFromFontASCII(char * File,uint8_t size) 
{
    FontBuffer Buffer = {};
    Buffer.GlyphCount = 96;
    Buffer.GlyphStart = 32;
    char *text= (char *)calloc((Buffer.GlyphCount + 1)*sizeof(uint32_t),1);
    for(int i= 0 ; i < Buffer.GlyphCount ; i++){
        text[i] = (char)(i+Buffer.GlyphStart);
    }
    text[Buffer.GlyphCount] = 0;
    Buffer.Size = size;
    TTF_Font *Font = TTF_OpenFont(File,Buffer.Size);
    int IsMonoSpaced = TTF_FontFaceIsFixedWidth(Font);
    TTF_SetFontHinting(Font,TTF_HINTING_NORMAL);
    SDL_Color Col = {0xff,0xff,0xff,0xff};
    SDL_Surface *Bitmap = TTF_RenderText_Blended(Font,text,Col);
    free(text);
    int w = Bitmap->w;
    int h = Bitmap->h;
    uint32_t *bitmap =(uint32_t *) Bitmap->pixels;
    Buffer.Bitmap = CreateBitmapBuffer(w,h);
    hmm_vec4 Color = HMM_Vec4(1,1,1,1); 
    uint32_t *Pixel = (uint32_t *)Buffer.Bitmap.Data;
    for(int i = 0 ; i <h  ;i++){
        for(int t = 0; t <w; t++){
            *Pixel = bitmap[(i*w)+t];
            Pixel++;
        }
    }
    TTF_CloseFont(Font);
    char buf[256] = {};
    strcpy(buf,File);
    strcat(buf,".bmp");
    SDL_SaveBMP(Bitmap,buf);
    SDL_FreeSurface(Bitmap);
    return Buffer;
}


FontBuffer GetCharBitmapFromFontUTF8ByRange(char * File,uint8_t size,uint32_t GlyphStart,uint32_t GlyphCount) 
{
    FontBuffer Buffer = {};
    Buffer.Size = size;
    TTF_Font *Font = TTF_OpenFont(File,Buffer.Size);
    Buffer.GlyphCount = GlyphCount;
    Buffer.GlyphStart = GlyphStart;
    uint32_t *text= (uint32_t *)calloc((Buffer.GlyphCount +1 )*sizeof(uint32_t),1);
    Buffer.Glyphs= (GlyphData *)calloc((Buffer.GlyphCount)*sizeof(GlyphData),1);
    {
        int advance = 0;
        for(uint32_t i= 0 ; i < Buffer.GlyphCount ; i++){
            text[i] = i+Buffer.GlyphStart;
            if(!(TTF_GlyphIsProvided(Font,(uint16_t) text[i])))
            {
                text[i] = ' ';
            }
            char CodePoint[4] = {};
            int w=0;
            int h=0;
            tuEncode8(CodePoint,text[i]);
            TTF_SizeUTF8(Font,CodePoint,&w,&h);
            Buffer.Glyphs[i].advance = advance;
            Buffer.Glyphs[i].w = w;
            advance +=w;        
        }
    }
    text[Buffer.GlyphCount] = 0;
    int *decodedtext = (int *)text;
    char *encodedtext  = (char*)calloc( (Buffer.GlyphCount*4) + 1,1 );
    char * utf8_enc = encodedtext;
    {
        for(int i =0 ;i < Buffer.GlyphCount ;i++)
        {
            utf8_enc = tuEncode8(utf8_enc,decodedtext[i]);
        }
    }
    encodedtext[Buffer.GlyphCount*4] = 0;
    int IsMonoSpaced = TTF_FontFaceIsFixedWidth(Font);
    TTF_SetFontHinting(Font,TTF_HINTING_NORMAL);
    SDL_Color Col = {0xff,0xff,0xff,0xff};
    SDL_Surface *Bitmap = TTF_RenderUTF8_Blended(Font,encodedtext,Col);    
    free(text);
    free(encodedtext);
    int w = Bitmap->w;
    int h = Bitmap->h;
    uint32_t *bitmap =(uint32_t *) Bitmap->pixels;
    Buffer.Bitmap = CreateBitmapBuffer(w,h);
    hmm_vec4 Color = HMM_Vec4(1,1,1,1); 
    uint32_t *Pixel = (uint32_t *)Buffer.Bitmap.Data;
    for(int i = 0 ; i <h  ;i++){
        for(int t = 0; t <w; t++){
            *Pixel = bitmap[(i*w)+t];
            Pixel++;
        }
    }
    TTF_CloseFont(Font);
    SDL_FreeSurface(Bitmap);
    return Buffer;
}


FontBuffer GetCharBitmapFromFontUTF8(char * File,uint8_t size) 
{
    return GetCharBitmapFromFontUTF8ByRange(File,size,32,93);
}

hmm_vec4* PrintTextASCII(char text[],FontBuffer *FontBitmap,hmm_vec2 screenbounds,hmm_vec2 Pos,int textcount ){

    hmm_vec4 *glyphpos = (hmm_vec4 *)malloc(sizeof(hmm_vec4)*textcount*4);
    hmm_vec2 fontbounds = HMM_Vec2(FontBitmap->Bitmap.Width,FontBitmap->Bitmap.Height);
    float w = ((FontBitmap->Bitmap.Width ) / FontBitmap->GlyphCount )  ;
    float h =FontBitmap->Bitmap.Height;
    hmm_vec2 InitPos = Pos;
    hmm_vec2 Size = {w,h};
    
    for(int i = 0 ; i < textcount;i++){
        if(text[i] == 0) break;
        if(text[i] == '\n'){
            Pos.Y += Size.Y;
            Pos.X = InitPos.X;
        }else{
            int glyphindex = text[i] - (char)(FontBitmap->GlyphStart);
            glyphpos[i*4 + 0].XY = HMM_Vec2((w*glyphindex),0) /fontbounds;
            glyphpos[i*4 + 1].XY = HMM_Vec2((w*glyphindex)+w,0) /fontbounds;
            glyphpos[i*4 + 2].XY = HMM_Vec2((w*glyphindex),h) /fontbounds;
            glyphpos[i*4 + 3].XY = HMM_Vec2((w*glyphindex)+w,h) /fontbounds;

            glyphpos[i*4 + 0].ZW = HMM_Vec2(Pos.X ,screenbounds.Y - Pos.Y)/(screenbounds/2);
            glyphpos[i*4 + 1].ZW = HMM_Vec2(Pos.X + Size.X,screenbounds.Y - Pos.Y) /(screenbounds/2);
            glyphpos[i*4 + 2].ZW = HMM_Vec2(Pos.X ,screenbounds.Y - (Pos.Y + Size.Y)) /(screenbounds/2);
            glyphpos[i*4 + 3].ZW = HMM_Vec2(Pos.X + Size.X,screenbounds.Y - (Pos.Y + Size.Y)) /(screenbounds/2);

            Pos.X +=Size.X;
        }
    }

    return glyphpos;
}


hmm_vec4* PrintTextUTF8(uint32_t *text,FontBuffer *FontBitmap,hmm_vec2 screenbounds,hmm_vec2 Pos,int textcount ){

    hmm_vec4 *glyphpos = (hmm_vec4 *)malloc(sizeof(hmm_vec4)*textcount*4);
    hmm_vec2 fontbounds = HMM_Vec2(FontBitmap->Bitmap.Width,FontBitmap->Bitmap.Height);
    float w = ((FontBitmap->Bitmap.Width ) / FontBitmap->GlyphCount )  ;
    float h =FontBitmap->Bitmap.Height;
    hmm_vec2 InitPos = Pos;
    hmm_vec2 Size = {w,h};
    for(int i = 0 ; i < textcount;i++){

        if(text[i] == 0) break;
        if(text[i] == '\n'){ // NOTE : '\n' == 0x0a
            Pos.Y += Size.Y;
            Pos.X = InitPos.X;
        }else{
            
            int glyphindex = text[i] - (FontBitmap->GlyphStart);
            if(!((glyphindex >= 0) && (glyphindex < FontBitmap->GlyphCount))){
                glyphindex = 0;
            }
            GlyphData Data = FontBitmap->Glyphs[glyphindex];
            
            glyphpos[i*4 + 0].XY = HMM_Vec2(Data.advance,0) /fontbounds;
            glyphpos[i*4 + 1].XY = HMM_Vec2(Data.advance+Data.w,0) /fontbounds;
            glyphpos[i*4 + 2].XY = HMM_Vec2(Data.advance,h) /fontbounds;
            glyphpos[i*4 + 3].XY = HMM_Vec2(Data.advance+Data.w,h) /fontbounds;

            Size.X = Data.w;
            glyphpos[i*4 + 0].ZW = HMM_Vec2(Pos.X ,screenbounds.Y - Pos.Y)/(screenbounds/2);
            glyphpos[i*4 + 1].ZW = HMM_Vec2(Pos.X + Size.X,screenbounds.Y - Pos.Y) /(screenbounds/2);
            glyphpos[i*4 + 2].ZW = HMM_Vec2(Pos.X ,screenbounds.Y - (Pos.Y + Size.Y)) /(screenbounds/2);
            glyphpos[i*4 + 3].ZW = HMM_Vec2(Pos.X + Size.X,screenbounds.Y - (Pos.Y + Size.Y)) /(screenbounds/2);

            Pos.X +=Size.X;
        }
    }

    return glyphpos;
}

FontData OpenFont(char * File,size_t size){

    FontData Data = {};
    Data.Size = size;
    Data.Handle = TTF_OpenFont(File,Data.Size);
    return Data;    
}

BitmapBuffer DrawUTF8(uint32_t *dectext, FontData *Data,int textcount ){

    uint32_t *decodedtext = dectext;
    char *encodedtext  = (char*)calloc( sizeof(char)*((textcount*4) + 1),1 );
    char * utf8_enc = encodedtext;
    int MaxW = 0;
    int MaxH = 0;
    TTF_SetFontHinting(Data->Handle,TTF_HINTING_NORMAL);
    SDL_Color Col = {0xff,0xff,0xff,0xff};
    size_t BitmapCount = 0;
    const int MaxBitmaps = 256;
    SDL_Surface *Bitmaps[MaxBitmaps] = {};
    for(int i =0 ;i < textcount ;i++)
    {
        if(decodedtext[i] == '\r'){
            Bitmaps[BitmapCount] = TTF_RenderUTF8_Blended(Data->Handle,encodedtext,Col);
            if(Bitmaps[BitmapCount]) BitmapCount++;
            for(int t = 3; t < (textcount*4) + 1; t++){
                encodedtext[t] = 0;
            }
            utf8_enc = encodedtext + 3;
            i++;
        }else{
            utf8_enc = tuEncode8(utf8_enc,decodedtext[i]);
        }
    }
    Bitmaps[BitmapCount] = TTF_RenderUTF8_Blended(Data->Handle,encodedtext,Col);
    if(Bitmaps[BitmapCount]) BitmapCount++;        
    free(encodedtext);
    for(int p = 0 ; p < BitmapCount;p++){
        MaxW = Bitmaps[p]->w > MaxW ? Bitmaps[p]->w :MaxW ;
        MaxH += Bitmaps[p]->h ;
    }
    BitmapBuffer TextBitmap = CreateBitmapBuffer(MaxW,MaxH);
    int PosY = 0;
    uint32_t *Pixel = (uint32_t *)(TextBitmap.Data);
    for(int p = 0 ; p < BitmapCount;p++){
        for(int i = 0; i <  Bitmaps[p]->h;i++){
            for(int t = 0; t < Bitmaps[p]->w; t++){
                int index = ((i)*Bitmaps[p]->w)+t;
                *Pixel = ((uint32_t *)Bitmaps[p]->pixels)[index];
                Pixel++;
            }
            Pixel -= Bitmaps[p]->w;
            Pixel += TextBitmap.Width;
        }
        SDL_FreeSurface(Bitmaps[p]);
    }
    return TextBitmap;
}

