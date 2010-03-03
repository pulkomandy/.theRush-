//------------------------------------
#ifdef CM
#include "ShadowTechnics.fx"
#include "std.fx"

float4 sunColor : SunColor;
float3 sunVector: SunVector;
float4 sunAmbient: SunAmbient;


#else
struct PS_OUTPUT
{
   
    float4 RGBAColor      : COLOR0;  // Pixel color    
    float4 Z		  : COLOR1;  // Pixel Z
    float4 Normal	  : COLOR2;  // Normal in viewspace
    float4 Albedo	  : COLOR3;  // Texture only    
};

string ParamID = "0x0001";
	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize, float2 vPos)
	{
		return 1;
	}

texture ShadowMap;

sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;

//#ifdef SHADOW_NVQ1
    MinFilter = Linear;  
    MagFilter = Linear;
    MipFilter = Linear;
    /*
#else
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
#endif
*/

    AddressU  = Clamp;
    AddressV  = Clamp;
};


float4 sunColor : SunColor
<

    string UIWidget = "Color";
    string UIName = "SunColor";
> = {1,1,1,1};

float3 sunVector: SunVector= {0.8,0.8,0};
float4 sunAmbient: SunAmbient<

    string UIWidget = "Color";
    string UIName = "SunAmbient";
> = {0.5,0.5,0.5,1};

#endif






float4x4 worldViewProj : WorldViewProjection;
float4x4 objWorld : World;
float4x4 shadowMat : ShadowMatrix;
float4x4 worldView : WorldView;
float4x4 view : View;
float4x4 viewI : ViewInverse;

texture diffuseTexture
<
string ResourceType = "2D";
>;

//------------------------------------
sampler TextureSampler = sampler_state 
{
    texture = <diffuseTexture>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};
//------------------------------------
struct vertexInput {
    float3 position				: POSITION;
    float3 normal				: NORMAL;
    float4 texCoordDiffuse		: TEXCOORD0;
	float4 diffuse				: COLOR0;
};

struct vertexOutput {
    float4 hPosition		: POSITION;
    float4 texCoordDiffuse		: TEXCOORD0;
    float4 col : TEXCOORD1;
   
};


//------------------------------------
vertexOutput VS_TransformAndTexture(vertexInput IN) 
{
    
    vertexOutput OUT = (vertexOutput)0; 
    float4 transfPos = mul( float4(IN.position.xyz , 1.0) , worldViewProj);
    OUT.hPosition = transfPos;
    OUT.texCoordDiffuse = IN.texCoordDiffuse;
    
    OUT.col = IN.diffuse;

    return OUT;
}
//-----------------------------------


float4 PS_TexturedUnLit( vertexOutput IN) : COLOR
{
	return tex2D(TextureSampler, IN.texCoordDiffuse.xy) * IN.col;
}

//-----------------------------------
technique texturedLit
{
    pass p0 
    {	
#ifdef CM	
		DepthMask = false;
		ZEnable = false;
		cullmode = int(0);	
		BlendEnable = true;
		BlendEquation = int(2);		
#endif	

	
		VertexShader = compile vs_2_0 VS_TransformAndTexture();
		PixelShader  = compile ps_2_0 PS_TexturedUnLit();
    }
}
