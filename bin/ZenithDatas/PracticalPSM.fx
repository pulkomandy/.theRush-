//-----------------------------------------------------------------------------
#define SHADOW_R32F_BILINEAR


texture diffuseTexture
<
string ResourceType = "2D";
>;

//------------------------------------
sampler diffuseSampler = sampler_state 
{
    texture = <diffuseTexture>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};



#ifdef SHADOW_NONE
	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize)
	{
		return 1;
	}
#endif

#ifdef SHADOW_R32F

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize)
	{
		float  shadowDepth0 = tex2D(ShadowMapSampler, (texCoord.xy) / texCoord.w).r;
		shadowTestDepth = texCoord.z / texCoord.w;
		shadowTestDepth -= 0.00505f;
		float  shadow0     = (shadowTestDepth <= shadowDepth0)?1.f:0.f;
		return shadow0;
	}

#endif

#ifdef SHADOW_R32F_BILINEAR

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize)
	{
		float dt = 1.0f/texSize;
		float2 tc = ceil(texCoord.xy*texSize) * dt ;
		float2 lineari = saturate((texCoord.xy*texSize) - int2(texCoord.xy*texSize));
	
		float2 shadowTexCoord = texCoord.xy / texCoord.w;
		shadowTestDepth = texCoord.z / texCoord.w;
	
		
		float  shadowDepth0 = tex2D(ShadowMapSampler, (texCoord.xy+float2(0,0)) / texCoord.w).r;
		float  shadowDepth1 = tex2D(ShadowMapSampler, (texCoord.xy+float2(dt,0)) / texCoord.w).r;
		float  shadowDepth2 = tex2D(ShadowMapSampler, (texCoord.xy+float2(0,dt)) / texCoord.w).r;
		float  shadowDepth3 = tex2D(ShadowMapSampler, (texCoord.xy+float2(dt,dt)) / texCoord.w).r;

		shadowTestDepth -= 0.00505f;
		float  shadow0     = (shadowTestDepth <= shadowDepth0)?1.f:0.f;
		float  shadow1     = (shadowTestDepth <= shadowDepth1)?1.f:0.f;
		float  shadow2     = (shadowTestDepth <= shadowDepth2)?1.f:0.f;
		float  shadow3     = (shadowTestDepth <= shadowDepth3)?1.f:0.f;

  		float dt1 = lerp(shadow0, shadow1, lineari.x);
  		float dt2 = lerp(shadow2, shadow3, lineari.x);
 
  		float shadow = lerp(dt1, dt2, lineari.y);

		return shadow;
	}

#endif


texture ShadowMap;
texture SpotLight;
texture ShadowColor;

//-----------------------------------------------------------------------------

float4x4 WorldViewProj : worldviewprojection;
float4x4 ViewProj : ViewProjection;
float4x4 TexTransform : TextureTransform;

//-----------------------------------------------------------------------------


struct VS_INPUT 
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
	float2 TexCoord0 : TEXCOORD0;
};

struct VS_INPUT_HWI
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
	float2 TexCoord0 : TEXCOORD0;
	
	float4 matX : TEXCOORD1;
	float4 matY : TEXCOORD2;
	float4 matZ : TEXCOORD3;
	
};

struct VS_OUTPUT {
    float4 Position  : POSITION;
    float4 TexCoord0 : TEXCOORD0;
    float3 Color0 : COLOR0;
    float  LightSpaceDepth     : TEXCOORD1;
	float2 TexCoord1 : TEXCOORD2;
};

//-----------------------------------------------------------------------------

sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;

    MinFilter = Point;//Linear;  
    MagFilter = Point;//Linear;
    MipFilter = None;
    AddressU  = Clamp;
    AddressV  = Clamp;

};

//----------------------------------------------------------------------------

VS_OUTPUT QuadVS(VS_INPUT IN)
{
   VS_OUTPUT OUT;
   OUT = (VS_OUTPUT)0;
   float2 Position = (IN.Position.xz / 1600.0f) + 0.5f;
   OUT.Position = float4(Position*0.45 + 0.5, 1, 1);
   OUT.TexCoord0 = float4(Position.x, 1.0-Position.y, 0, 0);
   return OUT;
}

VS_OUTPUT ShadowMapVS(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = mul(IN.Position, WorldViewProj);
    OUT.TexCoord0 = float4(OUT.Position.zzz, OUT.Position.w);
	OUT.TexCoord1 = IN.TexCoord0;
	
    return OUT;
}

VS_OUTPUT ShadowMapVS_HWI(VS_INPUT_HWI IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

	float4x4 matWorld;
	matWorld[0] = IN.matX;
	matWorld[1] = IN.matY;
	matWorld[2] = IN.matZ;
	matWorld[3] = float4(0,0,0,1);
 	float4 wpos = mul(IN.Position, transpose(matWorld));
	OUT.Position = mul (wpos, ViewProj);
	
    OUT.TexCoord0 = float4(OUT.Position.zzz, OUT.Position.w);
	OUT.TexCoord1 = IN.TexCoord0;
	
    return OUT;
}



VS_OUTPUT ShadowMapVSDF24(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = mul(IN.Position, WorldViewProj);
    
    return OUT;
}


//-----------------------------------------------------------------------------

VS_OUTPUT MainVS(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
  
    OUT.Color0 = 1;

    OUT.TexCoord0 = mul(IN.Position, TexTransform);
    
    OUT.Position = mul(IN.Position, WorldViewProj);
 
 
    return OUT;
}

//-----------------------------------------------------------------------------

VS_OUTPUT MainVSDF4(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
  
    OUT.Color0 = 1;

    OUT.TexCoord0 = mul(IN.Position, TexTransform);
    
    OUT.Position = mul(IN.Position, WorldViewProj);
 
    // Calculate depth in light space and scale to range
    OUT.LightSpaceDepth = mul( IN.Position, TexTransform  ).z; 
    return OUT;
}
//-----------------------------------------------------------------------------

float4 DrawShadowPS(VS_OUTPUT IN) : COLOR
{
    return float4(tex2D(ShadowMapSampler, IN.TexCoord0.xy).xyz, 1.0);
}

float4 BlackPS(VS_OUTPUT IN) : COLOR
{
    return float4(0,0,0,1);
}

float4 OutputDepthPS(VS_OUTPUT IN) : COLOR
{
	
    float depth = IN.TexCoord0.b;// / IN.TexCoord0.a;
	float diffuseAlpha = tex2D(diffuseSampler, IN.TexCoord1.xy).w;
	clip(diffuseAlpha-0.5);
    return float4(depth.xxx, 1);
}

//-----------------------------------------------------------------------------

float4 MainPS_HW(VS_OUTPUT IN) : COLOR 
{

    float3 shadow    = tex2D(ShadowMapSampler, IN.TexCoord0.xy).xyz;
    float3 color = ((shadow*0.2+0.8) * IN.Color0)*0.9 + 0.1;
  
    return float4(shadow, 1.0);
}

float4 MainPS_HWDF4(VS_OUTPUT IN) : COLOR 
{
	float4   shadowMapWeights;

	float4 shadowMapVals = tex2D(ShadowMapSampler, IN.TexCoord0.xy);
	
	float dist = IN.LightSpaceDepth - 0.015;
	
	float4 pFrac = frac(IN.TexCoord0 * float4( 1024.0, 1024.0, 0.0, 0.0 ));

	shadowMapWeights = float4( 1, 1-pFrac.x, 1, 1-pFrac.x );

	float4 inLight = ( dist < shadowMapVals );
    float percentInLight = dot(inLight, shadowMapWeights);
    	
    percentInLight *= 0.5f;

	return percentInLight;
}


float4 MainPS_R32F(VS_OUTPUT IN) : COLOR
{

  	float shadow = GetShadow( ShadowMapSampler, IN.LightSpaceDepth, IN.TexCoord0, 1024.0f);


	//shadow *= 0.20f;
	float3 color = ((shadow*0.4+0.6));//*0.9 + 0.1;
	return (color.x);//, 1.0);
}

//-----------------------------------------------------------------------------

technique UseHardwareShadowMap // 0
{
    pass P0
    {
        VertexShader = compile vs_1_1 MainVS();
        PixelShader = compile ps_2_0 MainPS_HW();
    }
}

technique UseHardwareShadowMapDF4 // 1
{
    pass P0
    {
        VertexShader = compile vs_1_1 MainVSDF4();
        PixelShader = compile ps_2_0 MainPS_HWDF4();
    }
}
technique UseR32FShadowMap // 2
{
    pass P0
    {
        VertexShader = compile vs_1_1 MainVS();
        PixelShader = compile ps_2_0 MainPS_R32F();
    }
}

technique GenHardwareShadowMap20 // 3
{
    pass P0
    {
#ifdef CM    
		BlendEnable = false;
		DepthMask = true;  //z write
#endif
        VertexShader = compile vs_1_1 ShadowMapVS();
        PixelShader = compile ps_2_0 OutputDepthPS();
    }
}

technique HWI_GenHardwareShadowMap20 // 3
{
    pass P0
    {
#ifdef CM    
		BlendEnable = false;
		DepthMask = true;  //z write
#endif
        VertexShader = compile vs_2_0 ShadowMapVS_HWI();
        PixelShader = compile ps_2_0 OutputDepthPS();
    }
}
technique GenHardwareShadowMapDF24 // 4
{
    pass P0
    {
#ifdef CM       
		BlendEnable = false;
		DepthMask = true;  //z write
#endif
        VertexShader = compile vs_1_1 ShadowMapVSDF24();
    }
}

technique GenHardwareShadowMap11 // 5
{
    pass P0
    {
#ifdef CM       
		BlendEnable = false;
		DepthMask = true;  //z write
#endif
        VertexShader = compile vs_1_1 ShadowMapVS();
        PixelShader = compile ps_1_1 BlackPS();
    }
}


technique DrawHardwareShadowMap // 6
{
    Pass P0
    {
        VertexShader = compile vs_1_1 QuadVS();
        PixelShader = compile ps_2_0 DrawShadowPS();
    }
}

//-----------------------------------------------------------------------------

