float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float3 gLightDirection : LightDirection = float3(0.577f, -0.577f, 0.577f);
float4x4 gWorldMatrix : WORLD;
float3 gCameraPosition : CAMERA;

float gPI : SV_GroupID = float(3.14159265359);
float gLightIntensity : LightIntensity = float(7.0f);
float gShininess : Shininess = float(25.0f);

SamplerState gSamplerState : Sampler;

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Color = input.Color;
    output.UV = input.UV;
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
    return output;
}

float4 Diffuse(VS_OUTPUT input)
{
    return gDiffuseMap.Sample(gSamplerState, input.UV);
}

float4 ObservedArea(VS_OUTPUT input)
{    
    return dot(input.Normal, gLightDirection);
}

float4 Labmert(VS_OUTPUT input)
{
    return ((gDiffuseMap.Sample(gSamplerState, input.UV) * gLightIntensity) / gPI);
}

float4 Phong(VS_OUTPUT input)
{
    float sampledGloss = gGlossinessMap.Sample(gSamplerState, input.UV);
    float exp = sampledGloss * gShininess;
    
    float specularReflectance = gSpecularMap.Sample(gSamplerState, input.UV);
    
    float3 reflectedRay = reflect(gLightDirection, -input.Normal);
    float cosAlpha = dot(normalize(reflectedRay), /*-viewDirection*/ gCameraPosition);
    float specularIntensity = specularReflectance * pow(cosAlpha, exp);
    
    return specularIntensity;
}


float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.Normal = 2 * (float3)gNormalMap.Sample(gSamplerState, input.UV) - float3(1, 1, 1);
    
    float3 binormal = input.Tangent - input.Normal;
   
    float3x3 tangentSpaceAxis = {
        input.Tangent,
        binormal,
        input.Normal
    };
    
    input.Normal = mul(input.Normal, tangentSpaceAxis);
    
    return /*Labmert(input) * */ObservedArea(input);
}

technique11 DefaultTechnique
{
    pass PO
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}