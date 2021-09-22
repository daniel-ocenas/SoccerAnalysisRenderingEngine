#pragma once

#include <glm/glm.hpp>

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

constexpr Material EMERALD
{
	glm::vec3(0.0215f,0.1745f,0.0215f),
	glm::vec3(0.07568f,0.61424f,0.07568f),
	glm::vec3(0.633f,0.727811f,0.633f),
	0.6f,
};

constexpr Material BLUE
{
	glm::vec3(0.02f,0.02f,1.0f),
	glm::vec3(0.11f,0.56f,1.0f),
	glm::vec3(0.11f,0.56f,1.f),
	0.6f,
};

constexpr Material PITCH
{
	glm::vec3(0.0215f,0.1745f,0.0215f),
	glm::vec3(0.0215f,0.1745f,0.0215f),
	glm::vec3(0.0215f,0.1745f,0.0215f),
	0.6f,
};

constexpr Material WHITE
{
	glm::vec3(0.5f),
	glm::vec3(0.8f),
	glm::vec3(1.f),
	0.6f,
};

constexpr Material RED
{
	glm::vec3(0.5f, 0.f, 0.f),
	glm::vec3(0.9f, 0.f, 0.f),
	glm::vec3(0.9f, 0.f, 0.f),
	.25,
};

constexpr Material GREENRUBBER
{
	glm::vec3(0.0f, 0.05f, 0.0f),
	glm::vec3(0.4f, 0.5f, 0.4f),
	glm::vec3(0.04f, 0.7f, 0.04f),
	.078125f,
};

constexpr Material GRAY
{
	glm::vec3(0.6f, 0.6f, 0.6f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	0.1f,
};

constexpr Material METALIC
{
	glm::vec3(0.83f, 0.83f, 0.83f),
	glm::vec3(0.83f, 0.83f, 0.83f),
	glm::vec3(0.83f, 0.83f, 0.83f),
	.078125f,
};

constexpr Material REDPLASTIC
{
	glm::vec3(0.f, 0.f, 0.f),
	glm::vec3(0.5f, 0.f, 0.f),
	glm::vec3(0.7f, 0.6f, 0.6f),
	.25,
};

constexpr Material LIME
{
	glm::vec3(1.f, 1.f, 0.f),
	glm::vec3(1.f, 1.f, 0.f),
	glm::vec3(1.f, 1.f, 0.f),
	.25,
};

constexpr Material GOLD
{
	glm::vec3(0.329412f, 0.223529f, 0.027451f),
	glm::vec3(0.780392f, 0.568627f, 0.113725f),
	glm::vec3(0.992157f, 0.941176f, 0.807843f),
	0.51794872f
};

constexpr Material BLACK
{
	glm::vec3(0.4f, 0.4f, 0.4f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	.25,
};

constexpr Material YELLOW
{
	glm::vec3(0.4f, 0.4f, 0.0f),
	glm::vec3(0.5f, 0.5f, 0.0f),
	glm::vec3(0.60f, 0.60f, 0.50f),
	.25f,
};

constexpr Material CYAN
{
	glm::vec3(0.0f, 0.1f, 0.06f),
	glm::vec3(0.0f, 0.5098f, 0.5098f),
	glm::vec3(0.5019f, 0.5019f, 0.5019),
	.25f,
};

constexpr Material ORANGE
{
	glm::vec3(1.f, 0.5f, 0.0f),
	glm::vec3(1.f, 0.5f, 0.0f),
	glm::vec3(1.f, 0.5f, 0.0f),
	.25,
};

constexpr Material MAGENTA
{
	glm::vec3(1.f, 0.0f, 1.0f),
	glm::vec3(1.f, 0.0f, 1.0f),
	glm::vec3(1.f, 0.0f, 1.0f),
	.25,
};
