#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptRenderTarget.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Image/BsPixelData.h"

namespace bs
{
	class RenderTexture;
	class RenderTextureEx;

	class BS_SCR_BE_EXPORT ScriptRenderTexture : public ScriptObject<ScriptRenderTexture, ScriptRenderTargetBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "RenderTexture")

		ScriptRenderTexture(MonoObject* managedInstance, const SPtr<RenderTexture>& value);

		SPtr<RenderTexture> getInternal() const;
		static MonoObject* create(const SPtr<RenderTexture>& value);

	private:
		static void Internal_create(MonoObject* managedInstance, PixelFormat format, int32_t width, int32_t height, int32_t numSamples, bool gammaCorrection, bool createDepth, PixelFormat depthStencilFormat);
		static void Internal_create0(MonoObject* managedInstance, MonoObject* colorSurface);
		static void Internal_create1(MonoObject* managedInstance, MonoObject* colorSurface, MonoObject* depthStencilSurface);
		static void Internal_create2(MonoObject* managedInstance, MonoArray* colorSurface);
		static void Internal_create3(MonoObject* managedInstance, MonoArray* colorSurface, MonoObject* depthStencilSurface);
		static MonoObject* Internal_getColorSurface(ScriptRenderTexture* thisPtr);
		static MonoArray* Internal_getColorSurfaces(ScriptRenderTexture* thisPtr);
		static MonoObject* Internal_getDepthStencilSurface(ScriptRenderTexture* thisPtr);
	};
}
