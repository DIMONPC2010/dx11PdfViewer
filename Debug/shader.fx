Texture2D ObjTexture[2] : register( t0 );
SamplerState ObjSamplerState : register( s0 );


cbuffer cbChangesEveryFrame : register( b0 )
{
	float4x4 WVP;
	float vInversion;
};


struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, WVP);
    output.TexCoord = inTexCoord;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float2 newTexcord;
	newTexcord.x = input.TexCoord.x*9;
	newTexcord.y = input.TexCoord.y*9;
	float4 color1 = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
	float4 color2 = ObjTexture[1].Sample(ObjSamplerState, newTexcord);
	

	if((input.TexCoord.x < 0.1 || input.TexCoord.x > 0.2 || input.TexCoord.y > 0.1))
		return abs(vInversion - color1);
	else
		return  abs(vInversion - saturate(color1 * color2 * 1.0));
}