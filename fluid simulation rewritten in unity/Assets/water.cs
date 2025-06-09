using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
using Unity.Mathematics;

public class Water : MonoBehaviour{
    // particle system settings
    public float3 minBound = new float3(0);
    public float3 maxBound = new float3(226);
    public float particleSize = 1f;
    public float gravity = 9.8f;
    public float particleMass = 1f;
    public float particleDamping = .05f;
    public float densityRadius = 1.9f;
    public float targetDensity = 2.75f;
    public float pressureMultiplier = 50f;
    public float viscosityMultiplier = .15f;
    public float nearPressureMultiplier = 1f;
    public Color particleColor = Color.blue;
    public ComputeShader computeShader;
    private Material material;

    // creates a 3d cube of particles with even spacing between particles
    void createWaterCube(ref List<float4> particles, ref List<float4> velocities, float3 offset, float size, int rows, int cols, int planes){
        float3 spacing = new float3(size / rows, size / cols, size / planes);
        float4 pos = new float4(0);

        for (int i = 0; i < rows; ++i){
            pos.x = offset.x + i * spacing.x;
            for (int j = 0; j < cols; ++j){
                pos.y = offset.y + j * spacing.y;
                for (int k = 0; k < planes; ++k){
                    pos.z = offset.z + k * spacing.z;

                    float rx = UnityEngine.Random.Range(-spacing.x / 4, spacing.x / 4);
                    float ry = UnityEngine.Random.Range(-spacing.y / 4, spacing.y / 4);
                    float rz = UnityEngine.Random.Range(-spacing.z / 4, spacing.z / 4);

                    particles.Add(pos + new float4(rx, ry, rz, 0));
                    velocities.Add(new float4(0, 0, 0, 0));
                }
            }
        }
    }

    private ComputeBuffer particleBuffer;
    private ComputeBuffer velocitiesBuffer;
    private ComputeBuffer predictedBuffer;
    private ComputeBuffer densitiesBuffer;
    private ComputeBuffer nearbyBuffer;
    private ComputeBuffer nearbyIdxBuffer;
    private int CSPredicted;
    private int CSNearbySort;
    private int CSNearbyIdx;
    private int CSCalcDensity;
    private int CSCalcForces;
    private int particleCount;
    private Mesh quadMesh;
    private Bounds boundingBox;
    private int dispatchSize;
    
    void Start(){
        // TODO particle_count must be power of 2 for the parallel sort -- MUST FIX
        const int N = 5;
        int r = (int)Mathf.Pow(2f, N);
        int c = (int)Mathf.Pow(2f, N);
        int p = (int)Mathf.Pow(2f, N);
        //float size = length(max_bound - min_bound) / 2. - 10.;
        float size = maxBound.x / 2 - 10;

        List<float4> particles = new List<float4>();
        List<float4> velocities = new List<float4>();
        createWaterCube(ref particles, ref velocities, minBound, maxBound.x, r, c, p);
        //createWaterCube(ref particles, minBound + new float3(size + 15, 5, size + 15), size, r, c, p);
        particleCount = particles.Count;
        Debug.Log(particleCount);

        material = new Material(Shader.Find("Unlit/waterShader"));
        material.SetBuffer("_Particles", particleBuffer);
        quadMesh = Resources.GetBuiltinResource<Mesh>("Quad.fbx");

        // create all of the buffers
        particleBuffer = new ComputeBuffer(particleCount, sizeof(float) * 4);
        particleBuffer.SetData(particles);
        velocitiesBuffer = new ComputeBuffer(particleCount, sizeof(float) * 4);
        velocitiesBuffer.SetData(velocities);
        predictedBuffer = new ComputeBuffer(particleCount, sizeof(float) * 4);
        densitiesBuffer = new ComputeBuffer(particleCount, sizeof(float) * 2);
        nearbyBuffer = new ComputeBuffer(particleCount, sizeof(int) * 2);
        nearbyIdxBuffer = new ComputeBuffer(particleCount, sizeof(int));

        CSPredicted = computeShader.FindKernel("CSPredicted");
        computeShader.SetBuffer(CSPredicted, "_Particles", particleBuffer);
        computeShader.SetBuffer(CSPredicted, "_Velocities", velocitiesBuffer);
        computeShader.SetBuffer(CSPredicted, "_PredictedParticles", predictedBuffer);
        computeShader.SetBuffer(CSPredicted, "_Nearby", nearbyBuffer);
        computeShader.SetBuffer(CSPredicted, "_NearbyIdx", nearbyIdxBuffer);

        CSNearbySort = computeShader.FindKernel("CSNearbySort");
        computeShader.SetBuffer(CSNearbySort, "_Nearby", nearbyBuffer);

        CSNearbyIdx = computeShader.FindKernel("CSNearbyIdx");
        computeShader.SetBuffer(CSNearbyIdx, "_Nearby", nearbyBuffer);
        computeShader.SetBuffer(CSNearbyIdx, "_NearbyIdx", nearbyIdxBuffer);

        CSCalcDensity = computeShader.FindKernel("CSCalcDensity");
        computeShader.SetBuffer(CSCalcDensity, "_PredictedParticles", predictedBuffer);
        computeShader.SetBuffer(CSCalcDensity, "_Densities", densitiesBuffer);
        computeShader.SetBuffer(CSCalcDensity, "_Nearby", nearbyBuffer);
        computeShader.SetBuffer(CSCalcDensity, "_NearbyIdx", nearbyIdxBuffer);

        CSCalcForces = computeShader.FindKernel("CSCalcForces");
        computeShader.SetBuffer(CSCalcForces, "_Particles", particleBuffer);
        computeShader.SetBuffer(CSCalcForces, "_Velocities", velocitiesBuffer);
        computeShader.SetBuffer(CSCalcForces, "_PredictedParticles", predictedBuffer);
        computeShader.SetBuffer(CSCalcForces, "_Densities", densitiesBuffer);
        computeShader.SetBuffer(CSCalcForces, "_Nearby", nearbyBuffer);
        computeShader.SetBuffer(CSCalcForces, "_NearbyIdx", nearbyIdxBuffer);

        computeShader.SetInt("_ParticleCount", particleCount);
        boundingBox = new Bounds((minBound + maxBound) / 2f, maxBound - minBound);
        dispatchSize = Mathf.CeilToInt(particleCount / 32f);

        Application.targetFrameRate = 120;
    }

    void Update(){
        UpdateSettings();
        
        computeShader.Dispatch(CSPredicted, dispatchSize, 1, 1);

        for (int k = 2; k <= particleCount; k *= 2){
            for (int j = k / 2; j > 0; j /= 2){
                computeShader.SetInt("_K", k);
                computeShader.SetInt("_J", j);

                computeShader.Dispatch(CSNearbySort, dispatchSize, 1, 1);
            }
        }

        computeShader.Dispatch(CSNearbyIdx, dispatchSize, 1, 1);
        computeShader.Dispatch(CSCalcDensity, dispatchSize, 1, 1);
        // float4[] data = new float4[particleCount];
        // predictedBuffer.GetData(data);
        // for (int i = 0; i < particleCount; ++i){
        //     Debug.Log(data[i]);
        // }
        // Debug.Log("END");

        computeShader.Dispatch(CSCalcForces, dispatchSize, 1, 1);

        material.SetBuffer("_Particles", particleBuffer);
        Graphics.DrawMeshInstancedProcedural(quadMesh, 0, material, boundingBox, particleCount);
    }

    void UpdateSettings(){
        // simulation settings set every frame so user can interact with them
        computeShader.SetFloat("_ParticleMass", particleMass);
        computeShader.SetFloat("_DensityRadius", densityRadius);
        computeShader.SetFloat("_TargetDensity", targetDensity);
        computeShader.SetFloat("_ParticleDamping", particleDamping);
        computeShader.SetFloat("_PressureMultiplier", pressureMultiplier);
        computeShader.SetFloat("_ViscosityMultiplier", viscosityMultiplier);
        computeShader.SetFloat("_NearPressureMultiplier", nearPressureMultiplier);
        computeShader.SetVector("_MinBound", new float4(minBound, 0));
        computeShader.SetVector("_MaxBound", new float4(maxBound, 0));
        computeShader.SetFloat("_Gravity", gravity);
        computeShader.SetFloat("_DT", Mathf.Min(Time.deltaTime, .05f));

        // settings for making the quads front facing
        material.SetVector("_CameraRight", Camera.main.transform.right);
        material.SetVector("_CameraUp", Camera.main.transform.up);

        // quad settings
        material.SetFloat("_Size", particleSize);
        material.SetColor("_Color", particleColor);
        
        boundingBox = new Bounds((minBound + maxBound) / 2f, maxBound - minBound);
    }

    void OnRenderObject(){}

    // draw the bounding box of the simulation
    void OnDrawGizmos(){
        Gizmos.color = Color.white;
        Gizmos.DrawWireCube(boundingBox.center, boundingBox.size);
    }

    void OnDestroy(){
        particleBuffer?.Release();
        velocitiesBuffer?.Release();
        predictedBuffer?.Release();
        nearbyBuffer?.Release();
        nearbyIdxBuffer?.Release();
    }
}
