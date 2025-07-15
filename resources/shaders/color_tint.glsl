#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 primaryTintColor;
uniform vec3 secondaryTintColor;
uniform vec3 primaryTargetColor;
uniform vec3 secondaryTargetColor;

uniform float tolerance;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);

    // Calculate color distance
    float primaryDistance = length(texelColor.rgb - primaryTargetColor);
    float secondaryDistance = length(texelColor.rgb - secondaryTargetColor);
    float distance = primaryDistance;

    vec3 tintColor = primaryTintColor;
    if(primaryDistance > secondaryDistance) {
      tintColor = secondaryTintColor;
      distance = secondaryDistance;
    }

    // If within tolerance, apply tint
    if (distance <= tolerance) {
        vec3 tinted = mix(texelColor.rgb, tintColor, 1.0);
        finalColor = vec4(tinted, texelColor.a) * colDiffuse * fragColor;
    } else {
        finalColor = texelColor * colDiffuse * fragColor;
    }
}
