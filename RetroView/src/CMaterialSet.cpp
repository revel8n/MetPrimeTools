#include "CMaterialSet.hpp"

CMaterialSet::CMaterialSet()
{
}

CMaterialSet::~CMaterialSet()
{
}

atUint32 CMaterialSet::textureID(const atUint32& index) const
{
    if (index > m_textureIds.size())
        return 0;

    return m_textureIds[index];
}

CMaterial& CMaterialSet::material(const atUint32& index)
{
    static CMaterial invalid;
    if (index > m_materials.size())
        return invalid;

    return m_materials[index];
}

CMaterial& CMaterialSet::material(const atUint32& index) const
{
    return material(index);
}

