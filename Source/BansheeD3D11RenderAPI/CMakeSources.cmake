set(BS_BANSHEED3D11RENDERAPI_INC_NOFILTER
	"BsD3D11VideoModeInfo.h"
	"BsD3D11VertexBuffer.h"
	"BsD3D11TimerQuery.h"
	"BsD3D11TextureView.h"
	"BsD3D11TextureManager.h"
	"BsD3D11Texture.h"
	"BsD3D11SamplerState.h"
	"BsD3D11RenderWindowManager.h"
	"BsD3D11RenderWindow.h"
	"BsD3D11RenderUtility.h"
	"BsD3D11RenderTexture.h"
	"BsD3D11RenderStateManager.h"
	"BsD3D11RasterizerState.h"
	"BsD3D11QueryManager.h"
	"BsD3D11Prerequisites.h"
	"BsD3D11OcclusionQuery.h"
	"BsD3D11Mappings.h"
	"BsD3D11InputLayoutManager.h"
	"BsD3D11IndexBuffer.h"
	"BsD3D11HLSLProgramFactory.h"
	"BsD3D11HLSLParamParser.h"
	"BsD3D11HardwareBufferManager.h"
	"BsD3D11HardwareBuffer.h"
	"BsD3D11GpuProgram.h"
	"BsD3D11GpuParamBlockBuffer.h"
	"BsD3D11GpuBufferView.h"
	"BsD3D11GpuBuffer.h"
	"BsD3D11EventQuery.h"
	"BsD3D11DriverList.h"
	"BsD3D11Driver.h"
	"BsD3D11Device.h"
	"BsD3D11DepthStencilState.h"
	"BsD3D11BlendState.h"
	"BsD3D11RenderAPI.h"
	"BsD3D11RenderAPIFactory.h"
	"BsD3D11CommandBuffer.h"
	"BsD3D11CommandBufferManager.h"
)

set(BS_BANSHEED3D11RENDERAPI_SRC_NOFILTER
	"BsD3D11VideoModeInfo.cpp"
	"BsD3D11VertexBuffer.cpp"
	"BsD3D11TimerQuery.cpp"
	"BsD3D11TextureView.cpp"
	"BsD3D11TextureManager.cpp"
	"BsD3D11Texture.cpp"
	"BsD3D11SamplerState.cpp"
	"BsD3D11RenderWindowManager.cpp"
	"BsD3D11RenderWindow.cpp"
	"BsD3D11RenderUtility.cpp"
	"BsD3D11RenderTexture.cpp"
	"BsD3D11RenderStateManager.cpp"
	"BsD3D11RasterizerState.cpp"
	"BsD3D11QueryManager.cpp"
	"BsD3D11Plugin.cpp"
	"BsD3D11OcclusionQuery.cpp"
	"BsD3D11Mappings.cpp"
	"BsD3D11InputLayoutManager.cpp"
	"BsD3D11IndexBuffer.cpp"
	"BsD3D11HLSLProgramFactory.cpp"
	"BsD3D11HLSLParamParser.cpp"
	"BsD3D11HardwareBufferManager.cpp"
	"BsD3D11HardwareBuffer.cpp"
	"BsD3D11GpuProgram.cpp"
	"BsD3D11GpuParamBlockBuffer.cpp"
	"BsD3D11GpuBufferView.cpp"
	"BsD3D11GpuBuffer.cpp"
	"BsD3D11EventQuery.cpp"
	"BsD3D11DriverList.cpp"
	"BsD3D11Driver.cpp"
	"BsD3D11Device.cpp"
	"BsD3D11DepthStencilState.cpp"
	"BsD3D11BlendState.cpp"
	"BsD3D11RenderAPI.cpp"
	"BsD3D11RenderAPIFactory.cpp"
	"BsD3D11CommandBuffer.cpp"
	"BsD3D11CommandBufferManager.cpp"
)

source_group("Header Files" FILES ${BS_BANSHEED3D11RENDERAPI_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEED3D11RENDERAPI_SRC_NOFILTER})

set(BS_BANSHEED3D11RENDERAPI_SRC
	${BS_BANSHEED3D11RENDERAPI_INC_NOFILTER}
	${BS_BANSHEED3D11RENDERAPI_SRC_NOFILTER}
)