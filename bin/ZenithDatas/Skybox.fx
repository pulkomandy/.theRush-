string description = "Basic Vertex Lighting with a Texture";

//------------------------------------
float4x4 worldViewProj : WorldViewProjection;
float4x4 world   : World;
float4x4 worldInverseTranspose : WorldInverseTranspose;
float4x4 viewInverse : ViewInverse;
float4x4 ViewProj : ViewProjection;

textureCUBE diffuseTexture : Diffuse
<
	string ResourceName = "default_color.dds";
	string ResourceType = "cube";
>;

float4 lightDir : Direction
<
	string Object = "DirectionalLight";
    string Space = "World";
> = {1.0f, -1.0f, 1.0f, 0.0f};

float4 lightColor : Diffuse
<
    string UIName = "Diffuse Light Color";
    string Object = "DirectionalLight";
> = {1.0f, 1.0f, 1.0f, 1.0f};

float4 lightAmbient : Ambient
<
    string UIWidget = "Ambient Light Color";
    string Space = "material";
> = {0.0f, 0.0f, 0.0f, 1.0f};

float4 materialDiffuse : Diffuse
<
    string UIWidget = "Surface Color";
    string Space = "material";
> = {1.0f, 1.0f, 1.0f, 1.0f};

float4 materialSpecular : Specular
<
	string UIWidget = "Surface Specular";
	string Space = "material";
> = {1.0f, 1.0f, 1.0f, 1.0f};

float shininess : SpecularPower
<
    string UIWidget = "slider";
    float UIMin = 1.0;
    float UIMax = 128.0;
    float UIStep = 1.0;
    string UIName = "specular power";
> = 30.0;


//------------------------------------
struct vertexInput {
    float3 position				: POSITION;

};

struct vertexOutput {
    float4 hPosition		: POSITION;
    float4 texCoordDiffuse	: TEXCOORD0;
};


//------------------------------------
vertexOutput VS_TransformAndTexture(vertexInput IN) 
{
    vertexOutput OUT = (vertexOutput)0;
    OUT.hPosition = mul( float4(IN.position.xyz*8000 , 0.0) , ViewProj);
    OUT.texCoordDiffuse = float4(normalize(IN.position.xyz).xyz, 0);
	OUT.hPosition.z = OUT.hPosition.w;
    return OUT;
}


//------------------------------------
sampler TextureSampler = sampler_state 
{
    texture = <diffuseTexture>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

//-----------------------------------
float4 PS_Textured( vertexOutput IN): COLOR
{
  float4 diffuseTexture = texCUBE( TextureSampler, IN.texCoordDiffuse.xyz );
  return diffuseTexture;
}


//-----------------------------------
technique textured
{
    pass p0 
    {		
		VertexShader = compile vs_2_0 VS_TransformAndTexture();
		PixelShader  = compile ps_2_0 PS_Textured();
    }
}