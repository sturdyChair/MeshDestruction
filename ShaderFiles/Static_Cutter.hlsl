struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
};

StructuredBuffer<Vertex> inputVerts : register(t0);
StructuredBuffer<uint4> indices : register(t1);

AppendStructuredBuffer<uint3> outputVertsUp : register(u0);
AppendStructuredBuffer<uint3> outputVertsDown : register(u1);
RWStructuredBuffer<Vertex> outputNewVertex : register(u2);

float4x4 identity =
{
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};
#define fEps  1e-6f
#define planaEps  1e-5f

cbuffer CutPlane : register(b0)
{
    float3 planeNormal;
    float planeD;
};

Vertex LerpVertex(Vertex v0, Vertex v1, float t)
{
    Vertex result;
    result.pos = lerp(v0.pos, v1.pos, t);
    //result.pos = floor(result.pos / fEps) * fEps;
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
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x ;
    uint iNumVert, iStride;
    inputVerts.GetDimensions(iNumVert, iStride);
    uint3 tri = indices[index].xyz;
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
        0.f,0.f,0.f
    };


    for (int i = 0; i < 3; ++i)
    {
        d[i] = dot(planeNormal, v[i].pos) + planeD;
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
                uint i0 = outputNewVertex.IncrementCounter();
                outputNewVertex[i0] = newVert;

                up[ucount] = iNumVert + i0;
                ++ucount;
                down[dcount] = iNumVert + i0;
                ++dcount;
            }
        }
        if (nside[k])
        {
            down[dcount] = tris[k];
            ++dcount;
            uint nextIdx = (k + 1) % 3;
            if (side[nextIdx])
            {
                Vertex newVert;
                float t = saturate(d[k] / (d[k] - d[nextIdx]));
                newVert = LerpVertex(v[k], v[nextIdx], t);
                uint i0 = outputNewVertex.IncrementCounter();
                outputNewVertex[i0] = newVert;

                down[dcount] = iNumVert + i0;
                ++dcount;
                up[ucount] = iNumVert + i0;
                ++ucount;
            }
        }
        if (coplanar[k])
        {
            uint i0 = outputNewVertex.IncrementCounter();
            outputNewVertex[i0] = v[k];
            up[ucount] = iNumVert + i0;
            ++ucount;
            down[dcount] = iNumVert + i0;
            ++dcount;
        }
    }
    if (ucount == 3)
    {
        outputVertsUp.Append(uint3(up[0], up[1], up[2]));

    }
    else if (ucount == 4)
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
    
    
    
    //bool side0 = d[0] >= 0;
    //bool side1 = d[1] >= 0;
    //bool side2 = d[2] >= 0;
    //int count = ((side0) + (side1) + (side2));
    //
    //int side0Idx = side0 ? 1 : -1;
    //int side1Idx = side1 ? 2 : -2;
    //int side2Idx = side2 ? 3 : -3;
    //
    //if (count == 3)
    //{
    //    outputVertsUp.Append(tri);
    //}
    //else if (count == 0)
    //{
    //    outputVertsDown.Append(tri);
    //}
    //else if (count == 1)//two bottom
    //{
    //    Vertex Out[3];
    //    Vertex newVert[2];
    //    int upIdx = max(side0Idx, 0) + max(side1Idx, 0) + max(side2Idx, 0) - 1;
    //    [unroll]
    //    for (int i = 0; i < 3; ++i)
    //    {
    //        Out[i].pos = pos[i];
    //        Out[i].normal = norm[i];
    //        Out[i].uv = uv[i];
    //        Out[i].tangent = tang[i];
    //    }
    //    int upNext = (upIdx + 1) % 3;
    //    int upBef = (upIdx + 2) % 3;
    //    float t0 = saturate(d[upIdx] / (d[upIdx] - d[upNext]));
    //    float t1 = saturate(d[upIdx] / (d[upIdx] - d[upBef]));
    //    newVert[0] = LerpVertex(Out[upIdx], Out[upNext], t0);
    //
    //    newVert[1] = LerpVertex(Out[upIdx], Out[upBef], t1);
    //
    //    
    //    uint i0 = outputNewVertex.IncrementCounter();
    //    uint i1 = outputNewVertex.IncrementCounter();
    //    
    //    outputNewVertex[i0] = newVert[0];
    //    outputNewVertex[i1] = newVert[1];
    //    
    //    
    //    outputVertsUp.Append(uint3(tri[upIdx], iNumVert + i0, iNumVert + i1));
    //    
    //    outputVertsDown.Append(uint3(iNumVert + i0, tri[upNext], tri[upBef]));
    //    outputVertsDown.Append(uint3(iNumVert + i0, tri[upBef], iNumVert + i1));
    //}
    //else if (count == 2)
    //{
    //    Vertex Out[3];
    //    Vertex newVert[2];
    //    int downIdx = -min(side0Idx, 0) - min(side1Idx, 0) - min(side2Idx, 0) - 1;
    //    [unroll]
    //    for (int i = 0; i < 3; ++i)
    //    {
    //        Out[i].pos = pos[i];
    //        Out[i].normal = norm[i];
    //        Out[i].uv = uv[i];
    //        Out[i].tangent = tang[i];
    //    }
    //    int downNext = (downIdx + 1) % 3;
    //    int downBef = (downIdx + 2) % 3;
    //    float t0 = saturate(d[downIdx] / (d[downIdx] - d[downNext]));
    //    float t1 = saturate(d[downIdx] / (d[downIdx] - d[downBef]));
    //    
    //    newVert[0] = LerpVertex(Out[downIdx], Out[downNext], t0);
    //
    //    newVert[1] = LerpVertex(Out[downIdx], Out[downBef], t1);
    //
    //    
    //    
    //    uint i0 = outputNewVertex.IncrementCounter();
    //    uint i1 = outputNewVertex.IncrementCounter();
    //
    //    outputNewVertex[i0] = newVert[0];
    //    outputNewVertex[i1] = newVert[1];
    //    
    //    outputVertsDown.Append(uint3(tri[downIdx], iNumVert + i0, iNumVert + i1));
    //    
    //    outputVertsUp.Append(uint3(iNumVert + i0, tri[downNext], tri[downBef]));
    //    outputVertsUp.Append(uint3(iNumVert + i0, tri[downBef], iNumVert + i1));
    //}
    return;
}