#include "core/CResourceManager.hpp"
#include "ui/CPakTreeWidget.hpp"
#include "core/GXCommon.hpp"

#include <CPakFileReader.hpp>
#include <iostream>
#include <algorithm>
#include <Athena/Utility.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QMessageBox>
#include <QLabel>

namespace
{
struct concrete_ResourceManager : public CResourceManager
{
};

}

CResourceManager::CResourceManager()
{
    std::cout << "ResourceManager created" << std::endl;
}

CResourceManager::~CResourceManager()
{
    for (std::pair<CUniqueID, IResource*> res : m_cachedResources)
        delete res.second;

    for (CPakFile* pak : m_pakFiles)
        delete pak;

    m_pakFiles.clear();

    std::cout << "ResourceManager destroyed" << std::endl;
}

void CResourceManager::loadPak(std::string filepath)
{

    std::vector<CPakFile*>::iterator iter = std::find_if(m_pakFiles.begin(), m_pakFiles.end(),
                                                            [&filepath](const CPakFile* r)->bool{return r->filename() == filepath; });

    if (iter != m_pakFiles.end())
        return;

    CPakFile* pak = nullptr;
    try
    {
        CPakFileReader reader(filepath);
        pak = reader.read();
        pak->removeDuplicates();
        m_pakFiles.push_back(pak);
        CPakTreeWidget* widget = new CPakTreeWidget(pak);

        m_pakTreeWidgets.push_back(widget);

        emit newPak(widget);

        std::cout << " loaded" << std::endl;
    }
    catch(...)
    {
        delete pak;
        std::cout << " failed to load" << std::endl;
    }
}

void CResourceManager::clear()
{
    for (std::pair<CUniqueID, IResource*> res : m_cachedResources)
        delete res.second;
    m_cachedResources.clear();
    m_failedAssets.clear();
}

void CResourceManager::initialize(const std::string& baseDirectory)
{
    m_baseDirectory = baseDirectory;

    std::cout << "ResourceManager initialized @ " << m_baseDirectory << std::endl;
    std::cout << "Searching for paks..." << std::endl;

    DIR* dir = opendir(m_baseDirectory.c_str());
    if (dir)
    {
        struct dirent * dp;

        while((dp = readdir(dir)) != NULL)
        {
            struct stat64 st;
            std::string filepath = m_baseDirectory + "/" + std::string(dp->d_name);
            stat64(filepath.c_str(), &st);
            if(S_ISREG(st.st_mode))
            {
                std::string filename = dp->d_name;
                std::string ext = filename.substr(filename.rfind('.') + 1, filename.size());
                Athena::utility::tolower(ext);
                if (!ext.compare("pak"))
                {
                    std::cout << "Found pak " << filename << " ...";
                    loadPak(filepath);
                }
            }
        }
        closedir(dir);
    }
}

IResource* CResourceManager::loadResource(const CUniqueID& assetID, const std::string& type)
{
    if (assetID == CUniqueID::InvalidAsset)
        return nullptr;

    std::vector<CUniqueID>::iterator failedIter = std::find(m_failedAssets.begin(), m_failedAssets.end(), assetID);
    if (failedIter != m_failedAssets.end())
        return nullptr;

    CachedResourceIterator iter = m_cachedResources.find(assetID);
    if (iter != m_cachedResources.end())
        return iter->second;

    for (CPakFile* pak : m_pakFiles)
    {
        IResource* ret = loadResourceFromPak(pak, assetID, type);
        if (ret)
            return ret;
    }

    return nullptr;
}

IResource* CResourceManager::loadResourceFromPak(CPakFile* pak, const CUniqueID& assetID, const std::string& type)
{
    if (assetID == CUniqueID::InvalidAsset)
        return nullptr;

    std::vector<CUniqueID>::iterator failedIter = std::find(m_failedAssets.begin(), m_failedAssets.end(), assetID);
    if (failedIter != m_failedAssets.end())
        return nullptr;


    IResource* res = m_cachedResources[assetID];
    if (res != nullptr)
        return res;

    std::vector<SPakResource> pakResources;
    if (!type.empty())
        pakResources = pak->resourcesByType(type);
    else
        pakResources = pak->resources();

    std::vector<SPakResource>::iterator iter = std::find_if(pakResources.begin(), pakResources.end(),
                                                            [&assetID](const SPakResource& r)->bool{return r.id == assetID; });
    if (iter != pakResources.end())
        return attemptLoad(*iter, pak);

    return nullptr;
}

void CResourceManager::destroyResource(IResource* res)
{
    m_cachedResources.erase(res->assetId());
    delete res;
}

std::shared_ptr<CResourceManager> CResourceManager::instance()
{
    static std::shared_ptr<CResourceManager> instance = std::make_shared<concrete_ResourceManager>();

    return instance;
}

std::vector<CPakTreeWidget*> CResourceManager::pakWidgets() const
{
    return m_pakTreeWidgets;
}

IResource* CResourceManager::attemptLoad(SPakResource res, CPakFile* pak)
{
    if (m_loaders.find(res.tag) == m_loaders.end())
        return nullptr;

    atUint8* data = pak->loadData(res.id, res.tag.toString());
    if (data == nullptr)
        return nullptr;

    IResource* ret = nullptr;
    try
    {
        ret = m_loaders[res.tag].byData(data, res.size);
        if (ret)
        {
            ret->m_assetType = res.tag;
            ret->m_assetID = res.id;
            ret->m_source = pak;
            m_cachedResources[res.id] = ret;
        }
    }
    catch(const Athena::error::Exception& e)
    {
        std::cout << e.file() << " " << e.message() << std::endl;
        delete ret;
        ret = nullptr;
        m_failedAssets.push_back(res.id);
    }

    return ret;
}

void CResourceManager::registerLoader(const CFourCC& tag, ResourceDataLoaderCallback byData)
{
    if (m_loaders.find(tag) != m_loaders.end())
    {
        std::cout << "Already had a loader for " << tag.toString() << " registered, ignored registration attempt" << std::endl;
        return;
    }

    std::cout << "Loader for resource " << tag.toString() << " registered" << std::endl;
    ResourceLoaderDesc desc;
    desc.tag = tag;
    desc.byData = byData;
    m_loaders[tag] = desc;
}

SResourceLoaderRegistrator::SResourceLoaderRegistrator(const CFourCC& tag, ResourceDataLoaderCallback byData)
{
    CResourceManager::instance().get()->registerLoader(tag, byData);
}


