Parameters =
{
	mat4x4 	gWorldTransform;
	float	gInvViewportWidth;
	float	gInvViewportHeight;
	float	gViewportYFlip;
	color	gTint;
	
	Sampler2D	gMainTexSamp : alias("gMainTexture");
	Texture2D	gMainTexture;
};

Blocks = 
{
	Block GUIParams : auto("GUIParams");
};

Technique =
{
	Pass =
	{
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
			WriteMask = RGB;
		};	
	
		DepthRead = false;
		DepthWrite = false;
		
		Common =
		{
			cbuffer GUIParams
			{
				float4x4 gWorldTransform;
				float gInvViewportWidth;
				float gInvViewportHeight;
				float gViewportYFlip;
				float4 gTint;
			}	
		};
		
		Vertex =
		{
			void main(
				in float3 inPos : POSITION,
				in float2 uv : TEXCOORD0,
				out float4 oPosition : SV_Position,
				out float2 oUv : TEXCOORD0)
			{
				float4 tfrmdPos = mul(gWorldTransform, float4(inPos.xy, 0, 1));

				float tfrmdX = -1.0f + (tfrmdPos.x * gInvViewportWidth);
				float tfrmdY = (1.0f - (tfrmdPos.y * gInvViewportHeight)) * gViewportYFlip;

				oPosition = float4(tfrmdX, tfrmdY, 0, 1);
				oUv = uv;
			}
		};
		
		Fragment =
		{
			SamplerState gMainTexSamp : register(s0);
			Texture2D gMainTexture : register(t0);

			float4 main(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target
			{
				float4 color = float4(gTint.rgb, gMainTexture.Sample(gMainTexSamp, uv).r * gTint.a);
				return color;
			}
		};
	};
};