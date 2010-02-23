struct PS_OUTPUT
{
   
    float4 RGBAColor      : COLOR0;  // Pixel color    
    float4 Z		  : COLOR1;  // Pixel Z
    float4 Normal	  : COLOR2;  // Normal in viewspace
    float4 Albedo	  : COLOR3;  // Texture only    
};