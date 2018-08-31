#version 150 core


out vec4 outColor;

vec3 CameraDirection = vec3(0.3, 0.5, -0.2);
vec3 CameraUp = vec3(0, 0, 1);
vec3 CameraPosition = vec3(-60, -60, 30);

float FieldOfView = 90.0f;
float AspectRatio = 1024.0f / 768.0f;

vec3 GetRay(vec2 screen_pos)
{
	vec3 CameraRight = cross(CameraUp, CameraDirection);

	vec3 ray = CameraDirection
		+ screen_pos.x * CameraRight * FieldOfView / 90.0
		+ screen_pos.y * CameraUp * FieldOfView / 90.0 / AspectRatio;

	return normalize(ray);
}

vec4 castRay(vec3 ray)
{
	if (ray.z >= 0)
	{
		return vec4(0.26, 0.58, 0.92, 1000);
	}
	else
	{
		vec2 floorxy = CameraPosition.xy + CameraPosition.z * ray.xy / ray.z;

		if ((int(floorxy.x) + int(floorxy.y)) % 2 == 0)
		{
			return vec4(1, 1, 1, length(CameraPosition.z * ray.xy / ray.z));
		}
		else
		{
			return vec4(0, 0, 0, length(CameraPosition.z * ray.xy / ray.z));
		}
	}
}

void main()
{
	vec2 screen_pos1 = ((gl_FragCoord.xy + vec2(-0.3, -0.3)) / vec2(1024, 768)) * 2.0f - vec2(1, 1);
	vec2 screen_pos2 = ((gl_FragCoord.xy + vec2(+0.3, -0.3)) / vec2(1024, 768)) * 2.0f - vec2(1, 1);
	vec2 screen_pos3 = ((gl_FragCoord.xy + vec2(-0.3, +0.3)) / vec2(1024, 768)) * 2.0f - vec2(1, 1);
	vec2 screen_pos4 = ((gl_FragCoord.xy + vec2(+0.3, +0.3)) / vec2(1024, 768)) * 2.0f - vec2(1, 1);
	
	vec4 rayCastResult1 = castRay(GetRay(screen_pos1));
	vec4 rayCastResult2 = castRay(GetRay(screen_pos2));
	vec4 rayCastResult3 = castRay(GetRay(screen_pos3));
	vec4 rayCastResult4 = castRay(GetRay(screen_pos4));
    
	vec4 rayCastResult = (rayCastResult1 + rayCastResult2 + rayCastResult3 + rayCastResult4) / 4;

	float dist = rayCastResult[3];

	vec3 col = mix(rayCastResult.rgb, vec3(0.26, 0.58, 0.92), clamp(dist, 0, 200) / 200);

	outColor = vec4(col, 1.0);
}
