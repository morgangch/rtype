/*
 * @file colorblind.frag
 * @brief Colorblindness (Daltonism) simulation fragment shader
 *
 * Applies a post-process color transform to simulate common color vision
 * deficiencies. Intended to be drawn over a full-screen sprite that captures
 * the current frame. Blend strength allows smooth interpolation between the
 * original and simulated color.
 *
 * Uniforms:
 * - sampler2D texture  : Source image/frame (bound by SFML when drawing a sprite)
 * - int mode           : 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Achromatopsia
 * - float strength     : Blend factor in [0..1] (0 = original, 1 = full simulation)
 *
 * Notes:
 * - Protanopia/Deuteranopia/Tritanopia use 3x3 matrices that approximate
 *   the perceptual shifts for each deficiency.
 * - Achromatopsia uses standard luminance weights to convert to grayscale.
 * - Designed for SFML: gl_TexCoord[0] is provided automatically and the
 *   default sampler uniform name is "texture".
 */
// mode: 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Achromatopsia

uniform sampler2D texture;
uniform int mode;
uniform float strength; // 0..1 blending with original

void main()
{
    vec4 src = texture2D(texture, gl_TexCoord[0].xy);
    vec3 c = src.rgb;
    vec3 sim = c;

    if (mode == 1) {
        // Protanopia simulation matrix
        mat3 M = mat3(
            0.56667, 0.43333, 0.0,
            0.55833, 0.44167, 0.0,
            0.0,     0.24167, 0.75833
        );
        sim = clamp(M * c, 0.0, 1.0);
    } else if (mode == 2) {
        // Deuteranopia simulation matrix
        mat3 M = mat3(
            0.625, 0.375, 0.0,
            0.70,  0.30,  0.0,
            0.0,   0.30,  0.70
        );
        sim = clamp(M * c, 0.0, 1.0);
    } else if (mode == 3) {
        // Tritanopia simulation matrix
        mat3 M = mat3(
            0.95,  0.05,  0.0,
            0.0,   0.43333, 0.56667,
            0.0,   0.475,  0.525
        );
        sim = clamp(M * c, 0.0, 1.0);
    } else if (mode == 4) {
        // Achromatopsia (complete color blindness) - grayscale
        float g = dot(c, vec3(0.299, 0.587, 0.114));
        sim = vec3(g, g, g);
    } else {
        sim = c;
    }

    vec3 outColor = mix(c, sim, clamp(strength, 0.0, 1.0));
    gl_FragColor = vec4(outColor, src.a);
}
