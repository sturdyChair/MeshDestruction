sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};
sampler DefaultSampler = sampler_state
{
    AddressU = wrap;
    AddressV = wrap;
};
sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
    BlendEnable[2] = false;
    BlendEnable[3] = false;
    BlendEnable[4] = false;
    BlendEnable[5] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = false;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = false;
	
    SrcBlend = src_alpha;
    DestBlend = inv_Src_Alpha;
    SrcBlendAlpha = one;
    DestBlendAlpha = zero;
    BlendOp = add;
};
BlendState BS_EffectBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = false;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = false;
	
    SrcBlend = src_alpha;
    DestBlend = one;
    SrcBlendAlpha = one;
    DestBlendAlpha = zero;
    BlendOp = add;
};
BlendState BS_WeightBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = false;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = false;
	
    SrcBlend = one;
    DestBlend = one;
    SrcBlendAlpha = one;
    DestBlendAlpha = one;
    BlendOp = add;
};
RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

RasterizerState RS_CULL_CW
{
    FillMode = solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_CULL_CCW
{
    FillMode = solid;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CULL_None
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

DepthStencilState DS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;

    StencilEnable = false;
};
DepthStencilState DS_NonZ
{
    DepthEnable = false;
    DepthWriteMask = zero;

    StencilEnable = false;
};
DepthStencilState DS_NoWriteZ
{
    DepthEnable = true;
    DepthWriteMask = zero;

    StencilEnable = false;
};

DepthStencilState WriteStencil
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = Equal;

    StencilEnable = true;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;
    
    
    FrontFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFail = keep;
    FrontFaceStencilDepthFail = keep;

    BackFaceStencilFunc = ALWAYS;
    BackFaceStencilPass = REPLACE;
    BackFaceStencilFail = keep;
    BackFaceStencilDepthFail = keep;

};
DepthStencilState WriteBackFaceStencil
{
    DepthEnable = true;
    DepthWriteMask = all;

    StencilEnable = TRUE;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;
    
    
    FrontFaceStencilFunc = ALWAYS;
    FrontFaceStencilPass = ZERO;
    FrontFaceStencilFail = KEEP;
    FrontFaceStencilDepthFail = KEEP;

    BackFaceStencilFunc = ALWAYS;
    BackFaceStencilPass = REPLACE;
    BackFaceStencilFail = KEEP;
    BackFaceStencilDepthFail = KEEP;

};
DepthStencilState TestStencilEqual1
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS;

    StencilEnable = TRUE;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0x00;

    FrontFaceStencilFunc = EQUAL;
    FrontFaceStencilPass = KEEP;
    FrontFaceStencilFail = KEEP;
    FrontFaceStencilDepthFail = KEEP;

    BackFaceStencilFunc = EQUAL;
    BackFaceStencilPass = KEEP;
    BackFaceStencilFail = KEEP;
    BackFaceStencilDepthFail = KEEP;

};
DepthStencilState TestStencilEqual0
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS;

    StencilEnable = TRUE;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0x00;

    
    FrontFaceStencilFunc = EQUAL;
    FrontFaceStencilPass = KEEP;
    FrontFaceStencilFail = KEEP;
    FrontFaceStencilDepthFail = KEEP;

    BackFaceStencilFunc = EQUAL;
    BackFaceStencilPass = KEEP;
    BackFaceStencilFail = KEEP;
    BackFaceStencilDepthFail = KEEP;
};


struct Light
{
    float3 position;
    float3 diffuseColor;
    float3 ambientColor;
    float3 specularColor;
    float intensity;
};
struct PointLight
{
    float3 position;
    float range;
    float3 color;
    float intensity;
    float3 specColor;
    float pad;
    float4 ambientColor;
};

struct DirectionalLight
{
    float3 direction;
    float intensity;
    float3 color;
    float pad1;
    float3 specColor;
    float pad2;
    float4 ambientColor;
};

struct SpotLight
{
    float3 position;
    float range;
    float3 direction;
    float innerConeCos;
    float3 color;
    float outerConeCos;
    float intensity;
    float3 specColor;
};

float3 CalculatePointLight(PointLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic)
{
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = saturate(1.0 - distance / light.range);
    float3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * light.color * albedo;

    // Specular
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0 / max(roughness, 0.001f));
    float3 specular = spec * light.color * metallic;

    // Combine results
    float3 color = (diffuse + specular) * light.intensity * attenuation;
    return color;
}


float3 CalculatePointNPR(PointLight light, float3 worldPos, float3 normal, float3 viewDir)
{
    float3 lightDir = light.position - worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = saturate(1.0 - distance / light.range);
    float3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * light.color;

    // Specular
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0 / max(roughness, 0.001f));
  //  float3 specular = spec * light.specColor;

    // Combine results
    float3 color = diffuse * light.intensity * attenuation;
    return color;
}

float3 CalculateDirectionalLight(DirectionalLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic)
{
    float3 lightDir = normalize(-light.direction); // Directional light direction is opposite

    float3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * light.color * albedo;

    // Specular
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0 / max(roughness, 0.001f));
    float3 specular = spec * light.color * metallic;

    // Combine results
    float3 color = (diffuse + specular) * light.intensity;// * occlusion;
    return color;
}

float3 CalculateDirectionalNPR(DirectionalLight light, float3 worldPos, float3 normal, float3 viewDir)
{
    float3 lightDir = normalize(-light.direction); // Directional light direction is opposite

    float3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * light.color + light.ambientColor.xyz;

    // Specular
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0 / max(roughness, 0.001f));
    //float3 specular = spec * light.color;

    // Combine results
    float3 color = diffuse * light.intensity; // * occlusion;
    return color;
}

float3 CalculateSpotLight(SpotLight light, float3 worldPos, float3 normal, float3 viewDir, float3 albedo, float occlusion, float roughness, float metallic)
{
    float3 lightToPixel = light.position - worldPos;
    float distance = length(lightToPixel);
    lightToPixel = normalize(lightToPixel);

    float attenuation = saturate(1.0 - distance / light.range);
    float3 halfwayDir = normalize(lightToPixel + viewDir);

    float spotFactor = saturate((dot(lightToPixel, normalize(-light.direction)) - light.outerConeCos) / (light.innerConeCos - light.outerConeCos));
    attenuation *= spotFactor;

    // Diffuse
    float diff = max(dot(normal, lightToPixel), 0.0);
    float3 diffuse = diff * light.color * albedo;

    // Specular
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0 / max(roughness, 0.001f));
    float3 specular = spec * light.color * metallic;

    // Combine results
    float3 color = (diffuse + specular) * light.intensity * attenuation;// * occlusion;
    return color;
}

float3 CalculateSpotNPR(SpotLight light, float3 worldPos, float3 normal, float3 viewDir)
{
    float3 lightToPixel = light.position - worldPos;
    float distance = length(lightToPixel);
    lightToPixel = normalize(lightToPixel);

    float attenuation = saturate(1.0 - distance / light.range);
    float3 halfwayDir = normalize(lightToPixel + viewDir);

    float spotFactor = saturate((dot(lightToPixel, normalize(-light.direction)) - light.outerConeCos) / (light.innerConeCos - light.outerConeCos));
    attenuation *= spotFactor;

    // Diffuse
    float diff = max(dot(normal, lightToPixel), 0.0);
    float3 diffuse = diff * light.color;

    // Specular
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0 / max(roughness, 0.001f));
    //float3 specular = spec * light.color * metallic;

    // Combine results
    float3 color = diffuse  * light.intensity * attenuation; // * occlusion;
    return color;
}

float4 normalMapSample(Texture2D normalMap, float2 TexCoord, float4 Tangent, float4 Bitangent, float4 Normal)
{
    // ³ë¸Ö ¸Ê »ùÇÃ¸µ
    float3 normalSample = normalMap.Sample(LinearSampler, TexCoord).rgb;
    normalSample = normalize(normalSample * 2.0 - 1.0); 

    float3x3 TBN = (float3x3(Tangent.xyz, Bitangent.xyz, Normal.xyz));
    float3 worldNormal = normalize(mul(normalSample, TBN));

    return float4(worldNormal * 0.5f + float3(0.5f,0.5f,0.5f), 0.0f);
}
uint g_iObjectID;

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265359 * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
#define PI 3.14159265359


static const float3 ssaoKernel[8] = 
{
    0.5381f, 0.1856f, 0.4319f,
    0.1379f, 0.2486f, 0.4430f,
    0.3371f, 0.5679f, 0.0057f,
    -0.6999f, -0.0451f, -0.0019f,
    0.0689f, -0.1598f, 0.7980f,
    -0.1437f, -0.3695f, 0.2806f,
    -0.6919f, 0.4943f, 0.1077f,
    0.5625f, 0.1424f, -0.5945f
};

static const float2 gNoise[8] =
{
    0.01f, 0.01f,
    0.01f, -0.01f,
    -0.01f, 0.01f,
    -0.01f, -0.01f,
    0.0f, 0.01f,
    0.0f,-0.01f,
    0.01f, 0.0f,
    -0.01f, 0.0f
};

float CalculateAO(float3 pos, float3 normal, float3 samplePos)
{
    float3 offset = samplePos - pos;
    float distance = length(offset);
    if (distance < 0.5f)
    {
        return 0.f;
    }
    float3 dir = normalize(offset);
    float NdotD = max(dot(normal, dir), 0.0);
    float ao = NdotD * (1.0 / (1.0 + distance * distance));
    return ao;
}

float LaplacianMask[9] =
{
    -1, -1, -1,
      -1, 8, -1,
      -1, -1, -1
};
       
float coord[3] = { -1, 0, +1 };

float4 QuaternionFromEuler(float3 euler)
{
    float pitch = euler.x;
    float yaw = euler.y;
    float roll = euler.z;

    float cy = cos(0.5f * yaw);
    float sy = sin(0.5f * yaw);
    float cp = cos(0.5f * pitch);
    float sp = sin(0.5f * pitch);
    float cr = cos(0.5f * roll);
    float sr = sin(0.5f * roll);

    float4 q;
    q.x = sr * cp * cy - cr * sp * sy; // x
    q.y = cr * sp * cy + sr * cp * sy; // y
    q.z = cr * cp * sy - sr * sp * cy; // z
    q.w = cr * cp * cy + sr * sp * sy; // w

    return q;
}

float3 RotateVectorByQuaternion(float3 v, float4 q)
{
    float3 qv = q.xyz;
    float3 t = 2.0f * cross(qv, v);
    float3 rotated = v + q.w * t + cross(qv, t);
    return rotated;
}