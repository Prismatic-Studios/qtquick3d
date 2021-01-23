/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Quick 3D.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qssgrendergeometry_p.h"
#include "qssgrendermesh_p.h"
#include "resourcemanager/qssgrenderbuffermanager_p.h"

QSSGRenderGeometry::QSSGRenderGeometry()
    : QSSGRenderGraphObject(QSSGRenderGraphObject::Type::Geometry)
{
    Q_STATIC_ASSERT(int(Triangles) == int(QSSGMeshUtilities::MeshData::Triangles));
    Q_STATIC_ASSERT(int(Attribute::TexCoord1Semantic) == int(QSSGMeshUtilities::MeshData::Attribute::TexCoord1Semantic));
    Q_STATIC_ASSERT(int(Attribute::F32Type) == int(QSSGMeshUtilities::MeshData::Attribute::F32Type));
}

QSSGRenderGeometry::~QSSGRenderGeometry()
{
}

const QByteArray &QSSGRenderGeometry::vertexBuffer() const
{
    return m_meshData.m_vertexBuffer;
}

QByteArray &QSSGRenderGeometry::vertexBuffer()
{
    return m_meshData.m_vertexBuffer;
}

const QByteArray &QSSGRenderGeometry::indexBuffer() const
{
    return m_meshData.m_indexBuffer;
}

QByteArray &QSSGRenderGeometry::indexBuffer()
{
    return m_meshData.m_indexBuffer;
}

int QSSGRenderGeometry::attributeCount() const
{
    return m_meshData.m_attributeCount;
}

QVector3D QSSGRenderGeometry::boundsMin() const
{
    return m_bounds.minimum;
}

QVector3D QSSGRenderGeometry::boundsMax() const
{
    return m_bounds.maximum;
}

int QSSGRenderGeometry::stride() const
{
    return m_meshData.m_stride;
}

QSSGRenderGeometry::PrimitiveType QSSGRenderGeometry::primitiveType() const
{
    return static_cast<QSSGRenderGeometry::PrimitiveType>(m_meshData.m_primitiveType);
}

QSSGRenderGeometry::Attribute QSSGRenderGeometry::attribute(int idx) const
{
    Attribute attr;
    const auto &mattr = m_meshData.m_attributes[idx];
    attr.offset = mattr.offset;
    attr.semantic = static_cast<QSSGRenderGeometry::Attribute::Semantic>(mattr.semantic);
    attr.componentType
            = static_cast<QSSGRenderGeometry::Attribute::ComponentType>(mattr.componentType);
    return attr;
}

void QSSGRenderGeometry::addAttribute(Attribute::Semantic semantic, int offset,
                                      Attribute::ComponentType componentType)
{
    Attribute attr;
    attr.semantic = semantic;
    attr.offset = offset;
    attr.componentType = componentType;
    addAttribute(attr);
}

void QSSGRenderGeometry::addAttribute(const Attribute &att)
{
    int index = m_meshData.m_attributeCount;
    m_meshData.m_attributes[index].semantic
            = static_cast<QSSGMeshUtilities::MeshData::Attribute::Semantic>(att.semantic);
    m_meshData.m_attributes[index].offset = att.offset;
    m_meshData.m_attributes[index].componentType
            = static_cast<QSSGMeshUtilities::MeshData::Attribute::ComponentType>(att.componentType);
    ++m_meshData.m_attributeCount;
    m_dirty = true;
}

void QSSGRenderGeometry::setStride(int stride)
{
    m_meshData.m_stride = stride;
    m_dirty = true;
}

void QSSGRenderGeometry::setPrimitiveType(PrimitiveType type)
{
    m_meshData.m_primitiveType
            = static_cast<QSSGMeshUtilities::MeshData::PrimitiveType>(type);
    m_dirty = true;
}

void QSSGRenderGeometry::setBounds(const QVector3D &min, const QVector3D &max)
{
    m_bounds = QSSGBounds3(min, max);
    m_dirty = true;
}

void QSSGRenderGeometry::clear()
{
    m_meshData.clear();
    m_bounds.setEmpty();
    m_dirty = true;
}

void QSSGRenderGeometry::clearAttributes()
{
    m_meshData.m_attributeCount = 0;
}

void QSSGRenderGeometry::setVertexData(const QByteArray &data)
{
    m_meshData.m_vertexBuffer = data;
    m_dirty = true;
}

void QSSGRenderGeometry::setIndexData(const QByteArray &data)
{
    m_meshData.m_indexBuffer = data;
    m_dirty = true;
}

QSSGRenderMesh *QSSGRenderGeometry::createOrUpdate(const QSSGRef<QSSGBufferManager> &bufferManager)
{
    if (!m_meshBuilder)
        m_meshBuilder = QSSGMeshUtilities::QSSGMeshBuilder::createMeshBuilder();

    if (m_dirty) {
        QSSGRenderMesh *renderMesh = nullptr;
        QString error;
        if (m_meshBuilder->setData(m_meshData, error, m_bounds)) {
            QSSGMeshUtilities::Mesh &mesh = m_meshBuilder->getMesh();
            renderMesh = bufferManager->loadCustomMesh(this, &mesh, true);
        } else {
            qWarning("Mesh building failed: %s", qPrintable(error));
        }
        m_meshBuilder->reset();
        m_dirty = false;
        return renderMesh;
    }

    return bufferManager->getMesh(this);
}
