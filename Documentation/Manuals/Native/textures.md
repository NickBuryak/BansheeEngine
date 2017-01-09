Textures									{#textures}
===============
[TOC]

Textures in Banshee are represented with the @ref bs::Texture "Texture" and @ref bs::ct::Texture "ct::Texture" classes. Both of these provide almost equivalent functionality, but the former is for use on the simulation thread, and the latter is for use on the core thread. If you are confused by the dual nature of the objects, read the [core thread](@ref coreThread) manual. 

We're going to focus on the simulation thread implementation in this manual, and then note the differences in the core thread version at the end.

# Creating a texture {#textures_a}
To create a texture call @ref bs::Texture::create "Texture::create" or one if its overloads. You'll need to populate the @ref bs::TEXTURE_DESC "TEXTURE_DESC" structure and pass it as a parameter. At minimum you need to provide a @ref bs::TextureType "texture type", dimensions and @ref bs::PixelFormat "pixel format". The dimensions range from one to three dimensional depending on the texture type.

Optionally you can also provide the number of mipmaps, number of samples, usage flags and a gamma correction flag:
 - A texture with mip-maps will contain a set of scaled down versions of itself that are used by the GPU for special filtering. 
 - A texture with more than one sample can be used only for rendering (see the [render targets](@ref renderTargets) manual), and is useful for antialiasing.
 - @ref bs::TextureUsage "Usage flags" specify how is the texture allowed to be used.
 - Gamma correction flag specifies if the data in the texture has been gamma corrected. If enabled the GPU will transform the texture data back to linear space when it is accessing it. If the texture is already in linear space, or you do not need it to be in linear space you can leave this off.
 
For example:
~~~~~~~~~~~~~{.cpp}
// Creates a 2D texture, 128x128 with an 8-bit RGBA format
TEXTURE_DESC desc;
desc.type = TEX_TYPE_2D;
desc.width = 128;
desc.heigth = 128;
desc.format = PF_R8G8B8A8;

HTexture texture = Texture::create(desc);
~~~~~~~~~~~~~ 

You can also create a non-empty texture by creating it with a populated @ref bs::PixelData "PixelData" object. More about @ref bs::PixelData "PixelData" later.
 
# Accessing properties {#textures_b} 
You can access all relevant information about a texture (e.g. width, height) by calling @ref bs::Texture::getProperties() "Texture::getProperties()" which will return an instance of @ref bs::TextureProperties "TextureProperties". 
 
# Reading/writing {#textures_c}
To read and write from/to the texture use the @ref bs::Texture::readData "Texture::readData" and @ref bs::Texture::writeData "Texture::writeData" methods. These expect a face and mipmap index to read/write to, and a @ref bs::PixelData "PixelData" object.

@ref bs::PixelData "PixelData" object is just a container for a set of pixels. You can create one manually or use @ref bs::TextureProperties::allocBuffer "TextureProperties::allocBuffer" to create the object of valid size and format for the specified sub-resource index. When reading from the texture the buffer will be filled with pixels from the texture, and when writing you are expected to populate the object.

Be aware that read and write operations are asynchronous and you must follow the rules for @ref asyncMethod "asynchronous methods".

## PixelData {#textures_c_a}
You can create @ref bs::PixelData "PixelData" manually by calling @ref bs::PixelData::create "PixelData::create" and providing it with dimensions and pixel format. When working with textures you must ensure that the dimensions and the format matches the texture sub-resource.

Once created you can use @ref bs::PixelData::getColorAt "PixelData::getColorAt", @ref bs::PixelData::setColorAt "PixelData::setColorAt", @ref bs::PixelData::getColors "PixelData::getColors" and @ref bs::PixelData::setColors "PixelData::setColors" to read/write colors from/to its internal buffer.

You can also use @ref bs::PixelUtil "PixelUtil" to perform various operations on the pixels. This includes generating mip maps, converting to different pixel formats, compressing, scaling and other.

## Cached CPU data {#textures_c_b}
When you read from a texture using the @ref bs::Texture::readData "Texture::readData" method the read will be performed from the GPU. This is useful if the GPU has in some way modified the texture, but will also incur a potentially large performance penalty because it will introduce a CPU-GPU synchronization point. In a lot of cases you might just want to read pixels from a texture that was imported or created on the CPU in some other way.

For this reason @ref bs::Texture::readCachedData "Texture::readCachedData" exists. It will read data quickly with little performance impact. However you must create the texture using the @ref bs::TU_CPUCACHED "TU_CPUCACHED" usage. This also means that the texture will keep a copy of its pixels in system memory, so use it sparingly. If the texture is modified from the GPU this method will not reflect such changes.

# Rendering using the texture {#textures_d}
To use a texture for rendering you need to either:
 - (High level) Assign it to a @ref bs::Material "Material" which will then automatically get used on a @ref bs::Renderable "Renderable" which uses the material. Read the [material manual](@ref materials) for more information.
 - (Low level) Bind the texture to a @ref bs::GpuParams "GpuParams" object, which can then be assigned to pipeline for rendering. Read the [render API manual](@ref renderAPI) for more information.

# Saving/loading {#textures_e}
A texture is a @ref bs::Resource "Resource" and can be saved/loaded like any other. See the [resource](@ref resources) manual.

# Core thread textures {#textures_f}
So far we have only talked about the simulation thread @ref bs::Texture "Texture" but have ignored the core thread @ref bs::ct::Texture "ct::Texture". The functionality between the two is mostly the same, with the major difference being that the core thread version doesn't have asychronous write/read methods, and those operations are instead performed immediately.

You can also use @ref bs::ct::Texture::lock "ct::Texture::lock" and @ref bs::ct::Texture::unlock "ct::Texture::unlock" to get access to the texture buffer, which allows you to only read/write from/to portions of it, instead of always writing to the entire buffer.

And finally @ref bs::ct::Texture::copy "ct::Texture::copy" method can be used for quickly copying a contents of one texture to another texture. This method will also resolve multi-sampled surface in the case the source is multi-sampled but the destination is not.

# Load-store textures {#textures_g}
Load-store textures are a special type of textures that can be written to by the GPU. This is opposed to normal textures which are read only. They are particularily useful for compute operations which cannot use render targets for output, or for GPU operations that which to write to arbitrary locations rather than just to their own pixel location.

To create a load-store texture just provide the @ref bs::TU_LOADSTORE "TU_LOADSTORE" usage flag on creation - the rest of the creation procedure is identical. Load-store textures cannot be multisampled and cannot be used as render or depth-stencil targets. They also cannot have mip-maps nor can they be created with compressed texture formats.

Rendering using these textures is similar to normal textures, but when binding them to @ref bs::Material "Material" or @ref bs::GpuParams "GpuParams" they also require a separate @ref bs::TextureSurface "TextureSurface" parameter to determine the surface of the texture to bind, in case it has multiple surfaces or mip levels.