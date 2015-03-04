#include <GL/glew.h>
#include "models/CModelFile.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"
#include "ui/CGLViewer.hpp"

#include <dae.h>
#include <dae/daeUtils.h>
#include <dom/domCOLLADA.h>

CModelFile::CModelFile()
{
    setAmbient(.5f, .5f, .5f);
}

CModelFile::~CModelFile()
{

}

void CModelFile::draw()
{
    CMaterialCache::instance()->setAmbientOnMaterials(currentMaterialSet().materials(), m_ambient);

    glm::mat4 model = CGLViewer::instance()->modelMatrix();
    CModelData::preDraw(currentMaterialSet());
    CModelData::drawIbos(false, currentMaterialSet(), model);
    CModelData::drawIbos(true,  currentMaterialSet(), model);
    CModelData::doneDraw();
}

void CModelFile::drawBoundingBox()
{
    CModelData::drawBoundingBoxes();
}

void CModelFile::updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj)
{
    for (atUint32 materialIdx : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(materialIdx);
        mat.setViewMatrix(view);
        mat.setProjectionMatrix(proj);
    }
}

CMaterialSet& CModelFile::currentMaterialSet()
{
    return m_materialSets[0];
}

CMaterialSet& CModelFile::currentMaterialSet() const
{
    return currentMaterialSet();
}

bool CModelFile::canExport() const
{
    return true;
}

template<typename T>
daeTArray<T> rawArrayToDaeArray(T rawArray[], size_t count)
{
    daeTArray<T> result;
    for (size_t i = 0; i < count; i++)
        result.append(rawArray[i]);

    return result;
}

std::string makeUriRef(const std::string& id) {
    return std::string("#") + id;
}

void addSource(daeElement* mesh, const std::string& srcID, const std::string& paramNames, domFloat values[], int valueCount)
{
    daeElement* src = mesh->add("source");
    src->setAttribute("id", srcID.c_str());
    domFloat_array* fa = daeSafeCast<domFloat_array>(src->add("float_array"));

    fa->setId((src->getAttribute("id") + "-array").c_str());
    fa->setCount(valueCount);
    fa->getValue() = rawArrayToDaeArray(values, valueCount);

    domAccessor* acc = daeSafeCast<domAccessor>(src->add("technique_common accessor"));
    acc->setSource(makeUriRef(fa->getId()).c_str());

    std::list<std::string> params;
}

void addInput(daeElement* triangles, const std::string& semantic, const std::string& srcID, int offset)
{
    domInputLocalOffset* input = daeSafeCast<domInputLocalOffset>(triangles->add("input"));
    input->setSemantic(semantic.c_str());
    input->setOffset(offset);
    input->setSource(makeUriRef(srcID).c_str());
    if (semantic == "TEXCOORD")
        input->setSet(0);
}

void CModelFile::exportToObj(const std::string& filepath)
{
    DAE dae;
    daeElement* root = dae.add(filepath + ".dae");
    daeElement* asset = root->add("asset");
    daeElement* contrib = asset->add("contributor");
    contrib->add("author")->setCharData("RetroView User");
    contrib->add("authoring_tool")->setCharData("RetroView");
    asset->add("up_axis")->setCharData("Z_UP");

    daeElement* geomLib = root->add("library_geometries");
    daeElement* geom = geomLib->add("geometry");
    std::string geomID = "cmdlGeom";
    geom->setAttribute("id", geomID.c_str());
    daeElement* mesh = geom->add("mesh");
    dae.writeAll();

    atUint32 vertexOff = 1;
    atUint32 normalOff = 1;
    atUint32 texCoordOff = 1;
    std::ofstream out(filepath);
    CModelData::exportModel(out, vertexOff, normalOff, texCoordOff, currentMaterialSet());
    out.flush();
    out.close();
}

SBoundingBox& CModelFile::boundingBox()
{
    static SBoundingBox bbox{m_boundingBox};

    if (bbox != SBoundingBox{m_boundingBox})
        bbox =  SBoundingBox{m_boundingBox};

    return bbox;
}
