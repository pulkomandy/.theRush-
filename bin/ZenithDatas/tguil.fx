float4x4 wvp : WorldViewProjection;
float4x4 world   : World;
float4x4 proj   : Projection;
float4x4 vp : ViewProjection;

texture diffuseTexture : Diffuse
<
	string ResourceName = "default_color.dds";
>;

sampler TextureSampler = sampler_state 
{
    texture = <diffuseTexture>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};


//------------------------------------

struct vertexInput 
{
    float3 position				: POSITION;
    float4 color				: COLOR;
    float4 texCoordDiffuse		: TEXCOORD0;
};

struct vertexOutput 
{
    float4 hPosition		: POSITION;
    float4 texCoordDiffuse	: TEXCOORD0;
    float4 diffAmbColor		: COLOR0;
};

float4 col : Color
<
	string UIName = "Color";
> = {0.5, 0.5, 0.5, 0.9};

//------------------------------------
vertexOutput VS_TransformAndTexture(vertexInput IN) 
{
    vertexOutput OUT;
    //float4 pos = mul(float4(IN.position.xy, 0, 0), world);
    float4 pos = 0;
    pos.xy = IN.position.x* float2(world[0][0], world[0][1] );
    pos.xy += IN.position.y* float2(world[1][0], world[1][1] );
    //pos.xy = IN.position.xy;
	
  pos.xy *= float2(1.f/640.0f, 1.0f/480.0f);
    //pos.xy += float2(-1.f,1.0f);
    
    OUT.hPosition = pos;
    OUT.hPosition.zw = float2(0.0f, 1.0f);
    
    
    OUT.texCoordDiffuse = IN.texCoordDiffuse;
	OUT.diffAmbColor = /*IN.color **/ col;

//pos.xy = IN.position.xy;
    pos.xy = IN.position.x* float2(world[0][0], world[0][1] ) ;
    pos.xy += IN.position.y* float2(world[1][0], world[1][1] ) ;
    pos.xy += float2(world[3][0], world[3][1]);
//pos.z = 10;
//OUT.hPosition = mul( float4(pos.xyz, 1.0) , proj);
OUT.hPosition.xy = pos.xy;
OUT.hPosition.xy *=2;
OUT.hPosition.zw=float2(0.5f, 1.0f);
OUT.hPosition.xy += float2(-1.0f,-1.0f);

    return OUT;
}

float4 debugPs(vertexOutput IN): COLOR
{
	return tex2D(TextureSampler , IN.texCoordDiffuse.xy)*IN.diffAmbColor;
}
//-----------------------------------
technique textured
{
    pass p0 
    {		
/*
    BlendEnable = true;
    CullFaceEnable=false;
		BlendEquation = int(2);
CullMode = none;
*/		
		VertexShader = compile vs_2_0 VS_TransformAndTexture();
		pixelShader = compile ps_2_0 debugPs();
    }
}
