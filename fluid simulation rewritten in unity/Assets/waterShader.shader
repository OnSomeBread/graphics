Shader "Unlit/waterShader"{
    Properties{
        _MainTex ("Particle Texture", 2D) = "white" {}
        _Size ("Particle Size", float) = 1
        _Color ("Particle Color", Color) = (0, 0, .5, 1)
        _Ambient ("Ambient Light", Color) = (.7, .8, .9, 1)
        _LightDir ("Light Direction", Vector) = (-0.5, 0.5, -0.5, 1)
        _Gloss("Gloss", Range(0,1)) = 1
    }
    SubShader{
        Tags { "RenderType"="Opaque" }
        
        Pass{
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            //#pragma multi_compile_instancing

            #include "UnityCG.cginc"
            #include "Lighting.cginc"
            #include "AutoLight.cginc"

            StructuredBuffer<float3> _Particles;
            float4 _Color;
            float _Size;
            float3 _CameraRight;
            float3 _CameraUp;
            float4 _Ambient;
            float4 _LightDir;
            float _Gloss;

            struct appdata{
                float3 vertex : POSITION;
                float2 uv : TEXCOORD0;
                uint instanceID : SV_InstanceID;
            };

            struct v2f{
                float4 vertex : SV_POSITION;
                float2 uv : TEXCOORD0;
                float3 wPos : TEXCOORD1;
                LIGHTING_COORDS(3,4)
            };

            v2f vert (appdata v){
                v2f o;
                float3 worldPos =  _Particles[v.instanceID] + (v.vertex.x * _CameraRight + v.vertex.y * _CameraUp) * _Size;

                o.vertex = UnityWorldToClipPos(float4(worldPos, 1));
                o.uv = v.uv;
                o.wPos = worldPos;
                TRANSFER_VERTEX_TO_FRAGMENT(o);
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

                //float3 lightDir = normalize(UnityWorldSpaceLightDir(i.wPos));
                float attenuation = LIGHT_ATTENUATION(i);
                float3 lambert = saturate(dot(quadNormal, normalize(_LightDir.xyz)));
                float3 diffuse = lambert * attenuation;

                // only apply ambient in base pass when doing multiple lights
                diffuse += _Ambient.rbg;

                // float3 V = normalize(_WorldSpaceCameraPos - i.wPos);
                // float3 halfVec = normalize(_LightDir.xyz + V);

                // float3 specularLight = saturate(dot(halfVec, quadNormal)) * (lambert > 0);
                // specularLight = pow(specularLight, exp2(_Gloss * 11) + 2) * _Gloss * attenuation;

                return tex2D(_MainTex, i.uv) * float4(diffuse * _Color.rgb, _Color.a);
            }
            ENDCG
        }
    }
}
