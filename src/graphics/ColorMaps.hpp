#pragma once
#include <algorithm>

#include "data_structures/Array.hpp"

struct Color
{
	float r, g, b;
};

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

enum class ColorPalette
{
	VIRIDIS,
	INFERNO,
	MAGMA,
	PLASMA,
	CIVIDIS,
	COOLWARM,
	JET
};
// hashing function
namespace std
{
	template<>
	struct hash<ColorPalette>
	{
		size_t operator()(ColorPalette palette) const noexcept
		{
			return std::hash<std::underlying_type_t<ColorPalette>>{}(static_cast<std::underlying_type_t<ColorPalette>>(palette));
		}
	};
}

class ColorMaps
{
private:
	inline static constexpr int NUM_POINTS = 8;

	// --- Viridis Data ---
	inline static const float s_viridisParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_viridisPoints[NUM_POINTS] = {
		{0.267004f, 0.004874f, 0.329415f}, {0.282623f, 0.163453f, 0.478433f},
		{0.20803f, 0.30864f, 0.556083f},   {0.138852f, 0.445272f, 0.563339f},
		{0.125189f, 0.582848f, 0.52226f},  {0.285836f, 0.71442f, 0.425114f},
		{0.68826f, 0.81646f, 0.252934f},   {0.993248f, 0.906157f, 0.143936f}
	};

	// --- Inferno Data ---
	inline static const float s_infernoParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_infernoPoints[NUM_POINTS] = {
		{0.000304f, 0.000304f, 0.003886f}, {0.111815f, 0.01244f, 0.231365f},
		{0.327493f, 0.090813f, 0.406935f}, {0.598286f, 0.201389f, 0.38484f},
		{0.849495f, 0.344485f, 0.22941f},  {0.98394f, 0.548434f, 0.11326f},
		{0.996538f, 0.835495f, 0.311388f}, {0.988362f, 0.998364f, 0.745138f}
	};

	// --- Magma Data ---
	inline static const float s_magmaParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_magmaPoints[NUM_POINTS] = {
		{0.001462f, 0.001462f, 0.003922f}, {0.103758f, 0.07542f, 0.264253f},
		{0.316849f, 0.12876f, 0.42775f},   {0.556214f, 0.22857f, 0.435728f},
		{0.793653f, 0.363198f, 0.347535f}, {0.963298f, 0.563837f, 0.32298f},
		{0.994998f, 0.812328f, 0.49077f},  {0.986637f, 0.964341f, 0.993438f}
	};
	// --- Plasma Data ---
	inline static const float s_plasmaParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_plasmaPoints[NUM_POINTS] = {
		{0.050383f, 0.029803f, 0.527975f}, {0.288297f, 0.053344f, 0.665639f},
		{0.490795f, 0.126764f, 0.682397f}, {0.693246f, 0.217314f, 0.621217f},
		{0.871686f, 0.340306f, 0.505952f}, {0.978588f, 0.504281f, 0.344463f},
		{0.995383f, 0.70757f, 0.24273f},   {0.940015f, 0.923249f, 0.384931f}
	};
	// --- Cividis Data ---
	inline static const float s_cividisParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_cividisPoints[NUM_POINTS] = {
		{0.000000f, 0.203922f, 0.266667f}, {0.239387f, 0.270932f, 0.428362f},
		{0.417402f, 0.355147f, 0.468382f}, {0.570776f, 0.443325f, 0.425317f},
		{0.709020f, 0.539461f, 0.334314f}, {0.838525f, 0.647317f, 0.222241f},
		{0.956863f, 0.768627f, 0.117647f}, {1.000000f, 0.901961f, 0.243137f}
	};
	// --- Coolwarm Data ---
	inline static const float s_coolwarmParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_coolwarmPoints[NUM_POINTS] = {
		{0.229806f, 0.298718f, 0.753693f}, {0.417939f, 0.509314f, 0.841219f},
		{0.613381f, 0.701831f, 0.916792f}, {0.825244f, 0.870836f, 0.957593f},
		{0.938814f, 0.814341f, 0.739433f}, {0.865924f, 0.592523f, 0.480088f},
		{0.705673f, 0.354165f, 0.219213f}, {0.496032f, 0.180126f, 0.127814f}
	};
	// --- Jet Data ---
	inline static const float s_jetParam[NUM_POINTS] = { 0.0f, 0.142f, 0.285f, 0.428f, 0.571f, 0.714f, 0.857f, 1.0f };
	inline static const Color s_jetPoints[NUM_POINTS] = {
		{0.0f, 0.0f, 0.5f},       // Deep Blue
		{0.0f, 0.0f, 1.0f},       // Blue
		{0.0f, 1.0f, 1.0f},       // Cyan
		{0.5f, 1.0f, 0.5f},       // Greenish-Yellow
		{1.0f, 1.0f, 0.0f},       // Yellow
		{1.0f, 0.5f, 0.0f},       // Orange
		{1.0f, 0.0f, 0.0f},       // Red
		{0.5f, 0.0f, 0.0f}        // Dark Red
	};
private:
	static Color getColorFromMap(float t, const float* param, const Color* colorPoints)
	{
		// clamp t
		t = std::max(0.0f, std::min(1.0f, t));
		// find two points to interpolate between
		for (int i = 0; i < NUM_POINTS - 1; i++)
		{
			if (t >= param[i] && t <= param[i + 1])
			{
				// local interpolation parameter
				float locT = (t - param[i])
					/ (param[i + 1] - param[i]);
				return{
					lerp(colorPoints[i].r, colorPoints[i + 1].r, locT),
					lerp(colorPoints[i].g, colorPoints[i + 1].g, locT),
					lerp(colorPoints[i].b, colorPoints[i + 1].b, locT),
				};
			}
		}
	}
	static Color getViridis(float t)
	{
		return getColorFromMap(t, s_viridisParam, s_viridisPoints);
	}
	static Color getInferno(float t)
	{
		return getColorFromMap(t, s_infernoParam, s_infernoPoints);
	}
	static Color getMagma(float t)
	{
		return getColorFromMap(t, s_magmaParam, s_magmaPoints);
	}
	static Color getPlasma(float t)
	{
		return getColorFromMap(t, s_plasmaParam, s_plasmaPoints);
	}
	static Color getCividis(float t)
	{
		return getColorFromMap(t, s_cividisParam, s_cividisPoints);
	}
	static Color getCoolwarm(float t)
	{
		return getColorFromMap(t, s_coolwarmParam, s_coolwarmPoints);
	}
	static Color getJet(float t)
	{
		return getColorFromMap(t, s_jetParam, s_jetPoints);
	}
public:
	static Color getColor(float t, ColorPalette palette)
	{
		if (palette == ColorPalette::VIRIDIS)
		{
			return getViridis(t);
		}
		if (palette == ColorPalette::INFERNO)
		{
			return getInferno(t);
		}
		if (palette == ColorPalette::MAGMA)
		{
			return getMagma(t);
		}
		if (palette == ColorPalette::PLASMA)
		{
			return getPlasma(t);
		}
		if (palette == ColorPalette::CIVIDIS)
		{
			return getCividis(t);
		}
		if (palette == ColorPalette::COOLWARM)
		{
			return getCoolwarm(t);
		}
		if (palette == ColorPalette::JET)
		{
			return getJet(t);
		}
	}
	static Array<Color> getPaletteData(ColorPalette palette, int resolution = 256)
	{
		static Array<Color> data(resolution);
		for (int i = 0; i < resolution; i++)
		{
			float t = static_cast<float>(i) / (resolution - 1);
			data[i] = getColor(t, palette);
		}
		return data;
	}
};