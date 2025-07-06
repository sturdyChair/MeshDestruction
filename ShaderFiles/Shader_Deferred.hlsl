#include "Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vLightPos;
vector g_vLightDir;
vector cameraPos;
vector ambientLight;

Texture2D gAlbedo;
Texture2D gNormal;
Texture2D gORM;
Texture2D gPosition;
Texture2D gEmission;
int iNumPoint = 0;
int iNumSpot = 0;
int iNumDirectional = 0;

Texture2D g_Texture;
Texture2D g_GlowTexture;
float2 gaussFilter[7] =
{
    -3.0, 1.0 / 64.0,
   -2.0, 6.0 / 64.0,
   -1.0, 15.0 / 64.0,
   0.0, 20.0 / 64.0,
   1.0, 15.0 / 64.0,
   2.0, 6.0 / 64.0,
   3.0, 1.0 / 64.0
};

float texScalerX = 1.0 / 1280.0;
float texScalerY = 1.0 / 720.0;
float3 grayScale = float3(1, 1, 1);

PointLight pointLights[32];


DirectionalLight directionalLights[32];


SpotLight spotLights[32];


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_FULLSCREEN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    In.vPosition.xy = sign(In.vPosition);
    Out.vPosition = float4(In.vPosition.xy, 1.f, 1.f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUT_CROSS
{
    float4 vColorUp : SV_TARGET0;
    float4 vColorRight : SV_TARGET1;
};

float4 PS_MAIN(PS_IN input) : SV_TARGET0
{
    float4 albedo = gAlbedo.Sample(PointSampler, input.TexCoord);
    float4 normalTex = gNormal.Sample(PointSampler, input.TexCoord);
    float4 orm = gORM.Sample(PointSampler, input.TexCoord);
    float4 position = gPosition.Sample(PointSampler, input.TexCoord);


    float3 normal = normalize(normalTex.xyz * 2.0 - 1.0);


    float occlusion = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;


    float3 viewDir = normalize(cameraPos.xyz - position.xyz);

    float3 lighting = ambientLight.xyz * albedo.rgb * occlusion;
    
    for (int i = 0; i < iNumPoint; ++i)
    {
        lighting += CalculatePointLight(pointLights[i], position.xyz, normal, viewDir, albedo.rgb, occlusion, roughness, metallic);
    }

    for (int j = 0; j < iNumDirectional; ++j)
    {
        lighting += CalculateDirectionalLight(directionalLights[j], position.xyz, normal, viewDir, albedo.rgb, occlusion, roughness, metallic);
    }

    for (int k = 0; k < iNumSpot; ++k)
    {
        lighting += CalculateSpotLight(spotLights[k], position.xyz, normal, viewDir, albedo.rgb, occlusion, roughness, metallic);
    }

    float4 color = float4(lighting, albedo.a);
    float4 emssionColor = 0;
    [unroll]
    for (i = 0; i < 7; i++)
    {
        emssionColor += gEmission.Sample(PointSampler, float2(input.TexCoord.x + gaussFilter[i].x * texScalerX, input.TexCoord.y)) * gaussFilter[i].y;
    }
    [unroll]
    for (i = 0; i < 7; i++)
    {
        emssionColor += gEmission.Sample(PointSampler, float2(input.TexCoord.x, input.TexCoord.y + gaussFilter[i].x * texScalerY)) * gaussFilter[i].y;
    }
    
    color.rgb += emssionColor.rgb;
    if(color.a < 0.05f)
        discard;
    
    return color;
}

float4 PS_TEMP(PS_IN input) : SV_TARGET0
{
    float4 vColor = gAlbedo.Sample(PointSampler, input.TexCoord);
    if (vColor.a < 0.01f)
    {
        discard;
    }
    vColor.w = 1.f;
    return vColor;

}

float4 PS_NORMAL(PS_IN input) : SV_TARGET0
{
    return gNormal.Sample(PointSampler, input.TexCoord);
}

float3 CalculatePointLight_PBR(PointLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0);
float3 CalculateDirectionalLight_PBR(DirectionalLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0);
float3 CalculateSpotLight_PBR(SpotLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0);

float3 CalculatePointLights(int numLights, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0)
{
    float3 totalLighting = float3(0, 0, 0);
    [unroll]
    for (int i = 0; i < numLights; ++i)
    {
        PointLight light = pointLights[i];
        totalLighting += CalculatePointLight_PBR(light, worldPos, normal, viewDir, albedo, occlusion, roughness, metallic, F0);
    }
    return totalLighting;
}

float3 CalculateDirectionalLights(int numLights, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0)
{
    float3 totalLighting = float3(0, 0, 0);
    [unroll]
    for (int i = 0; i < numLights; ++i)
    {
        DirectionalLight light = directionalLights[i];
        totalLighting += CalculateDirectionalLight_PBR(light, worldPos, normal, viewDir, albedo, occlusion, roughness, metallic, F0);
    }
    return totalLighting;
}

float3 CalculateSpotLights(int numLights, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0)
{
    float3 totalLighting = float3(0, 0, 0);
    [unroll]
    for (int i = 0; i < numLights; ++i)
    {
        SpotLight light = spotLights[i];
        totalLighting += CalculateSpotLight_PBR(light, worldPos, normal, viewDir, albedo, occlusion, roughness, metallic, F0);
    }
    return totalLighting;
}

float3 CalculatePointLight_PBR(PointLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0)
{
    // PBR 연산
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    
    float attenuation = saturate(1.0 - distance / light.range);
    float3 irradiance = light.color * light.intensity * attenuation;
    
    float3 halfwayDir = normalize(lightDir + viewDir);
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    float3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

    float3 numerator = NDF * G * F;
    float3 denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    float3 specular = light.specColor * numerator / denominator;

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0);
    float3 color = (kD * albedo / PI + specular) * irradiance * NdotL + albedo * light.ambientColor.rgb * attenuation;

    return color;
}

float3 CalculateDirectionalLight_PBR(DirectionalLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0)
{
    // PBR 연산
    float3 lightDir = normalize(-light.direction);
    float3 irradiance = light.color * light.intensity;
    
    float3 halfwayDir = normalize(lightDir + viewDir);
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    float3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

    float3 numerator = NDF * G * F;
    float3 denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    float3 specular = light.specColor * numerator / denominator;

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0);
    float3 color = (kD * albedo / PI + specular) * irradiance * NdotL + albedo * light.ambientColor.rgb;

    return color;
}

float3 CalculateSpotLight_PBR(SpotLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic, float3 F0)
{
    // PBR 연산
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = saturate(1.0 - distance / light.range);
    float3 irradiance = light.color * light.intensity * attenuation;
    
    float3 halfwayDir = normalize(lightDir + viewDir);
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    float3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

    float3 numerator = NDF * G * F;
    float3 denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    float3 specular = light.specColor * numerator / denominator;

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerConeCos - light.outerConeCos;
    float intensity = saturate((theta - light.outerConeCos) / epsilon);

    float3 color = (kD * albedo / PI + specular) * irradiance * NdotL * intensity;

    return color;
}

float4 PS_PBR(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 albedo = gAlbedo.Sample(PointSampler, texCoord);
    float4 normalTex = gNormal.Sample(PointSampler, texCoord);
    float4 orm = gORM.Sample(PointSampler, texCoord);
    float4 position = gPosition.Sample(PointSampler, texCoord);

    float3 normal = normalize(normalTex.xyz * 2.0 - 1.0);

    float occlusion = orm.r;
    float roughness = max(orm.g, 0.0001); // 0으로 나누기 방지
    float metallic = orm.b;

    float3 viewDir = normalize(cameraPos.xyz - position.xyz);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo.rgb, metallic);

    float3 lighting = (float3)0;

    lighting += CalculatePointLights(iNumPoint, position.xyz, normal, viewDir, albedo.rgb, occlusion, roughness, metallic, F0) * 1.5f;
    lighting += CalculateDirectionalLights(iNumDirectional, position.xyz, normal, viewDir, albedo.rgb, occlusion, roughness, metallic, F0);
    lighting += CalculateSpotLights(iNumSpot, position.xyz, normal, viewDir, albedo.rgb, occlusion, roughness, metallic, F0);
    lighting *= 2.0f;
    float4 color = float4(lighting, albedo.a);
    float4 emssionColor = 0;
    [unroll]
    for (int i = 0; i < 7; i++)
    {
        emssionColor += gEmission.Sample(PointSampler, float2(texCoord.x + gaussFilter[i].x * texScalerX, texCoord.y)) * gaussFilter[i].y;
    }
    [unroll]
    for (i = 0; i < 7; i++)
    {
        emssionColor += gEmission.Sample(PointSampler, float2(texCoord.x, texCoord.y + gaussFilter[i].x * texScalerY)) * gaussFilter[i].y;
    }
    color.rgb += emssionColor.rgb * emssionColor.a;
    
    //float3 lightDir = position.xyz - g_vLightPos.xyz;
    //float dist = length(lightDir);

    //// 라이트 샤프트 계산
    //float3 lightDirNorm = normalize(lightDir);
    //float attenuation = exp(-dist * density);
    //float intensity = weight * attenuation * (1.0f / (dist * decay + 1.f));
    
    //color.rgb += normalize(ambientLight.rgb) * intensity * exposure;
    if(color.a < 0.01f)
        discard;
    
    return color;
}


float4 PS_DIRECTIONAL(PS_IN In) : SV_TARGET0
{
    vector vNormalDesc = gNormal.Sample(PointSampler, In.TexCoord);

    float3 vNormal = (vNormalDesc.xyz * 2.f - 1.f);

    
    vector vShade = ambientLight;
   
    for (int i = 0; i < iNumDirectional; ++i)
    {
        DirectionalLight light = directionalLights[i];
        vShade += max(dot(normalize(light.direction) * -1.f, normalize(vNormal)), 0.f) * vector(light.color, 1.f); //* light.intensity;
    }
    for (int j = 0; j < iNumPoint; ++j)
    {
        PointLight light = pointLights[j];
        float3 vPos = light.position;
        float3 direction = vPos - In.vPosition.xyz;
        float distance = length(direction);
        direction = normalize(direction);
        float attenuation = saturate(1.0 - distance / light.range);
        vShade += max(dot(normalize(direction) * -1.f, normalize(vNormal)), 0.f) * vector(light.color, 1.f) * attenuation; //* light.intensity;
    }
    return gAlbedo.Sample(PointSampler, In.TexCoord) * vShade;
}
float4 PS_POWER(PS_IN In) : SV_TARGET0
{
    
    float4 c = g_Texture.Sample(LinearSampler, In.TexCoord);
    //c.rgb = c.rgb / (c.a);
    //c.a = 1.f - 1.f / (c.a + 1.f);


    return c;
}
float4 PS_BLUR_X(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    int i;
    for (i = 0; i < 7; i++)
    {
        vColor += g_Texture.Sample(LinearSampler, float2(In.TexCoord.x, In.TexCoord.y + gaussFilter[i].x * texScalerY)) * gaussFilter[i].y;
    }
    return vColor;
}

float4 PS_BLUR_Y(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
   
    int i;
    for (i = 0; i < 7; i++)
    {
        vColor += g_Texture.Sample(LinearSampler, float2(In.TexCoord.x + gaussFilter[i].x * texScalerX, In.TexCoord.y)) * gaussFilter[i].y;
    }
    return vColor;
}


float4 PS_GLOW(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    vColor = g_Texture.Sample(LinearSampler, In.TexCoord);
    vColor += g_GlowTexture.Sample(LinearSampler, In.TexCoord) * 2.5f;
    vColor.rgb = vColor.rgb / (vColor.a) * 1.3f;
    vColor.a = 1.f - 1.f / (vColor.a + 1.f) ;
    vColor.a *= 1.3f;
    

    if (vColor.a < 0.01f)
        discard;
    
    return vColor;
}

float4 PS_CROSS_POWER(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    vColor = g_Texture.Sample(LinearSampler, In.TexCoord);
    if (vColor.r < 0.6f)
        vColor.r = 0.f;
    if (vColor.g < 0.6f)
        vColor.g = 0.f;
    if (vColor.b < 0.6f)
        vColor.b = 0.f;
    vColor.w = saturate(vColor.w * 2.f);
    return vColor;
}

float4 PS_CROSS(PS_IN In) : SV_TARGET0
{
    float4 Out = (float4) 0;
    
    int i;
    for (i = 0; i < 7; i++)
    {
        Out += g_Texture.Sample(LinearSampler, float2(In.TexCoord.x + gaussFilter[i].x * texScalerX, In.TexCoord.y)) * gaussFilter[i].y;
    }
    for (i = 0; i < 7; i++)
    {
        Out += g_Texture.Sample(LinearSampler, float2(In.TexCoord.x, In.TexCoord.y + gaussFilter[i].x * texScalerY)) * gaussFilter[i].y;
    }

    return Out;
}

float2 screenCenter;
float blurStrength;

float4 PS_RADIAL_BLUR(PS_IN In) : SV_TARGET0
{
    float2 direction = In.TexCoord - screenCenter;
    //float distance = length(direction);
    //direction /= distance;

    float4 color = gAlbedo.Sample(LinearSampler, In.TexCoord);
    float samples = 10.0f;
    float totalWeight = 1.0f;

    for (float i = 1.0f; i <= samples; ++i)
    {
        float weight = (samples - i) / samples;
        float2 sampleUV = In.TexCoord - direction * (i / samples) * blurStrength;
        color += gAlbedo.Sample(LinearSampler, sampleUV) * weight;
        totalWeight += weight;
    }

    return color / totalWeight;

}
float4 PS_ZERO_MASK(PS_IN input) : SV_TARGET0
{
    float4 color = g_Texture.Sample(PointSampler, input.TexCoord);

    if (color.a < 0.01f)
        discard;
    
    return color;
}

Texture2D g_Noise_Texture;
float g_Dissolve;
float g_DissolveRange = 0.1f;

float4 PS_DISSOLVE(PS_IN input) : SV_TARGET0
{
    float4 vColor = gAlbedo.Sample(PointSampler, input.TexCoord);
    
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, input.TexCoord);
    float fNoise = vNoise.x;
    if (fNoise < g_Dissolve)
    {
        float fDist = g_Dissolve - fNoise;
        if (fDist < g_DissolveRange)
        {
            vColor = lerp(vector(1.f, 0.5f, 0.f, 1.0f), vColor, fDist / g_DissolveRange);
        }
        else
        {
            discard;
        }
    }
    return vColor;

}


float4 PS_NPR(PS_IN input) : SV_TARGET0
{
    float4 albedo = gAlbedo.Sample(PointSampler, input.TexCoord);
    float4 normalTex = gNormal.Sample(PointSampler, input.TexCoord);
    float4 position = gPosition.Sample(PointSampler, input.TexCoord);


    float3 normal = normalize(normalTex.xyz * 2.0 - 1.0);



    float3 viewDir = normalize(cameraPos.xyz - position.xyz);

    float3 lighting = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < iNumPoint; ++i)
    {
        lighting += CalculatePointNPR(pointLights[i], position.xyz, normal, viewDir);
    }

    for (int j = 0; j < iNumDirectional; ++j)
    {
        lighting += CalculateDirectionalNPR(directionalLights[j], position.xyz, normal, viewDir);
    }

    for (int k = 0; k < iNumSpot; ++k)
    {
        lighting += CalculateSpotNPR(spotLights[k], position.xyz, normal, viewDir);
    }
    
    float ao = 0.0f;
    for (int l = 0; l < 4; l++)
    {
        float4 samplePos = gPosition.Sample(PointSampler, input.TexCoord + gNoise[l]);
        ao += CalculateAO(position.xyz, normal, samplePos.xyz);
    }
    ao *= 0.25f;
    lighting *= (1.f - ao);
    lighting = pow(saturate(lighting), 0.5f);
    lighting *= 4.f;
    lighting = ceil(lighting);
    lighting /= 4.f;
    float4 color = albedo * float4(lighting, 1.f);
    float4 emssionColor = gEmission.Sample(PointSampler, input.TexCoord);
    color.rgb += emssionColor.rgb * emssionColor.a;
    float4 OutLine = (float4)0;
    for (uint n = 0; n < 9; n++)
        OutLine += LaplacianMask[n] * (gNormal.Sample(LinearSampler, input.TexCoord + float2(coord[n % 3] /1280.f, coord[n / 3] / 720.f)));
    float gray = 1 - dot(grayScale, OutLine.xyz);
    if (gray < 0.3f)
        color.rgb = (float3) 0;
        
   if (color.a < 0.05f)
        discard;
    
    
    
    return color;
}

float4 PS_WEIGHTED_BLENDED(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    vColor = g_Texture.Sample(LinearSampler, In.TexCoord);
    vColor.rgb = vColor.rgb / (vColor.a);
    vColor.a = 1.f - 1.f / (vColor.a + 1.f);
    
    if (vColor.a < 0.01f)
        discard;
    
    return vColor;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Temp
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TEMP();
    }
    pass PBR
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PBR();
    }
    pass Normal
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NORMAL();
    }
    pass Directional
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DIRECTIONAL();
    }
    pass GlowPower//5
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_POWER();
    }
    pass GlowBlurX//6
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_X();
    }
    pass GlowBlurY//7
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_Y();
    }
    pass Glow//8
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GLOW();
    }
    pass CrossPower //9
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CROSS_POWER();
    }
    pass Cross //10
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CROSS();
    }
    pass RadialBlur //11
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR();
    }
    pass ZeroMask //12
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ZERO_MASK();
    }
    pass Dissolve //13
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
    pass NPR //14
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NPR();
    }
    pass WeightedBlended //15
    {
        SetDepthStencilState(DS_NonZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHTED_BLENDED();
    }
//
}
