#pragma once
#include "data_structures/Array.hpp"

template<typename T>
struct Material
{
	T diffusionCoeff;
};

template<typename T>
class MaterialManager
{
private:
	Array<Material<T>> m_materials;
public:
	void addMaterial(const Material<T>& material);
	const Material<T>& getMaterial(int i) const;
};

template<typename T>
inline void MaterialManager<T>::addMaterial(const Material<T>& material)
{
	m_materials.pushBack(material);
}

template<typename T>
inline const Material<T>& MaterialManager<T>::getMaterial(int i) const
{
	return m_materials[i];
}