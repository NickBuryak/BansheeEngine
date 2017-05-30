#include "$ENGINE$\ReflectionCubemapCommon.bslinc"
#include "$ENGINE$\SHCommon.bslinc"

technique IrradianceReduceSH
{
	mixin ReflectionCubemapCommon;
	mixin SHCommon;

	code
	{
		#define PI 3.1415926
	
		struct SHCoeffsAndWeight
		{
			SHVector5RGB coeffs;
			float weight;
		};

		StructuredBuffer<SHCoeffsAndWeight> gInput;
		RWStructuredBuffer<SHVector5RGB> gOutput;
		
		[internal]
		cbuffer Params
		{
			uint gNumEntries;
		}			
		
		[numthreads(1, 1, 1)]
		void csmain(
			uint groupIdx : SV_GroupIndex,
			uint groupId : SV_GroupID,
			uint3 dispatchThreadId : SV_DispatchThreadID)
		{
			SHVector5RGB coeffs;
			float weight = 0;
			
			SHZero(coeffs.R);
			SHZero(coeffs.G);
			SHZero(coeffs.B);
			
			// Note: There shouldn't be many entries, so we add them all in one thread. Otherwise we should do parallel reduction.
			for(uint i = 0; i < gNumEntries; i++)
			{
				SHCoeffsAndWeight current = gInput[i];
			
				SHAdd(coeffs.R, current.coeffs.R);
				SHAdd(coeffs.G, current.coeffs.G);
				SHAdd(coeffs.B, current.coeffs.B);

				weight += current.weight;
			}
			
			// Normalize
			float normFactor = (4 * PI) / weight;
			SHMultiply(coeffs.R, normFactor);
			SHMultiply(coeffs.G, normFactor);
			SHMultiply(coeffs.B, normFactor);
				
			gOutput[0] = coeffs;
		}
	};
};