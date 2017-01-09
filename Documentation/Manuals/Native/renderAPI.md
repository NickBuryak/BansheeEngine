Render API									{#renderAPI}
===============
[TOC]

Render API is an interface that allows you to perform low-level rendering operations akin to DirectX or OpenGL. In Banshee this API is provided through the @ref bs::RenderAPI "RenderAPI" for the simulation thread, and @ref bs::ct::RenderAPI "ct::RenderAPI" for the core thread. If you are confused by the dual nature of objects, read the [core thread](@ref coreThread) manual. 

For the remainder of this manual we'll focus on the core thread portion of the API, but both provide essentially identical functionality. The main difference that the execution of commands on the simulation thread isn't immediate, instead they are queued on an internal queue which is sent to the core thread at the end of the frame.

Render API lets you manipulate the GPU pipeline by setting various states (depth stencil, blend and rasterizer), binding GPU programs, textures and executing draw calls. In this manual we'll cover how to use this API to perform rendering manually. 

To render using this API you need to:
 - Create and bind a render target
  - Set up a viewport (if rendering to a part of a render target)
  - Clear render target (if we're re-using a render target)
 - Create and bind a pipeline state
   - Create and bind vertex/fragment GPU programs
   - Create and bind depth stencil, blend and/or rasterizer states (optionally)
   - Create and bind geometry/hull/domain GPU program (optionally)
 - Bind GPU program parameters (textures, samplers, etc., if any)
 - Create and bind vertices
  - Create and bind the vertex declaration
  - Create and bind the vertex buffer
 - Create and bind the index buffer (optionally)
 - Issue a draw command
 
We'll cover each step of this process, and at the end also show an alternate pipeline for compute operations.

# Render targets {#renderAPI_a}
Before any rendering can be done you must bind at least one render target. To learn more about, and how to create a render target check out the [render target](@ref renderTargets) manual. 

Binding a render target involves calling @ref bs::ct::RenderAPI::setRenderTarget "ct::RenderAPI::setRenderTarget". This will cause any rendering to be output to the entirety of the render target. Optionally you can also call @ref bs::ct::RenderAPI::setViewport "ct::RenderAPI::setViewport" to select a sub-rectangle of the target to render to. 

Binding a render target means you cannot use it for reading within a GPU program. However if your render target has a depth-buffer you can optionally set the `readOnlyDepthStencil` parameter of @ref bs::ct::RenderAPI::setRenderTarget "ct::RenderAPI::setRenderTarget" to true, which will allow you to have a depth buffer be bound for both depth-testing and reading from the GPU program.
 
Before doing any rendering it's always good to clear the render target to some default value. Use @ref bs::ct::RenderAPI::clearRenderTarget "ct::RenderAPI::clearRenderTarget" to clear the entire target, or @ref bs::ct::RenderAPI::clearViewport "ct::RenderAPI::clearViewport" to clear just the viewport portion of the target. When clearing you can choose whether to clear color, depth or stencil buffers (or all) determined by @ref bs::FrameBufferType "FrameBufferType" flags. You can also choose the values to clear each of the buffers to. And finally if your render target has multiple surfaces, you can choose to clear only some of the surfaces by providing a bitmask.

Once you are done rendering make sure to call @ref bs::ct::RenderAPI::swapBuffers "ct::RenderAPI::swapBuffers" if your render target has multiple buffers (like a window). This will swap the buffers and present the rendered image on the screen.

A simple example covering all these commands:
~~~~~~~~~~~~~{.cpp}
SPtr<RenderTarget> myRt = ...; // Assuming we created this earlier, and it's a double-buffered window

RenderAPI& rapi = RenderAPI::instance();
rapi.setRenderTarget(myRt);
rapi.setViewport(Rect2(0.5f, 0.0f, 0.5f, 1.0f)); // Draw only to the right side of the target

// Clear all buffers: color to white, depth to 1.0, stencil to 0
rapi.clearViewport(FBT_COLOR | FBT_DEPTH | FBT_STENCIL, Color::White, 1.0f, 0);
... execute some draw calls ...
rapi.swapBuffers();
~~~~~~~~~~~~~
 
# Pipeline state {#renderAPI_b}
Before executing the drawing operation you must set up an @ref bs::ct::GraphicsPipelineState "ct::GraphicsPipelineState" object, which contains a set of fixed and programmable states that control primitive rendering. This includes GPU programs (e.g. vertex/fragment) and fixed states (depth-stencil, blend, rasterizer).

To create a pipeline state you must fill out @ref bs::PIPELINE_STATE_DESC "PIPELINE_STATE_DESC" descriptor, and use it to construct the state, like so:
~~~~~~~~~~~~~{.cpp}
PIPELINE_STATE_DESC desc;
// Fixed states (see below on how to create them)
desc.blendState = ...;
desc.rasterizerState = ...;
desc.depthStencilState = ...;

// GPU programs (see below on how to create them)
desc.vertexProgram = ...
desc.fragmentProgram = ...;
desc.geometryProgram = ...;
desc.hullProgram = ...;
desc.domainProgram = ...;

SPtr<GraphicsPipelineState> state = GraphicsPipelineState::create(desc);
~~~~~~~~~~~~~

Once created the pipeline can be bound for rendering by calling @ref bs::ct::RenderAPI::setGraphicsPipeline "ct::RenderAPI::setGraphicsPipeline".

~~~~~~~~~~~~~{.cpp}
// Bind pipeline for use (continued from above)

RenderAPI& rapi = RenderAPI::instance();
rapi.setGraphicsPipeline(state);
~~~~~~~~~~~~~

We continue below with explanation on how to create fixed and programmable states required to initialize GraphicsPipelineState.

## Fixed pipeline states {#renderAPI_b_a}
Fixed pipeline states allow you to control (to some extent) non-programmable parts of the pipeline. This includes anything from blend operations, rasterization mode to depth testing. Setting these states is optional and if not set, default values will be used.

States can be created by:
 - @ref bs::ct::DepthStencilState "ct::DepthStencilState" - Populate @ref bs::DEPTH_STENCIL_STATE_DESC "DEPTH_STENCIL_STATE_DESC" and call @ref bs::ct::DepthStencilState::create "ct::DepthStencilState::create" 
 - @ref bs::ct::BlendState "ct::BlendState" - Populate @ref bs::BLEND_STATE_DESC "BLEND_STATE_DESC" and call @ref bs::ct::BlendState::create "ct::BlendState::create" 
 - @ref bs::ct::RasterizerState "ct::RasterizerState" - Populate @ref bs::RASTERIZER_STATE_DESC "RASTERIZER_STATE_DESC" and call @ref bs::ct::RasterizerState::create "ct::RasterizerState::create" 
 
We won't explain what each of the states does. For that you can check out the class documentation of the states themselves, or familiarize yourself with the modern GPU pipeline in general, as the states mirror it exactly.

## GPU programs {#renderAPI_b_b}
The pipeline state also requires you to bind at least one GPU program (programmable state). At minimum you will need to bind a vertex program, while in most cases you will also need a fragment program. Optionally you can also bind geometry, hull or domain programs for more advanced functionality. To learn how to create GPU programs see [GPU program manual](@ref gpuPrograms).

Most GPU programs also accept a number of parameters, whether textures, buffers, sampler states or primitive values like floats or integers. These parameters are accessed through @ref bs::ct::GpuParams "ct::GpuParams" object. You can use this object to assign individual parameters and then bind the object to the render API using @ref bs::ct::RenderAPI::setGpuParams "ct::RenderAPI::setGpuParams". See below for an example.

~~~~~~~~~~~~~{.cpp}
... assuming graphics pipeline state and relevant GPU programs are created ...
SPtr<GraphicsPipelineState> state = ...;
SPtr<GpuParams> params = GpuParams::create(state);

// Retrieve GPU param handles we can then read/write to
GpuParamVec2 myVectorParam;
GpuParamTexture myTextureParam;

params->getParam(GPT_FRAGMENT_PROGRAM, "myVector", myVectorParam); // Assuming "myVector" is the variable name in the program source code
params->getTextureParam(GPT_FRAGMENT_PROGRAM, "myTexture", myTextureParam); // Assuming "myTexture" is the variable name in the program source code

myVectorParam.set(Vector2(1, 2));
myTextureParam.set(myTexture); // Assuming we created "myTexture" earlier.

// Bind parameters for use 
RenderAPI& rapi = RenderAPI::instance();
rapi.setGpuParams(params);
~~~~~~~~~~~~~
 
All parameters are bound by first retrieving their handles, and then using those handles for parameter access. In the example above we show how to bind a texture and a 2D vector to a GPU program. Same approach follows for all available parameter types.

After the parameters are set, we bind them to the pipeline by calling @ref bs::ct::RenderAPI::setGpuParams "ct::RenderAPI::setGpuParams".

### Data parameters {#renderAPI_c_a_a} 
Handles for data parameters like int, float, 2D vector, etc. can be retrieved by calling @ref bs::ct::GpuParams::getParam "ct::GpuParams::getParam" which can then be assigned to as shown above.

Alternatively you may also assign entire blocks of data parameters by calling @ref bs::ct::GpuParams::setParamBlockBuffer(GpuProgramType,const String&,const ParamsBufferType&) "ct::GpuParams::setParamBlockBuffer". When assigning entire blocks you must create and populate the @ref bs::GpuParamBlockBuffer "GpuParamBlockBuffer" object manually.

When writing to buffers manually you must ensure to write to offsets the GPU program expects the data to be at. You can find thise information from @ref bs::GpuParamDesc "GpuParamDesc" structure accessible from @ref bs::ct::GpuProgram::getParamDesc "ct::GpuProgram::getParamDesc". 

### Texture parameters {#renderAPI_c_a_b} 
Handles for texture parameters can be retrieved by calling @ref bs::ct::GpuParams::getTextureParam "ct::GpuParams::getTextureParam", or @ref bs::ct::GpuParams::getLoadStoreTextureParam "ct::GpuParams::getLoadStoreTextureParam" if the texture should be bound for load-store operations.

Learn more about textures and their different types in the [texture manual](@ref textures).

### Sampler state parameters {#renderAPI_c_a_c}
Sampler states can be used to customize how is the texture sampled. You can retrieve a handle for a sampler state parameter by calling @ref bs::ct::GpuParams::getSamplerStateParam "ct::GpuParams::getSamplerStateParam".

Sampler states are represented by the @ref bs::ct::SamplerState "ct::SamplerState" object, which you can create by populating the @ref bs::SAMPLER_STATE_DESC "SAMPLER_STATE_DESC" and calling @ref bs::ct::SamplerState::create "ct::SamplerState::create". 

An example to create and bind a sampler state:
~~~~~~~~~~~~~{.cpp}

// Use nearest neighbor filtering when sampling
SAMPLER_STATE_DESC ssDesc;
ssDesc.magFilter = FO_POINT;
ssDesc.minFilter = FO_POINT;
ssDesc.mipFilter = FO_POINT;

SPtr<SamplerState> mySamplerState = SamplerState::create(ssDesc);

SPtr<GpuParams> params = ...;

GpuParamSampState mySamplerParam;
params->getSamplerStateParam(GPT_FRAGMENT_PROGRAM, "mySamplerState", mySamplerParam); // Assuming "mySamplerState" is the variable name in the program source code

mySamplerParam.set(mySamplerState);

~~~~~~~~~~~~~

# Vertex buffer {#renderAPI_d}
@ref bs::ct::VertexBuffer "Vertex buffer" is a buffer that contains all vertices of the object we wish to render. When drawing the vertices will be interpreted as primitives (either points, lines or triangles) and rendered. Each vertex can have one or multiple properties associated with it.

To create a vertex buffer call @ref bs::ct::VertexBuffer::create "ct::VertexBuffer::create". You need to know the size of an individual vertex (determined by the properties each vertex requires) and the number of vertices. Optionally if your vertex buffer is used for output from the geometry GPU program you can toggle on the `streamOut` parameter.

Once the vertex buffer is created you will want to populate it with some data (detailed below) and then bind it to the pipeline using @ref bs::ct::RenderAPI::setVertexBuffers "ct::RenderAPI::setVertexBuffers". You can bind one or multiple vertex buffers at once. They all must have the same vertex counts but can have different properties, which will all be fed to the pipeline when rendering.

Creation of an example vertex buffer:
~~~~~~~~~~~~~{.cpp}
// Create a vertex buffer containing 8 vertices with just a vertex position
SPtr<VertexBuffer> vb = VertexBuffer::create(sizeof(Vector3), 8);

RenderAPI& rapi = RenderAPI::instance();
rapi.setVertexBuffers(0, { vb });
~~~~~~~~~~~~~

## Reading/writing {#renderAPI_d_a}
@ref bs::ct::VertexBuffer "ct::VertexBuffer" provides a couple of way to read and write data from/to it:
 - @ref bs::ct::VertexBuffer::lock "ct::VertexBuffer::lock" locks a specific region of the vertex buffer and returns a pointer you can then use for reading and writing. Make sure to specify valid @ref bs::GpuLockOptions "GpuLockOptions" signaling whether you are planning on read or writing from the buffer. Once done call @ref bs::ct::VertexBuffer::unlock "ct::VertexBuffer::unlock" to make the locked region accessible to the GPU again.
 - @ref bs::ct::VertexBuffer::readData "ct::VertexBuffer::readData" and @ref bs::ct::VertexBuffer::writeData "ct::VertexBuffer::writeData" to write or read entire blocks at once, but are more or less similar to the previous method.
 - @ref bs::ct::VertexBuffer::copyData "ct::VertexBuffer::copyData" can be used to efficiently copy data between two vertex buffers.

An example of writing to the vertex buffer:
~~~~~~~~~~~~~{.cpp}
// Create a vertex buffer containing 8 vertices with just a vertex position
SPtr<VertexBuffer> vb = VertexBuffer::create(sizeof(Vector3), 8);

Vector3* positions = (Vector3)vb->lock(0, sizeof(Vector3) * 8, GBL_WRITE_ONLY_DISCARD);
... write to the positions array ...
vb->unlock();
~~~~~~~~~~~~~

When your vertices contain multiple properties it can be difficult to keep track of which offset to write which property, or determine the stride between two vertices. For this purpose you can use @ref bs::VertexDataDesc "VertexDataDesc" which allows you to easily set up vertex properties like so:
~~~~~~~~~~~~~{.cpp}
// Create a vertex with a position, normal and UV coordinates
SPtr<VertexDataDesc> vertexDesc = VertexDataDesc::create();
vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
vertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);
vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD);
~~~~~~~~~~~~~

You can then use methods like @ref bs::VertexDataDesc::getElementSize "VertexDataDesc::getElementSize" to learn the size of a particular element, @ref bs::VertexDataDesc::getVertexStride "VertexDataDesc::getVertexStride" to learn the stride between elements. You can also retrieve detailed per-property information by iterating over all properties with @ref bs::VertexDataDesc::getNumElements "VertexDataDesc::getNumElements" and @ref bs::VertexDataDesc::getElement "VertexDataDesc::getElement". These methods return a @ref bs::VertexElement "VertexElement" which can be used for finding out the offset of the individual element.

To learn more about vertex descriptors read the [mesh](@ref meshes) manual.

## Vertex declaration {#renderAPI_d_b}
Before a vertex buffer can be used for rendering, you need to tell the pipeline in what format are its vertices structured in. You do that by creating a @ref bs::ct::VertexDeclaration "ct::VertexDeclaration" object using the @ref bs::VertexDataDesc "VertexDataDesc" we described in the previous section. This object can then be passed to @ref bs::ct::RenderAPI::setVertexDeclaration "ct::RenderAPI::setVertexDeclaration" to bind it to the pipeline.

For example:
~~~~~~~~~~~~~{.cpp}
SPtr<VertexDataDesc> vertexDesc = ...; // Creating vertex desc as above
SPtr<VertexDeclaration> vertexDecl = VertexDeclaration::create(vertexDesc);

RenderAPI& rapi = RenderAPI::instance();
rapi.setVertexDeclaration(vertexDecl);
~~~~~~~~~~~~~

It is important that the vertex declaration contains properties needed by the bound vertex GPU program, as well as that it matches the vertex layout in the vertex buffer. See the [gpu program](@ref gpuPrograms) manual to learn how to retrieve vertex properties expected by a GPU program.

# Index buffer {#renderAPI_e}
Normally when you draw data from a vertex buffer, the vertices are assumed to form primitives sequentially (e.g. every three vertices is a triangle). By using an @ref bs::ct::IndexBuffer "index buffer" you an provide an additional layer of abstraction. Index buffer is fully optional, but when bound it will be used for forming primitives instead of the vertex buffer (i.e. every three indices will form a triangle). Each entry in an index buffer points to a vertex in the vertex buffer. This level of abstraction allows you to re-use the same vertex in multiple primitives, as well as create more optimal vertex order for GPU processing.

To create an index buffer call @ref bs::ct::IndexBuffer::create "ct::IndexBuffer::create". It expects a number of indices, and the type of indices. Index type can be either 16- or 32-bit. To bind an index buffer to the pipeline call @ref bs::ct::RenderAPI::setIndexBuffer "ct::RenderAPI::setIndexBuffer".

Reading and writing from/to the index buffer has the identical interface to the vertex buffer, so we won't show it again.

# Drawing {#renderAPI_f}
Once all the previous states, programs and buffers have been set up, we can finally render our object. First of we must set up the type of primitives we wish to render by calling @ref bs::ct::RenderAPI::setDrawOperation "ct::RenderAPI::setDrawOperation" with a @ref bs::DrawOperationType "DrawOperationType" specifying the primitive type. This determines how the the vertices (or indices) in our buffers interpreted.

After that you can issue a @ref bs::ct::RenderAPI::draw "ct::RenderAPI::draw" call if rendering without an index buffer. It expects the vertex index to start rendering from, and the number of vertices to render. The number of vertices must be divisible by the number of vertices expected by the @ref bs::DrawOperationType "DrawOperationType" you're using (e.g. three for triangles, two for lines, one for points). The vertices will then be pulled from the vertex buffers, processed by the fixed pipeline controlled by the states, and by the programmable pipeline controlled by the GPU programs and the output image will be rendered to the bound render target.

If using an index buffer you should issue a @ref bs::ct::RenderAPI::drawIndexed "ct::RenderAPI::drawIndexed" call. Aside from vertex offset/count, it also expects an offset into the index buffer to start rendering from, and number of indices to render from. In this case the vertex offset will be added to every read index, allowing you to re-use the index buffer for potentially different geometry. 
 
And this wraps up the rendering pipeline. After this step your object should be rendered to your render target and ready to display. 
 
# Compute {#renderAPI_g}
The compute pipeline is a very simple pipeline that can be used for general purpose calculations. It is separate from the graphics pipeline we have been describing so far, but uses the same functionality, just in a more limited way. You don't have to set fixed states, render targets, vertex/index buffers and only one GPU program type is supported (compute GPU program).

The pipeline is represented with the @ref bs::ct::ComputePipelineState "ct::ComputePipelineState" object, which must be initialized with the compute GPU program to use.

After creation use @ref bs::ct::RenderAPI::setComputePipeline "ct::RenderAPI::setComputePipeline" to bind the pipeline for further operations. When the pipeline is set up you can execute it by calling @ref bs::ct::RenderAPI::dispatchCompute "ct::RenderAPI::dispatchCompute". You should provide it a three dimensional number that determines how many instances of the currently bound GPU program to execute. The total number of executions will be X * Y * Z.

Since compute pipeline doesn't support render targets, you will want to use load-store textures for output. An example of a simple compute pipeline:
~~~~~~~~~~~~~{.cpp}
SPtr<GpuProgram> computeProgram = ...;

SPtr<ComputePipelineState> state = ComputePipelineState::create(computeProgram);
SPtr<GpuParams> computeGpuParams = GpuParams::create(state);

... optionally set some parameters ...

RenderAPI& rapi = RenderAPI::instance();
rapi.setComputePipeline(state);
rapi.setGpuParams(computeGpuParams);
rapi.dispatchCompute(512, 512);

... read from our texture to get the result ...
~~~~~~~~~~~~~

We won't go any deeper about the details of the compute pipeline as this information can be found by learning about the GPU pipeline in general from other sources.

# API specifics {#renderAPI_h}
@ref bs::RenderAPI "RenderAPI" can be internally implemented by a variety of actual rendering API's like DirectX or OpenGL. Most of the functionality is shared, but there are always some differences between them to be noted (for example DirectX uses a depth range of [0, 1] while OpenGL uses [-1, 1]). Often those differences can be important for various rendering algorithms.

Use @ref bs::RenderAPI::getAPIInfo "RenderAPI::getAPIInfo" to receive the @ref bs::RenderAPIInfo "RenderAPIInfo" containing such information, so you may modify your rendering accordingly. 

For convenience a specialized @ref bs::RenderAPI::convertProjectionMatrix "RenderAPI::convertProjectionMatrix" method is also provided, which converts a generic engine projection matrix, into a render API specific one.

# Command buffers {#renderAPI_i}
Almost all @ref bs::ct::RenderAPI "ct::RenderAPI" commands we talked about so far support @ref bs::ct::CommandBuffer "ct::CommandBuffer"s. Command buffers are optional, but they allow the rendering commands to be generated from threads other than the core thread.

To create a command buffer call @ref bs::ct::CommandBuffer::create "ct::CommandBuffer::create" after which provide it to the relevant @ref bs::ct::RenderAPI "ct::RenderAPI" calls. Those commands will get recorded in the command buffer, but not executed. To actually execute the commands call @ref bs::ct::RenderAPI::submitCommandBuffer "ct::RenderAPI::submitCommandBuffer".

This allows rendering to be faster since work can be distributed over multiple CPU cores. Note that only command queuing can happen on a separate thread, command buffer creation and execution must still happen on the core thread.

Command buffer example:
~~~~~~~~~~~~~{.cpp}
// Core thread
SPtr<CommandBuffer> cmdBuffer = CommandBuffer::create(CBT_COMPUTE);
SPtr<GpuProgram> computeProgram = ...;
SPtr<GpuParams> computeGpuParams = ...;
SPtr<ComputePipelineState> state = ComputePipelineState::create(computeProgram);

... queue up worker thread(s) ...

// Worker thread
RenderAPI& rapi = RenderAPI::instance();
rapi.setComputePipeline(state, cmdBuffer);
rapi.setGpuParams(computeGpuParams, cmdBuffer);
rapi.dispatchCompute(512, 512, cmdBuffer);

// Core thread
rapi.submitCommandBuffer(cmdBuffer);
~~~~~~~~~~~~~