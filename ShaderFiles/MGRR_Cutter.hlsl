struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    uint4  blendIndices;
    float4 blendWeights;
};

StructuredBuffer<Vertex> inputVerts : register(t0);
StructuredBuffer<uint4> indices : register(t1);

AppendStructuredBuffer<uint3> outputVertsUp : register(u0);
AppendStructuredBuffer<uint3> outputVertsDown : register(u1);
RWStructuredBuffer<Vertex> outputNewVertex : register(u2);
RWStructuredBuffer<uint2> outputNewEdge : register(u3);

float4x4 identity =
{
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};
#define fEps  1e-4f
#define planaEps 1e-5f
cbuffer CutPlane : register(b0)
{
    float3 planeNormal;
    float planeD;
};

cbuffer BoneMatrices : register(b1)
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

float3 AttachToPlane(float3 v)
{
    float3 result;
    float d = dot(v, planeNormal);
    float lsq = dot(planeNormal, planeNormal);
    float t = (d + planeD) / max(lsq, 0.00001f);
    result.x = v.x - planeNormal.x * t;
    result.y = v.y - planeNormal.y * t;
    result.z = v.z - planeNormal.z * t;
    
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
void temp( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x ;
    uint iNumVert, iStride;
    inputVerts.GetDimensions(iNumVert, iStride);
    uint3 tri = indices[index].xyz;
    
    if (indices[index].w != 3)
        return;
    
    Vertex v[3] = { inputVerts[tri.x], inputVerts[tri.y], inputVerts[tri.z] };
    
    float3 pos[3];
    float3 norm[3];
    float3 tang[3];
    float2 uv[3];
    float d[3] =
    {
        0.f,0.f,0.f
    };

    float4 skinnedPlane[3];

    for (int i = 0; i < 3; ++i)
    {
        float4x4 skinningMatrix = 0;
        float accw = 0.f;
        [unroll]
        for (int j = 0; j < 3; ++j)
        {
            float4x4 bone = Matrices[v[i].blendIndices[j]];
            float w = v[i].blendWeights[j];
            accw += w;
            skinningMatrix += bone * w;
        }
        skinningMatrix += Matrices[v[i].blendIndices[3]] * (1.f - accw);
  
        //bool isZero = all(skinningMatrix == 0);
        //isZero = isZero || accw > 1.1f || accw < 0.9f;
        //if (isZero)
        //{
        //    skinningMatrix = 0;
        //    accw = 0.f;
        //    [unroll]
        //    for (int j = 0; j < 4; ++j)
        //    {
        //        float4x4 bone = Matrices[v[(i + 1) % 3].blendIndices[j]];
        //        float w = v[(i + 1) % 3].blendWeights[j];
        //        accw += w;
        //        skinningMatrix += bone * w;
        //    }
        //    isZero = all(skinningMatrix == 0);
        //    isZero = isZero || accw > 1.1f || accw < 0.9f;
        //    if (isZero)
        //    {
        //        skinningMatrix = 0;
        //        accw = 0.f;
        //        [unroll]
        //        for (int j = 0; j < 4; ++j)
        //        {
        //            float4x4 bone = Matrices[v[(i + 2) % 3].blendIndices[j]];
        //            float w = v[(i + 2) % 3].blendWeights[j];
        //            accw += w;
        //            skinningMatrix += bone * w;
        //        }
        //        isZero = all(skinningMatrix == 0);
        //        isZero = isZero || accw > 1.1f || accw < 0.9f;
        //        if (isZero)
        //            return;
        //        else
        //        {
        //            v[i].blendIndices = v[(i + 2) % 3].blendIndices;
        //            v[i].blendWeights = v[(i + 2) % 3].blendWeights;
        //        }
        //
        //    }
        //    else
        //    {
        //        v[i].blendIndices = v[(i + 1) % 3].blendIndices;
        //        v[i].blendWeights = v[(i + 1) % 3].blendWeights;
        //    }
        //}
        //skinningMatrix = isZero ? identity : skinningMatrix;
        
        d[i] = dot(planeNormal, mul(skinningMatrix, float4(v[i].pos, 1.f)).xyz) + planeD;
        pos[i] = v[i].pos;
        norm[i] = v[i].normal;
        tang[i] = v[i].tangent;
        uv[i] = v[i].uv;
    }

    
    bool side0 = d[0] >= 0;
    bool side1 = d[1] >= 0;
    bool side2 = d[2] >= 0;

    int count = ((side0) + (side1) + (side2));

    int side0Idx = side0 ? 1 : -1;
    int side1Idx = side1 ? 2 : -2;
    int side2Idx = side2 ? 3 : -3;
    
    if (count == 3)
    {
        outputVertsUp.Append(tri);
    }
    else if (count == 0)
    {
        outputVertsDown.Append(tri);
    }
    else if (count == 1)//two bottom
    {
        Vertex Out[3];
        Vertex newVert[4];
        int upIdx = max(side0Idx, 0) + max(side1Idx, 0) + max(side2Idx, 0) - 1;
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            Out[i].pos = pos[i];
            Out[i].normal = norm[i];
            Out[i].uv = uv[i];
            Out[i].tangent = tang[i];
            Out[i].blendIndices = v[i].blendIndices;
            v[i].blendWeights = uv[i].x <= 1.f || length(v[i].blendWeights) <= 1.5f || length(v[i].blendWeights) >= 0.5f ? v[i].blendWeights : normalize(v[i].blendWeights);
            Out[i].blendWeights = v[i].blendWeights;
        }
        int upNext = (upIdx + 1) % 3;
        int upBef = (upIdx + 2) % 3;
        float t0 = saturate(d[upIdx] / (d[upIdx] - d[upNext]));
        float t1 = saturate(d[upIdx] / (d[upIdx] - d[upBef]));
        newVert[0] = LerpVertex(Out[upIdx], Out[upNext], t0);
        newVert[2] = newVert[0];
        //newVert[0].pos = AttachToPlane(newVert[0].pos);
        newVert[1] = LerpVertex(Out[upIdx], Out[upBef], t1);
        newVert[3] = newVert[1];
        //newVert[1].pos = AttachToPlane(newVert[1].pos);
        
         
        newVert[0].blendIndices = v[upIdx].blendIndices;
        newVert[0].blendWeights = v[upIdx].blendWeights;
        newVert[2].blendIndices = v[upNext].blendIndices;
        newVert[2].blendWeights = v[upNext].blendWeights;
        
        newVert[1].blendIndices = v[upIdx].blendIndices;
        newVert[1].blendWeights = v[upIdx].blendWeights;
        newVert[3].blendIndices = v[upBef].blendIndices;
        newVert[3].blendWeights = v[upBef].blendWeights;
        
        uint i0 = outputNewVertex.IncrementCounter();
        uint i1 = outputNewVertex.IncrementCounter();
        uint i2 = outputNewVertex.IncrementCounter();
        uint i3 = outputNewVertex.IncrementCounter();
        
        outputNewVertex[i0] = newVert[0];
        outputNewVertex[i1] = newVert[1];
        outputNewVertex[i2] = newVert[2];
        outputNewVertex[i3] = newVert[3];
        
        outputNewEdge[outputNewEdge.IncrementCounter()] = uint2(i0, i1);
        
        
        outputVertsUp.Append(uint3(tri[upIdx], iNumVert + i0, iNumVert + i1));
        
        outputVertsDown.Append(uint3(iNumVert + i2, tri[upNext], tri[upBef]));
        outputVertsDown.Append(uint3(iNumVert + i2, tri[upBef], iNumVert + i3));
    }
    else if (count == 2)
    {
        Vertex Out[3];
        Vertex newVert[4];
        int downIdx = -min(side0Idx, 0) - min(side1Idx, 0) - min(side2Idx, 0) - 1;
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            Out[i].pos = pos[i];
            Out[i].normal = norm[i];
            Out[i].uv = uv[i];
            Out[i].tangent = tang[i];
            Out[i].blendIndices = v[i].blendIndices;
            v[i].blendWeights = uv[i].x <= 1.f || length(v[i].blendWeights) <= 1.5f || length(v[i].blendWeights) >= 0.5f ? v[i].blendWeights : normalize(v[i].blendWeights);
            Out[i].blendWeights = v[i].blendWeights;
        }
        int downNext = (downIdx + 1) % 3;
        int downBef = (downIdx + 2) % 3;
        float t0 = saturate(d[downIdx] / (d[downIdx] - d[downNext]));
        float t1 = saturate(d[downIdx] / (d[downIdx] - d[downBef]));
        
        newVert[0] = LerpVertex(Out[downIdx], Out[downNext], t0);
        //newVert[0].pos = AttachToPlane(newVert[0].pos);
        newVert[2] = newVert[0];
        newVert[1] = LerpVertex(Out[downIdx], Out[downBef], t1);
        //newVert[1].pos = AttachToPlane(newVert[1].pos);
        newVert[3] = newVert[1];
        
        newVert[0].blendIndices = v[downIdx].blendIndices;
        newVert[0].blendWeights = v[downIdx].blendWeights;
        newVert[2].blendIndices = v[downNext].blendIndices;
        newVert[2].blendWeights = v[downNext].blendWeights;
        
        newVert[1].blendIndices = v[downIdx].blendIndices;
        newVert[1].blendWeights = v[downIdx].blendWeights;
        newVert[3].blendIndices = v[downBef].blendIndices;
        newVert[3].blendWeights = v[downBef].blendWeights;
        
        uint i0 = outputNewVertex.IncrementCounter();
        uint i1 = outputNewVertex.IncrementCounter();
        uint i2 = outputNewVertex.IncrementCounter();
        uint i3 = outputNewVertex.IncrementCounter();
        outputNewVertex[i0] = newVert[0];
        outputNewVertex[i1] = newVert[1];
        outputNewVertex[i2] = newVert[2];
        outputNewVertex[i3] = newVert[3];
        
        outputNewEdge[outputNewEdge.IncrementCounter()] = uint2(i0, i1);
        
        outputVertsDown.Append(uint3(tri[downIdx], iNumVert + i0, iNumVert + i1));
        
        outputVertsUp.Append(uint3(iNumVert + i2, tri[downNext], tri[downBef]));
        outputVertsUp.Append(uint3(iNumVert + i2, tri[downBef], iNumVert + i3));
    }
    return;
}

//    
//    
//    

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    uint iNumVert, iStride;
    inputVerts.GetDimensions(iNumVert, iStride);
    uint3 tri = indices[index].xyz;
    
    if (indices[index].w != 3)
        return;
    uint tris[3];
    tris[0] = tri.x;
    tris[1] = tri.y;
    tris[2] = tri.z;
    
    Vertex v[3] = { inputVerts[tri.x], inputVerts[tri.y], inputVerts[tri.z] };
    
    float3 pos[3];
    float3 norm[3];
    float3 tang[3];
    float2 uv[3];
    float d[3] =
    {
        0.f, 0.f, 0.f
    };

    float4 skinnedPlane[3];

    for (int i = 0; i < 3; ++i)
    {
        float4x4 skinningMatrix = 0;
        float accw = 0.f;
        [unroll]
        for (int j = 0; j < 3; ++j)
        {
            float4x4 bone = Matrices[v[i].blendIndices[j]];
            float w = v[i].blendWeights[j];
            accw += w;
            skinningMatrix += bone * w;
        }
        skinningMatrix += Matrices[v[i].blendIndices[3]] * (1.f - accw);
 
        d[i] = dot(planeNormal, mul(skinningMatrix, float4(v[i].pos, 1.f)).xyz) + planeD;
        pos[i] = v[i].pos;
        norm[i] = v[i].normal;
        tang[i] = v[i].tangent;
        uv[i] = v[i].uv;
    }
    bool side[3], nside[3];
    
    side[0] = d[0] >= planaEps;
    side[1] = d[1] >= planaEps;
    side[2] = d[2] >= planaEps;
    nside[0] = d[0] <= -planaEps;
    nside[1] = d[1] <= -planaEps;
    nside[2] = d[2] <= -planaEps;
    bool coplanar[3];
    for (int j = 0; j < 3; ++j)
    {
        coplanar[j] = !side[j] && !nside[j];

    }
    
    uint up[4], down[4], ucount = 0, dcount = 0;
    
    for (int k = 0; k < 3; ++k)
    {
        if (side[k])
        {
            up[ucount] = tris[k];
            ++ucount;
            uint nextIdx = (k + 1) % 3;
            if (nside[nextIdx])
            {
                Vertex newVert;
                float t = saturate(d[k] / (d[k] - d[nextIdx]));
                newVert = LerpVertex(v[k], v[nextIdx], t);
                newVert.blendIndices = v[k].blendIndices;
                newVert.blendWeights = v[k].blendWeights;
                uint i0 = outputNewVertex.IncrementCounter();
                outputNewVertex[i0] = newVert;
                
                newVert.blendIndices = v[nextIdx].blendIndices;
                newVert.blendWeights = v[nextIdx].blendWeights;
                uint i1 = outputNewVertex.IncrementCounter();
                outputNewVertex[i1] = newVert;
                up[ucount] = iNumVert + i0;
                ++ucount;
                down[dcount] = iNumVert + i1;
                ++dcount;
            }
        }
        if(nside[k])
        {
            down[dcount] = tris[k];
            ++dcount;
            uint nextIdx = (k + 1) % 3;
            if (side[nextIdx])
            {
                Vertex newVert;
                float t = saturate(d[k] / (d[k] - d[nextIdx]));
                newVert = LerpVertex(v[k], v[nextIdx], t);
                newVert.blendIndices = v[k].blendIndices;
                newVert.blendWeights = v[k].blendWeights;
                uint i0 = outputNewVertex.IncrementCounter();
                outputNewVertex[i0] = newVert;
                
                newVert.blendIndices = v[nextIdx].blendIndices;
                newVert.blendWeights = v[nextIdx].blendWeights;
                uint i1 = outputNewVertex.IncrementCounter();
                outputNewVertex[i1] = newVert;
                down[dcount] = iNumVert + i0;
                ++dcount;
                up[ucount] = iNumVert + i1;
                ++ucount;
            }
        }
        if (coplanar[k])
        {
            up[ucount] = tris[k];
            ++ucount;
            down[dcount] = tris[k];
            ++dcount;
        }
    }
    if (ucount == 3)
    {
        outputVertsUp.Append(uint3(up[0], up[1], up[2]));

    }
    else if(ucount == 4)
    {
        outputVertsUp.Append(uint3(up[0], up[1], up[2]));
        outputVertsUp.Append(uint3(up[0], up[2], up[3]));
    }
    
    if (dcount == 3)
    {
        outputVertsDown.Append(uint3(down[0], down[1], down[2]));

    }
    else if (dcount == 4)
    {
        outputVertsDown.Append(uint3(down[0], down[1], down[2]));
        outputVertsDown.Append(uint3(down[0], down[2], down[3]));
    }
    return;
}