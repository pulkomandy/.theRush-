//#define SHADOW_NONE 
//#define SHADOW_R32F_BILINEAR
//#define SHADOW_R32F
#define SHADOW_NVQ1 // nvidia HW shadowmap
//#define SHADOW_JITTERPS3 
/*
float4 FilterSize<> = {8.f, 8.f, 8.f, 8.f};
float4 JitterScale<> = {1.f, 1.f, 1.f, 1.f};
*/
//float loopIterations<> = 1;
/*
    float4 FilterSize<> = {0, 0, 0, 8.f / 1024.f};
           float4 JitterScale<> = {1.f / 32.f, 1.f / 32.f, 0, 0};
  */         
           
      
float shadowFilterSz : ShadowFilterSize;

texture ShadowMap;

sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;

    MinFilter = Linear;  
    MagFilter = Linear;
    MipFilter = Linear;

    AddressU  = Clamp;
    AddressV  = Clamp;
	AddressW  = Clamp;
};

/*
texture3D JitterShadow
<
string ResourceType = "3D";
>;


sampler3D JitterSampler = sampler_state
{
    Texture = <JitterShadow>;
    MinFilter = POINT;  
    MagFilter = POINT;
    MipFilter = POINT;
    AddressU  = WRAP;
    AddressV  = WRAP;
    AddressW  = WRAP;
};


*/

#ifdef SHADOW_JITTERPS3

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 ProjectedCoord, float texSize, float2 vPos)
	{

			float4 FilterSize = float4(0, 0, 0, shadowFilterSz);//0.0078125); // 8/1024
           float4 JitterScale = float4(0.03125, 0.03125, 0, 0);     // 1/32	


			float fsize = ProjectedCoord.w * FilterSize.w;
			float4 smcoord = {0, 0, ProjectedCoord.zw};
			float4 jcoord = float4(vPos.xy * JitterScale.xy , 0, 0);
			float shadow = 0;//, shadowsample;
			float4 jitter;

			
			// Perform 8 test samples
			jitter = (2.0 * tex3D(JitterSampler, jcoord) - 1.0);

			jcoord.z += 0.03125f;
			smcoord.xy = jitter.xy * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;



			smcoord.xy = jitter.zw * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			

			jitter = (2 * tex3D(JitterSampler, jcoord) - 1.0);
			jcoord.z += 0.03125f;
			smcoord.xy = jitter.xy * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			smcoord.xy = jitter.zw * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			
			jitter = (2 * tex3D(JitterSampler, jcoord) - 1.0);
			jcoord.z += 0.03125f;
			smcoord.xy = jitter.xy * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			smcoord.xy = jitter.zw * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			
			jitter = (2 * tex3D(JitterSampler, jcoord) - 1.0);
			jcoord.z += 0.03125f;
			smcoord.xy = jitter.xy * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			smcoord.xy = jitter.zw * fsize + ProjectedCoord.xy;
			shadow += tex2Dproj(ShadowMapSampler, smcoord).r;
			

			if( (shadow > 0) && (shadow < 8) ) 
			{	
	
				float loopIterations = 2;
				for(int i = 0; i < loopIterations; i++) 
				{
				
					jitter = (2 * tex3D(JitterSampler, jcoord) - 1.0);

					jcoord.z += 0.03125f;
					smcoord.xy = jitter.xy * fsize + ProjectedCoord.xy;

					shadow += tex2Dproj(ShadowMapSampler, smcoord).r;

					smcoord.xy = jitter.zw * fsize + ProjectedCoord.xy;
					shadow += tex2Dproj(ShadowMapSampler, smcoord).r;

				}	
		
  				shadow /= (2 * (loopIterations + 4));

			} 
			else 
			
			{
	  			shadow /= 8;
			}

		return shadow;

	}

#endif


#ifdef SHADOW_NONE
	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize, float2 vPos)
	{
		return 1;
	}
#endif

#ifdef SHADOW_R32F

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize, float2 vPos)
	{
		float  shadowDepth0 = tex2D(ShadowMapSampler, (texCoord.xy) / texCoord.w).r;
		float shadowTestDepth2 = texCoord.z / texCoord.w;
		shadowTestDepth2 -= 0.00505f;
		float  shadow0     = (shadowTestDepth2 <= shadowDepth0)?1.f:0.f;
		return shadow0;
	}

#endif

#ifdef SHADOW_NVQ1

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize, float2 vPos)
	{
		static const float4 sz = float4(1.f/1024.f, 1.f/1024.f, 0, 0) * 1;
		static const float weights[8] = {0.2, 0.5, 0.2,
		0.5, 0.5,
		0.2, 0.5, 0.2};
		static const float4 tcs[8] = { sz*float4(-1,-1,0,0), sz*float4(0,-1,0,0), sz*float4(1,-1,0,0),
		sz*float4(-1, 0,0,0), sz*float4(1, 0,0,0),
		sz*float4(-1, 1,0,0), sz*float4(0, 1,0,0), sz*float4(1, 1,0,0) };
			
		float shadow = 0;
		shadow = tex2Dproj(ShadowMapSampler, texCoord).r;
		shadow += tex2Dproj(ShadowMapSampler, texCoord+tcs[1]).r*weights[1];
		shadow += tex2Dproj(ShadowMapSampler, texCoord+tcs[3]).r*weights[3];
		shadow += tex2Dproj(ShadowMapSampler, texCoord+tcs[4]).r*weights[4];


		shadow += tex2Dproj(ShadowMapSampler, texCoord+tcs[6]).r*weights[6];
		/*
		shadow += tex2Dproj(ShadowMapSampler, texCoord+tcs[4]).r*weights[4];
/*
		shadow += tex2Dproj(ShadowMapSampler, texCoord+tcs[5]).r*weights[5];
		/*
		shadow += tex2Dproj(ShadowMapSampler, texCoord+sz*float4( 0, 1,0,0)).r*weights[7];
		shadow += tex2Dproj(ShadowMapSampler, texCoord+sz*float4( 1, 1,0,0)).r*weights[8];
*/
		shadow *= (1.f/(3.f));
		/*
	

		float  shadow = 0;
		int idx = 0;
		for (int y=-1;y<2;y++)
		{
			for (int x=-1;x<2;x++)
			{
				shadow += tex2Dproj(ShadowMapSampler, texCoord+(sz*float4(x,y,0,0))).r * weights[idx];
				++idx;
			}
		}
		shadow *= (1.f/(3.7f));
		*/
		return shadow;
	}
#endif

#ifdef SHADOW_NVQ2

#define TEXPIXSIZE 0.0009765f

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize, float2 vPos)
	{
		float  shadow = tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w)).r;
/*
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(TEXPIXSIZE, TEXPIXSIZE)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(TEXPIXSIZE, -TEXPIXSIZE)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(-TEXPIXSIZE, TEXPIXSIZE)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(-TEXPIXSIZE, -TEXPIXSIZE)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(TEXPIXSIZE, 0)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(-TEXPIXSIZE, 0)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(0, TEXPIXSIZE)).r;
			shadow += tex2D(ShadowMapSampler, (texCoord.xy/texCoord.w) + float2(0, -TEXPIXSIZE)).r;

		shadow *= 0.111111f;
*/
		return shadow;
	}
#endif

#ifdef SHADOW_R32F_BILINEAR

	float GetShadow(uniform sampler2D ShadowMapSampler, float shadowTestDepth, float4 texCoord, float texSize, float2 vPos)
	{
		float dt = 1.0f/1024.0f;//texSize;
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

  		float4 dt1 = lerp(shadow0, shadow1, lineari.x);
  		float4 dt2 = lerp(shadow2, shadow3, lineari.x);
 
  		float shadow = lerp(dt2, dt1, lineari.y);

		return shadow;
	}

#endif



