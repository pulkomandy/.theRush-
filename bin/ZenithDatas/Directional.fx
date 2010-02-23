//------------------------------------
#ifdef CM
#include "./ZenithDatas/ShadowTechnics.fx"
#include "./ZenithDatas/std.fx"


#else

string ParamID = "0x01";

#endif



texture diffuse
<
	string ResourceName = "default_color.dds";
	string ResourceType = "2D";
>;

sampler diffuseSampler = sampler_state 
{
texture = <diffuse>;
    AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4x4 worldViewProj : WorldViewProjection;

//------------------------------------
struct vertexInput {
    float3 position				: POSITION;
    float3 normal				: NORMAL;
    float4 texCoordDiffuse		: TEXCOORD0;
};
struct vertexInputUnlit {
    float3 position				: POSITION;
    //float4 texCoordDiffuse		: TEXCOORD0;
};
struct vertexOutput 
{
    float4 hPosition		: POSITION;
    float4 texCoordDiffuse		: TEXCOORD0;
    
};
struct vertexOutputGridded
{
    float4 hPosition		: POSITION;
    float3 pos : TEXCOORD0;
    float4 color : COLOR;
    
};

//------------------------------------
vertexOutput VS_Debug(vertexInput IN) 
{
    vertexOutput OUT = (vertexOutput)0; 
    float4 transfPos = mul( float4(IN.position.xyz , 1.0) , worldViewProj);
    OUT.hPosition = transfPos;
	OUT.texCoordDiffuse = IN.texCoordDiffuse;
    
    return OUT;
}

float4 PS_Debug( vertexOutput IN) : COLOR
{
	return tex2D(diffuseSampler, IN.texCoordDiffuse.xy);//float4(1,0,1,1);
}

//-----------------------------------
technique Debug
{
    pass p0 
    {	
	
		VertexShader = compile vs_2_0 VS_Debug();
		PixelShader  = compile ps_2_0 PS_Debug();
    }
}


//------------------------------------
vertexOutputGridded VS_Debug2(vertexInput IN) 
{
    vertexOutputGridded OUT = (vertexOutputGridded )0; 
    float4 transfPos = mul( float4(IN.position.xyz , 1.0) , worldViewProj);
    OUT.hPosition = transfPos;
    OUT.color = dot( normalize(IN.normal), float3(1,1,0));
    OUT.pos = IN.position;
    return OUT;
}

float4 PS_Debug2( vertexOutputGridded IN) : COLOR
{
	float3 frc1 = (frac(IN.pos)<0.1)?1:0;
	float3 frc2 = (frac(IN.pos*10)<0.1)?1:0;
	float frc1a = frc1.x+frc1.y+frc1.z;
	float frc2a = frc2.x+frc2.y+frc2.z;
	return IN.color + frc1a* float4(0.2,1,0.2, 1)+frc2a*float4(0, 0.5, 0,1);
	/*
		return float4(1,1,0,1);
	return IN.color;//float4(1,0,1,1);
	*/
}


technique DebugWire
{
    pass p0 
    {		
#ifdef CM	
		FillMode = 1; // 0=  fill 1 = wire
#endif
		
		VertexShader = compile vs_2_0 VS_Debug();
		PixelShader  = compile ps_2_0 PS_Debug();
    }
}

technique DebugHardCodedLighting
{
    pass p0 
    {	
	
		VertexShader = compile vs_2_0 VS_Debug2();
		PixelShader  = compile ps_2_0 PS_Debug2();
    }
}
