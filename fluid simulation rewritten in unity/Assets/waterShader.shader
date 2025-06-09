Shader "Unlit/waterShader"{
    Properties{
        _MainTex ("Particle Texture", 2D) = "white" {}
    }
    SubShader{
        Tags { "RenderType"="Opaque" }
        
        Pass{
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            //#pragma multi_compile_instancing

            #include "UnityCG.cginc"

            StructuredBuffer<float4> _Particles;
            float4 _Color;
            float _Size;
            float3 _CameraRight;
            float3 _CameraUp;

            struct appdata{
                float3 vertex : POSITION;
                float2 uv : TEXCOORD0;
                uint instanceID : SV_InstanceID;
            };

            struct v2f{
                float4 vertex : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            v2f vert (appdata v){
                v2f o;
                float3 pos = _Particles[v.instanceID];
                float3 worldPos = pos + (v.vertex.x * _CameraRight + v.vertex.y * _CameraUp) * _Size;

                o.vertex = UnityWorldToClipPos(float4(worldPos, 1));
                o.uv = v.uv;
                return o;
            }

            sampler2D _MainTex;

            fixed4 frag(v2f i) : SV_Target{
                float2 quadCenter = i.uv * 2. - 1.;
                float sqrDst = dot(quadCenter, quadCenter);
                if(sqrDst > 1) discard;

                // sphere imposter
                float quadz = sqrt(1 - sqrDst);
                float3 quadNormal = normalize(float3(quadCenter, quadz));

                return tex2D(_MainTex, i.uv) * float4(pow(_Color.rgb, 1.0/2.2), _Color.a);
            }
            ENDCG
        }
    }
}
