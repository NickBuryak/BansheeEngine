#include "$ENGINE$\PPBase.bslinc"

technique PPDownsample
{
	mixin PPBase;

	code
	{
		[internal]
		cbuffer Input
		{
			float2 gInvTexSize;
		}		
	
		SamplerState gInputSamp;
		Texture2D gInputTex;

		float4 fsmain(VStoFS input) : SV_Target0
		{
			float2 UV[4];

			// Blur using a 4x4 kernel. It's assumed current position is right in the middle of a 2x2 kernel (because the output
			// texture should be 1/2 the size of the output texture), and moving by one in each direction will sample areas
			// between a 2x2 kernel as well if bilinear filtering is enabled.
			UV[0] = input.uv0 + gInvTexSize * float2(-1, -1);
			UV[1] = input.uv0 + gInvTexSize * float2( 1, -1);
			UV[2] = input.uv0 + gInvTexSize * float2(-1,  1);
			UV[3] = input.uv0 + gInvTexSize * float2( 1,  1);

			float4 samples[4];

			for(uint i = 0; i < 4; i++)
				samples[i] = gInputTex.Sample(gInputSamp, UV[i]);

			return (samples[0] + samples[1] + samples[2] + samples[3]) * 0.25f;
		}	
	};
};