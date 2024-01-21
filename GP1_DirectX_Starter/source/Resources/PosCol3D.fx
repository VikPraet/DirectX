float4x4 gWorldViewProj : WorldViewProjection;

bool gUseNormalMap : UseNormalMap = true;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float3 gLightDirection : LightDirection = float3(0.577f, -0.577f, 0.577f);
float3 gAmbientIntensity : Ambient = float3(0.03f, 0.03f, 0.03f);
float4x4 gWorldMatrix : WORLD;
float3 gCameraPosition : CAMERA;

float gPI : PI = float(3.14159265359);
float gLightIntensity : LightIntensity = float(7.0f);
float gShininess : Shininess = float(25.0f);

SamplerState gSamplerState : Sampler;

RasterizerState gRasterizerState
{
    CullMode = back;
    FrontCounterClockwise = false;
};

BlendState gBlendState
{
    BlendEnable[0] = FALSE;
    SrcBlend = ONE;
    DestBlend = ZERO;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less;
    StencilEnable = false;
};

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
    float4 WorldPosition : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    // Transform the incoming vertex position from model space to world space
    output.WorldPosition = mul(float4(input.Position, 1.0f), gWorldMatrix);
    
    // Perform other vertex shader calculations
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
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
    return dot(input.Normal, -gLightDirection);
}

float4 Labmert(VS_OUTPUT input)
{
    return ((gDiffuseMap.Sample(gSamplerState, input.UV) * gLightIntensity) / gPI);
}

float4 Phong(VS_OUTPUT input)
{
    float3 invViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);
    
    float sampledGloss = gGlossinessMap.Sample(gSamplerState, input.UV);
    float exp = sampledGloss * gShininess;
    
    float specularReflectance = gSpecularMap.Sample(gSamplerState, input.UV);
    
    float3 reflectedRay = reflect(gLightDirection, -input.Normal);
    float cosAlpha = dot(normalize(reflectedRay), normalize(invViewDirection));
    float specularIntensity = specularReflectance * pow(cosAlpha, exp);
    
    return saturate(specularIntensity);
}

float3 transformNormal(VS_OUTPUT input)
{
    // Sample the normal map
    float3 sampledNormal = 2 * gNormalMap.Sample(gSamplerState, input.UV).xyz - float3(1, 1, 1);
    
    // Transform the sampled normal from object space to tangent space
    float3 binormal = cross(input.Normal, input.Tangent);
    float3x3 tangentSpaceAxis =
    {
        input.Tangent,
        binormal,
        input.Normal
    };
    
    float3 transformedNormal = mul(sampledNormal, tangentSpaceAxis);
    transformedNormal = normalize(transformedNormal);
    
    return transformedNormal;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{    
    if(gUseNormalMap) input.Normal = transformNormal(input);
    
    float4 finalColor = Labmert(input) * ObservedArea(input) + Phong(input) + float4(gAmbientIntensity, 1.0f);
    
    return saturate(finalColor);
}

technique11 DefaultTechnique
{
    pass PO
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}