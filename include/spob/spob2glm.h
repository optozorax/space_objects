#pragma once

#include <spob/spob.h>
#include <glm/glm.hpp>

namespace spob
{
	/*
		crd.from(p) == getFromMatrix(crd) * p
		crd.to(p) == getToMatrix(crd) * p

		invert(crd).to(p) == glm::inverse(getToMatrix(crd)) * p

		combine(crd1, crd2).to(p) == getToMatrix(crd1) * getToMatrix(crd2) * p
	*/

	glm::mat3 getFromMatrix(const crd2& crd);
	glm::mat3 getToMatrix(const crd2& crd);

	glm::mat4 getFromMatrix(const crd3& crd);
	glm::mat4 getToMatrix(const crd3& crd);

	crd2 getToCrd(const glm::mat3& m);
	crd2 getFromCrd(const glm::mat3& m);

	crd3 getToCrd(const glm::mat4& m);
	crd3 getFromCrd(const glm::mat4& m);

	glm::vec2 spob2glm(const vec2& a);
	glm::vec3 spob2glm(const vec3& a);

	vec2 glm2spob(const glm::vec2& a);
	vec3 glm2spob(const glm::vec3& a);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
inline glm::mat3 getFromMatrix(const crd2& crd) {
	glm::mat3 result;
	result[0] = glm::vec3(crd.i.x, crd.j.x, crd.pos.x);
	result[1] = glm::vec3(crd.i.y, crd.j.y, crd.pos.y);
	result[2] = glm::vec3(0, 0, 1);
	return glm::transpose(result);
}

//----------------------------------------------------------------------------
inline glm::mat3 getToMatrix(const crd2& crd) {
	return glm::inverse(getFromMatrix(crd));
}

//-----------------------------------------------------------------------------
inline glm::mat4 getFromMatrix(const crd3& crd) {
	glm::mat4 result;
	result[0] = glm::vec4(crd.i.x, crd.j.x, crd.k.x, crd.pos.x);
	result[1] = glm::vec4(crd.i.y, crd.j.y, crd.k.y, crd.pos.y);
	result[2] = glm::vec4(crd.i.z, crd.j.z, crd.k.z, crd.pos.z);
	result[3] = glm::vec4(0, 0, 0, 1);
	return glm::transpose(result);
}

//-----------------------------------------------------------------------------
inline glm::mat4 getToMatrix(const crd3& crd) {
	return glm::inverse(getFromMatrix(crd));
}

//-----------------------------------------------------------------------------
inline crd2 getToCrd(const glm::mat3& m) {
	return crd2(vec2(m[0][0], m[0][1]), vec2(m[1][0], m[1][1]), vec2(m[2][0], m[2][1]));
}

//-----------------------------------------------------------------------------
inline crd2 getFromCrd(const glm::mat3& m) {
	return getToCrd(glm::inverse(m));
}

//-----------------------------------------------------------------------------
inline crd3 getToCrd(const glm::mat4& m) {
	return {};
}

//-----------------------------------------------------------------------------
inline crd3 getFromCrd(const glm::mat4& m) {
	return {};
}

//-----------------------------------------------------------------------------
inline glm::vec2 spob2glm(const vec2& a) {
	return glm::vec2(a.x, a.y);
}

//-----------------------------------------------------------------------------
inline glm::vec3 spob2glm(const vec3& a) {
	return glm::vec3(a.x, a.y, a.z);
}

//-----------------------------------------------------------------------------
inline vec2 glm2spob(const glm::vec2& a) {
	return vec2(a.x, a.y);
}

//-----------------------------------------------------------------------------
inline vec3 glm2spob(const glm::vec3& a) {
	return vec3(a.x, a.y, a.z);
}

};