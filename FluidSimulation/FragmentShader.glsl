#version 330 core
out vec4 FragColor;
in vec2 localCoordinates;

uniform vec3 color = vec3(0.4f, 0.4f, 0.8f);

void main()
{
	float R = 1.f;
	float dist = sqrt(dot(localCoordinates, localCoordinates));

	if (dist >= R) {
		discard;
	}
	else {
		float alpha = smoothstep(R,R-0.2,dist);
		FragColor = vec4(color, alpha);
	}
}