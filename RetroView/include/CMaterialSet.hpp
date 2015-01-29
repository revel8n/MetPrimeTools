#ifndef MATERIALCONTAINER_HPP
#define MATERIALCONTAINER_HPP

#include "CMaterial.hpp"
#include <vector>

class CMaterialSet
{
public:
    CMaterialSet();
    ~CMaterialSet();

    atUint32 textureID(const atUint32& index) const;

    CMaterial& material(const atUint32& index);
    CMaterial& material(const atUint32& index) const;
private:
    friend class CMaterialReader;

    std::vector<atUint32> m_textureIds;
    std::vector<CMaterial> m_materials;
};

#endif // MATERIALCONTAINER_HPP
