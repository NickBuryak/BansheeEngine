#include "$ENGINE$\PPBase.bslinc"
#include "$ENGINE$\ReflectionCubemapCommon.bslinc"

technique ReflectionCubeDownsample
{
	mixin PPBase;
	mixin ReflectionCubemapCommon;

	code
	{
		[internal]
		cbuffer Input
		{
			int gCubeFace;
		}	
	
		SamplerState gInputSamp;
		TextureCube gInputTex;

		float4 fsmain(VStoFS input) : SV_Target0
		{
			float2 scaledUV = input.uv0 * 2.0f - 1.0f;
			float3 dir = getDirFromCubeFace(gCubeFace, scaledUV);
			
			return gInputTex.Sample(gInputSamp, dir);
		}	
	};
};