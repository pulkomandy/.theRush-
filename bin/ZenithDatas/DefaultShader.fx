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
};
struct vertexInputUnlit {
    float3 position				: POSITION;
    float4 texCoordDiffuse		: TEXCOORD0;
};
struct vertexOutput {
    float4 hPosition		: POSITION;
    float4 texCoordDiffuse		: TEXCOORD0;
    float4 col : TEXCOORD1;
    float4 TexShadow : TEXCOORD2;
    float4 TexCoordZ : TEXCOORD3;
    float3 Normal : TEXCOORD4;

	

};


//------------------------------------
vertexOutput VS_TransformAndTexture(vertexInput IN) 
{
    vertexOutput OUT; 
    float4 transfPos = mul( float4(IN.position.xyz , 1.0) , worldViewProj);
    OUT.hPosition = transfPos;
    OUT.texCoordDiffuse = IN.texCoordDiffuse;
	float3 wnrm = normalize(mul(IN.normal.xyz, (float3x3)objWorld));
    OUT.col = sunColor*(saturate(dot(wnrm , sunVector )));

    OUT.Normal = normalize(mul(IN.normal.xyz, (float3x3)worldView))*0.5f +0.5f;



float4 tmpshad= mul( float4(IN.position.xyz , 1.0) , objWorld);

    OUT.TexShadow = mul(tmpshad, shadowMat);//TexTransform);

    OUT.TexCoordZ = transfPos;

    return OUT;
}


vertexOutput VS_TransformAndTextureUnlit(vertexInputUnlit IN) 
{
    vertexOutput OUT = (vertexOutput)0; 
    float4 transfPos = mul( float4(IN.position.xyz , 1.0) , worldViewProj);
    OUT.hPosition = transfPos;
    OUT.texCoordDiffuse = IN.texCoordDiffuse;
    
    OUT.TexCoordZ = transfPos;

    return OUT;
}
//-----------------------------------



PS_OUTPUT PS_TexturedLit( vertexOutput IN)
{
	PS_OUTPUT colorResult;
	float4 diffuseColor = tex2D(TextureSampler, IN.texCoordDiffuse.xy);

	
	
	
float shadowcomponent = GetShadow( ShadowMapSampler, 0, IN.TexShadow, 1024.0f, float2(0,0));//vPos);

	float4 lightColor = sunAmbient + min(shadowcomponent.xxxx, IN.col);
  	colorResult.RGBAColor =  float4(diffuseColor.xyz  * lightColor.xyz, diffuseColor.w);


	float depth = ((IN.TexCoordZ.z /* / IN.TexCoordZ.w*/ ));//*0.0012f);///100.0f;
	colorResult.Z = depth;
	colorResult.Normal = float4(IN.Normal,1);
	colorResult.Albedo = diffuseColor;
	clip(colorResult.RGBAColor.a-0.5);
  	return colorResult;
}

PS_OUTPUT PS_TexturedUnLit( vertexOutput IN)
{
	PS_OUTPUT colorResult;
    colorResult.RGBAColor = tex2D(TextureSampler, IN.texCoordDiffuse.xy);
	float depth = IN.TexCoordZ.z;//*0.1f;//(IN.TexCoordZ.z / IN.TexCoordZ.w);///100.0f;
	colorResult.Z = depth;
	colorResult.Normal = float4(IN.Normal,1);
	colorResult.Albedo = colorResult.RGBAColor;//diffuseColor;
	clip(colorResult.RGBAColor.a-0.5);

    return colorResult;
}

PS_OUTPUT PS_TexturedUnLitForHUD( vertexOutput IN)
{
	PS_OUTPUT colorResult;
    colorResult.RGBAColor = tex2D(TextureSampler, IN.texCoordDiffuse.xy);
	float depth = IN.TexCoordZ.z;//*0.1f;//(IN.TexCoordZ.z / IN.TexCoordZ.w);///100.0f;
	colorResult.Z = depth;
	colorResult.Normal = float4(IN.Normal,1);
	colorResult.Albedo = colorResult.RGBAColor;//diffuseColor;
	//clip(colorResult.RGBAColor.a-0.5);

    return colorResult;
}
//-----------------------------------
technique texturedLit
{
    pass p0 
    {	
#ifdef CM	
		//CullMode = int(1);// CW
		AlphaTestEnable = true;
#endif
	
		VertexShader = compile vs_2_0 VS_TransformAndTexture();
		PixelShader  = compile ps_2_0 PS_TexturedLit();
    }
}


technique texturedUnLit
{
    pass p0 
    {		
#ifdef CM	
		AlphaTestEnable = true;
		//CullMode = int(1);// CW
#endif
		
		VertexShader = compile vs_2_0 VS_TransformAndTextureUnlit();
		PixelShader  = compile ps_2_0 PS_TexturedUnLit();
    }
}

technique texturedUnLitForHUD
{
    pass p0 
    {		
#ifdef CM	
		AlphaTestEnable = true;
		CullMode = int(0);// CW
#endif
		
		VertexShader = compile vs_2_0 VS_TransformAndTextureUnlit();
		PixelShader  = compile ps_2_0 PS_TexturedUnLitForHUD();
    }
}
