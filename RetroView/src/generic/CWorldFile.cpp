#include "generic/CWorldFile.hpp"
#include "generic/CStringTable.hpp"
#include "core/CResourceManager.hpp"
#include "core/IRenderableModel.hpp"

CWorldFile::CWorldFile()
{

}

CWorldFile::~CWorldFile()
{

}

std::string CWorldFile::areaName(const CUniqueID& assetId, CPakFile* pak)
{
    std::vector<SWorldArea>::iterator iter = std::find_if(m_areas.begin(), m_areas.end(),
                                                            [&assetId](const SWorldArea r)->bool{return r.mreaID == assetId; });
    if (iter == m_areas.end())
        return std::string();

    SWorldArea area = (SWorldArea)(*iter);

    if (area.nameID != CUniqueID::InvalidAsset)
    {
        CStringTable* table;
        if (pak != nullptr)
            table = dynamic_cast<CStringTable*>(CResourceManager::instance()->loadResourceFromPak(pak, area.nameID, "STRG"));
        else
            table = dynamic_cast<CStringTable*>(CResourceManager::instance()->loadResource(area.nameID, "STRG"));

        if (table)
        {
            std::string ret = table->string();
            table->destroy();
            return ret;
        }
    }

    if (area.internalName != std::string())
        return std::string("!!") + area.internalName;

    return std::string();
}

IRenderableModel* CWorldFile::skyboxModel()
{
    return dynamic_cast<IRenderableModel*>(CResourceManager::instance()->loadResourceFromPak(m_source, m_skyboxID, "CMDL"));
}

