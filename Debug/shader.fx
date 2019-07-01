Texture2D ObjTexture : register( t0 );
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
    return  abs(vInversion - ObjTexture.Sample( ObjSamplerState, input.TexCoord ));
}