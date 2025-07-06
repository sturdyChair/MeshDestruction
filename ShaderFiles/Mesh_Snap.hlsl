struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    uint4  blendIndices;
    float4 blendWeights;
};

struct SVertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
};

StructuredBuffer<Vertex> inputVerts : register(t0);

RWStructuredBuffer<SVertex> outputNewVertex : register(u0);

float4x4 identity =
{
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};
#define fEps  1e-4f

cbuffer BoneMatrices : register(b0)
{
    float4x4 Matrices[512];
};

Vertex LerpVertex(Vertex v0, Vertex v1, float t)
{
    Vertex result;
    result.pos = lerp(v0.pos, v1.pos, t);
    result.pos = floor(result.pos / fEps) * fEps;
    result.normal = normalize(lerp(v0.normal, v1.normal, t));
    result.tangent = normalize(lerp(v0.tangent, v1.tangent, t));
    result.uv = lerp(v0.uv, v1.uv, t);

    return result;
}

float3x3 Inverse3x3(float3x3 m)
{
    float3 a = m[0];
    float3 b = m[1];
    float3 c = m[2];

    float3 r0 = cross(b, c);
    float3 r1 = cross(c, a);
    float3 r2 = cross(a, b);

    float det = dot(r2, c);
    return transpose(float3x3(r0, r1, r2)) / det;
}
float4 TransformPlane(float3 normal, float d, float4x4 transform)
{
    float3x3 invTrans = transpose((float3x3) transform);
    float3 transformedNormal = normalize(mul(normal, invTrans));

    float3 pointOnPlane = normal * -d;

    float4 transformedPoint = mul(float4(pointOnPlane, 1.0), transform);

    float newD = -dot(transformedNormal, transformedPoint.xyz);

    return float4(transformedNormal, newD);
}

[numthreads(64, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x ;
    uint iNumVert, iStride;
    inputVerts.GetDimensions(iNumVert, iStride);
    Vertex v = inputVerts[index];


    float4x4 skinningMatrix = 0;
    float accw = 0.f;
    [unroll]
    for (int j = 0; j < 3; ++j)
    {
        float4x4 bone = Matrices[v.blendIndices[j]];
        float w = v.blendWeights[j];
        accw += w;
        skinningMatrix += bone * w;
    }
    skinningMatrix += Matrices[v.blendIndices[3]] * (1.f - accw);
    //bool isZero = all(skinningMatrix == 0);
    //isZero = isZero || accw > 1.1f || accw < 0.9f;
    //if (isZero)
    //{
    //    skinningMatrix = identity;
    //}
    outputNewVertex[index].pos = mul(skinningMatrix, float4(v.pos, 1.f)).xyz;
    outputNewVertex[index].normal = mul(skinningMatrix, float4(v.normal, 0.f)).xyz;
    outputNewVertex[index].tangent = mul(skinningMatrix, float4(v.tangent, 0.f)).xyz;
    outputNewVertex[index].uv = v.uv;
    outputNewVertex.IncrementCounter();
    return;
}