#pragma once
#include <d3d12.h>
#include <vector>

struct Vertex
{
	float pos[3];
	float normal[3];
	float uv[2];
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	D3D12_PRIMITIVE_TOPOLOGY topology;
};
