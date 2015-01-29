#include <GL/glew.h>
#include "CModelData.hpp"
#include <glm/gtc/matrix_transform.hpp>

CModelData::CModelData()
    : m_ibosIndexed(false),
      m_ibosLoaded(false)
{
}

CModelData::~CModelData()
{
    if (m_ibosLoaded)
    {
        glDeleteBuffers(1, &m_vertexObj);
        glDeleteBuffers(1, &m_normalObj);
        glDeleteBuffers(2, m_clrObjs);
        glDeleteBuffers(7, m_texCoordObjs);

        for (atUint32 ibo = 0; ibo < m_ibos.size(); ibo++)
            glDeleteBuffers(1, &m_ibos[ibo].bufferID);
    }
}

void CModelData::exportModel(std::ofstream& of, atUint32& vertexOff, atUint32& normalOff, atUint32& texOff)
{
    glm::mat4 rotMat = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(-1, 0, 0));

    of << "# vertex positions" << std::endl;
    for (glm::vec3 v : m_vertices)
    {
        glm::vec3 rv(glm::vec4(rotMat * glm::vec4(v, 1)));
        of << "v " << rv.x << " " << rv.y << " " << rv.z << std::endl;
    }
    of << std::endl << std::endl;

    of << "# texture coordinates" << std::endl;
    for (glm::vec2 uv : m_texCoords)
    {
        uv /= 1.f;
        of << "vt " << uv.x << " " << uv.y << std::endl;
    }
    of << std::endl << std::endl;

    of << "# vertex normals" << std::endl;
    for (glm::vec3 n : m_normals)
    {
        of << "vn " << n.y << " " << n.x << " " << n.z << std::endl;
    }
    of << std::endl << std::endl;

    int meshIndex = 0;

    for (CMesh m : m_meshes)
    {
        of << "o mesh_" << meshIndex << std::endl;
        meshIndex++;
        //of << "s 1" << std::endl;

        for (CPrimitive s : m.m_primitives)
        {
            switch (s.primitive)
            {
                case EPrimitive::Lines:
                {
                    for (atUint32 i = 1 ; i < s.indices.size(); i += 2)
                    {
                        VertexDescriptor& f0 = s.indices[i - 1];
                        VertexDescriptor& f1 = s.indices[i - 0];

                        of << "l "
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff
                           << " " << f1.position + vertexOff << "/" << f1.texCoord[0] + texOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::LineStrip:
                {
                    for (atUint32 i = 1 ; i < s.indices.size(); ++i)
                    {
                        VertexDescriptor& f0 = s.indices[i - 1];
                        VertexDescriptor& f1 = s.indices[i - 0];

                        of << "l"
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff
                           << " " << f1.position + vertexOff << "/" << f1.texCoord[0] + texOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::Triangles:
                {
                    for (atUint32 i = 2 ; i < s.indices.size(); i += 3)
                    {
                        VertexDescriptor& f0 = s.indices[i - 2];
                        VertexDescriptor& f1 = s.indices[i - 1];
                        VertexDescriptor& f2 = s.indices[i - 0];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << f1.texCoord[0] + texOff << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << f2.texCoord[0] + texOff << "/" << f2.normal + normalOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::TriangleStrip:
                {
                    for (atUint32 i = 2 ; i < s.indices.size(); ++i)
                    {
                        VertexDescriptor& f0 = s.indices[i - 2];
                        VertexDescriptor& f1 = s.indices[i - (1 - (i & 1))];
                        VertexDescriptor& f2 = s.indices[i - ((i & 1) - 0)];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << f1.texCoord[0] + texOff << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << f2.texCoord[0] + texOff << "/" << f2.normal + normalOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::TriangleFan:
                {
                    VertexDescriptor& f0 = s.indices[0];
                    for (atUint32 i = 2 ; i < s.indices.size(); ++i)
                    {
                        VertexDescriptor& f1 = s.indices[i - 1];
                        VertexDescriptor& f2 = s.indices[i - 0];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << f1.texCoord[0] + texOff << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << f2.texCoord[0] + texOff << "/" << f2.normal + normalOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::Quads:
                {
                    for (atUint32 i = 3 ; i < s.indices.size(); i += 4)
                    {
                        VertexDescriptor& f0 = s.indices[i - 3];
                        VertexDescriptor& f1 = s.indices[i - 2];
                        VertexDescriptor& f2 = s.indices[i - 1];
                        VertexDescriptor& f3 = s.indices[i - 0];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << f1.texCoord[0] + texOff << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << f2.texCoord[0] + texOff << "/" << f2.normal + normalOff << std::endl;
                        of << "f"
                           << " " << f0.position + vertexOff << "/" << f0.texCoord[0] + texOff << "/" << f0.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << f2.texCoord[0] + texOff << "/" << f2.normal + normalOff
                           << " " << f3.position + vertexOff << "/" << f3.texCoord[0] + texOff << "/" << f3.normal + normalOff << std::endl;
                    }
                }
                    break;
                default:
                {
                    for (atUint32 i = 0 ; i < s.indices.size(); ++i)
                    {
                        VertexDescriptor& f0 = s.indices[i];
                        of << "f " << f0.position + vertexOff << " " << f0.position + vertexOff << " " << f0.position + vertexOff << std::endl;
                    }
                }
                    break;
            }
        }
    }

    vertexOff += m_vertices.size();
    normalOff += m_normals.size();
    texOff    += m_texCoords.size();
}

void CModelData::draw(CMaterialSet& materialSet)
{
    loadIbos();
    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_COLOR_MATERIAL);
    glPrimitiveRestartIndex(0xFFFFFFFF);

    //
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexObj);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalObj);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_clrObjs[0]);
    glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
    //
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, m_clrObjs[1]);
    glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
    //
    for (int i = 0; i < 7; i++)
    {
        glEnableVertexAttribArray(i+4);
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordObjs[i]);
        glVertexAttribPointer(i+4, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    for (std::pair<atUint32, std::vector<SIndexBufferObject> > iboPair : m_indexedIbos)
    {
        CMaterial& mat = materialSet.material(iboPair.first);
        //        if ((mat.isTransparent() && firstPass) || (!mat.isTransparent() && !firstPass))
        //            continue;

        //        if (!mat.bind())
        //            continue;

        //        atUint32 projectionLoc = mat.program()->uniformLocation("projectionMatrix");
        //        atUint32 modelLoc = mat.program()->uniformLocation("modelMatrix");
        //        atUint32 viewLoc = mat.program()->uniformLocation("viewMatrix");
        //        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &GLViewer::instance()->projectionMatrix()[0][0]);
        //        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &GLViewer::instance()->modelMatrix()[0][0]);
        //        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &GLViewer::instance()->viewMatrix()[0][0]);
        /*
        for (atUint32 i = 0; i < mat.textureIndices().size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, GLViewer::instance()->texture(mc.texture(mat.TextureIndices[i]), this));
        }
*/

        for (SIndexBufferObject ibo : iboPair.second)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.bufferID);
            glDrawElements(ibo.primitiveType, ibo.indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);
        }
    }

    for (int i = 0; i <= 10; i++)
        glDisableVertexAttribArray(i);
}

void CModelData::indexIBOs(CMaterialSet& materialSet)
{
    if (m_ibosIndexed)
        return;

    m_ibosIndexed = true;

    for (CMesh& mesh : m_meshes)
    {

        CMaterial material = materialSet.material(mesh.m_materialID);

        // If it's not drawn don't even bother indexing it
        //            if (material.materialFlags() & 0x200)
        //                continue;

        atUint32 vertStartIndex = m_vertexBuffer.size();
        atUint32 iboStartIndex = m_ibos.size();

        for (CPrimitive& primitive : mesh.m_primitives)
        {
            atUint32 type;
            atUint32 stepSize = 1;
            switch(primitive.primitive)
            {
                case EPrimitive::Quads:
                    type = GL_QUADS;
                    stepSize = 4;
                    break;
                case EPrimitive::Triangles:
                    type = GL_TRIANGLES;
                    stepSize = 3;
                    break;
                case EPrimitive::TriangleStrip:
                    type = GL_TRIANGLE_STRIP;
                    stepSize = 3;
                    break;
                case EPrimitive::TriangleFan:
                    type = GL_TRIANGLE_FAN;
                    stepSize = 3;
                    break;
                case EPrimitive::Lines:
                    type = GL_LINES;
                    stepSize = 2;
                    break;
                case EPrimitive::LineStrip:
                    type = GL_LINE_STRIP;
                    stepSize = 2;
                    break;
                case EPrimitive::Points:
                    type = GL_POINTS;
                    stepSize = 1;
                    break;
                default:
                    continue;
            }

            atUint32 iboID = getIbo(type, mesh.m_materialID, iboStartIndex);

            m_ibos[iboID].pos = mesh.m_pivot;

            for (atUint32 v = 0; v < primitive.indices.size(); v++)
            {
                switch(stepSize)
                {
                    case 4:
                        if (v < primitive.indices.size())
                        {
                            indexVert(material, primitive.indices[v], mesh, iboID, vertStartIndex);
                            v++;
                        }
                    case 3:
                        if (v < primitive.indices.size())
                        {
                            indexVert(material, primitive.indices[v], mesh, iboID, vertStartIndex);
                            v++;
                        }
                    case 2:
                        if (v < primitive.indices.size())
                        {
                            indexVert(material, primitive.indices[v], mesh, iboID, vertStartIndex);
                            v++;
                        }
                    case 1:
                        if (v < primitive.indices.size())
                            indexVert(material, primitive.indices[v], mesh, iboID, vertStartIndex);
                        break;
                }
            }
            m_ibos[iboID].indexBuffer.push_back(0xFFFFFFFF);
        }
    }

    atUint32 size = m_vertexBuffer.size();
    m_posBuf.resize(size);
    m_normBuf.resize(size);
    for (atUint32 i = 0; i < 2; i++)
        m_clrBufs[i].resize(size);

    for (int i = 0; i < 7; i++)
        m_texCoordBuf[i].resize(size);

    for (atUint32 v = 0; v < m_vertexBuffer.size(); v++)
    {
        m_posBuf[v]         = m_vertexBuffer[v].pos;
        m_normBuf[v]        = m_vertexBuffer[v].norm;
        for (atUint32 c = 0; c < 2; c++)
            m_clrBufs[c][v]  = m_vertexBuffer[v].color[c];

        for (atUint32 t = 0; t < 7; t++)
            m_texCoordBuf[t][v] = m_vertexBuffer[v].texCoords[t];
    }
}

atUint32 CModelData::getIbo(atUint32 prim, atUint32 matId, atUint32 start)
{
    if (!m_ibos.empty())
    {
        for (atUint32 i = start; i < m_ibos.size(); i++)
        {
            SIndexBufferObject* ibo = &m_ibos[i];

            if (ibo->primitiveType == prim && ibo->materialId == matId)
                return i;
        }
    }

    m_ibos.push_back(SIndexBufferObject());
    m_ibos.back().primitiveType = prim;
    m_ibos.back().materialId = matId;
    return m_ibos.size() - 1;
}

void CModelData::indexVert(CMaterial& material, VertexDescriptor& desc, CMesh& mesh, atUint32 iboID, atUint32 vertStartIndex)
{
    SVertex vert;
    if (material.hasPosition())
        vert.pos = m_vertices[desc.position];
    if (material.hasNormal())
    {
        vert.norm = m_normals[desc.normal];
    }

    for (atUint32 c = 0; c < 2; c++)
    {
        if (material.hasColor(c))
            vert.color[c] = m_colors[desc.clr[0]];
    }

    for (atUint32 t = 0; t < 7; t++)
    {
        if (material.hasUV(t))
        {
            glm::vec2 tex;

            if (((material.materialFlags() & 0x6000) & 3) && t == 0)
                tex = m_lightmapCoords[desc.texCoord[t]];
            else
                tex = m_texCoords[desc.texCoord[t]];

            vert.texCoords[t] = tex;
        }
    }

    bool unique = true;
    atUint32 index;

    if (!m_vertexBuffer.empty())
    {
        for (atUint32 vi = vertStartIndex; vi < m_vertexBuffer.size(); vi++)
        {
            if (m_vertexBuffer[vi] == vert)
            {
                index = vi;
                unique = false;
                break;
            }
        }
    }

    if (unique)
    {
        m_vertexBuffer.push_back(vert);
        index = m_vertexBuffer.size() - 1;
    }
    m_ibos[iboID].indexBuffer.push_back(index);
}

void CModelData::loadIbos()
{
    if (m_ibosLoaded)
        return;

    m_ibosLoaded = true;
    glGenBuffers(1, &m_vertexObj);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexObj);
    glBufferData(GL_ARRAY_BUFFER, m_posBuf.size() * sizeof(glm::vec3), &m_posBuf[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_normalObj);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalObj);
    glBufferData(GL_ARRAY_BUFFER, m_normBuf.size() * sizeof(glm::vec3), &m_normBuf[0], GL_STATIC_DRAW);

    glGenBuffers(2, m_clrObjs);
    for (atUint32 c = 0; c < 2; c++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_clrObjs[c]);
        glBufferData(GL_ARRAY_BUFFER, m_clrBufs[c].size() * sizeof(atUint32), &m_clrBufs[c][0], GL_STATIC_DRAW);
    }

    glGenBuffers(7, m_texCoordObjs);

    for (int i = 0; i < 7; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordObjs[i]);
        glBufferData(GL_ARRAY_BUFFER, m_texCoordBuf[i].size() * sizeof(glm::vec2), &m_texCoordBuf[i][0], GL_STATIC_DRAW);
    }

    for (atUint32 ibo = 0; ibo < m_ibos.size(); ibo++)
    {
        glGenBuffers(1, &m_ibos[ibo].bufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibos[ibo].bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ibos[ibo].indexBuffer.size() * sizeof(atUint32), &m_ibos[ibo].indexBuffer[0], GL_STATIC_DRAW);
        m_indexedIbos[m_ibos[ibo].materialId].push_back(m_ibos[ibo]);
    }
}
