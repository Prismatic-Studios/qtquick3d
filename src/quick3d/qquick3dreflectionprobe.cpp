/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#include "qquick3dreflectionprobe_p.h"
#include "qquick3dnode_p_p.h"

#include <QtQuick3DRuntimeRender/private/qssgrenderreflectionprobe_p.h>
#include <QtQuick3DUtils/private/qssgutils_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype ReflectionProbe
    \inherits Node
    \inqmlmodule QtQuick3D
    \brief Defines a reflection probe in the scene.

    A reflection probe is used to provide reflections of the current scene to the objects. The probe
    provides properties to the runtime which are then used to render the scene to a cube map. The cube map
    is then used as the reflection information for the reflecting objects.

    \sa {Qt Quick 3D - Reflection Probes Example}
*/

QQuick3DReflectionProbe::QQuick3DReflectionProbe(QQuick3DNode *parent)
    : QQuick3DNode(*(new QQuick3DNodePrivate(QQuick3DNodePrivate::Type::ReflectionProbe)), parent)
{
}

/*!
    \qmlproperty ReflectionQuality ReflectionProbe::quality

    Quality determines the resolution of the cube map.
    The qualities are \c {ReflectionQuality.VeryLow}, \c {ReflectionQuality.Low},
    \c {ReflectionQuality.Medium}, \c {ReflectionQuality.High} and \c {ReflectionQuality.VeryHigh}
    corresponding to 128x128, 256x256, 512x512, 1024x1024 and 2048x2048 resolutions.
*/
QQuick3DReflectionProbe::ReflectionQuality QQuick3DReflectionProbe::quality() const
{
    return m_quality;
}

/*!
    \qmlproperty Color ReflectionProbe::clearColor

    Clear color is the color used to clear the cube map prior rendering the scene.
*/
QColor QQuick3DReflectionProbe::clearColor() const
{
    return m_clearColor;
}

/*!
    \qmlproperty ReflectionRefreshMode ReflectionProbe::refreshMode

    Refresh mode tells the runtime how many times the cube map is rendered.
    The settings are \c {ReflectionRefreshMode.FirstFrame} and \c {ReflectionRefreshMode.EveryFrame}.
    With \c {ReflectionRefreshMode.FirstFrame} the scene is rendered once and with
    \c {ReflectionRefreshMode.EveryFrame} the scene is rendered every frame.

    \note Use \c {ReflectionRefreshMode.FirstFrame} for improved performance.
*/
QQuick3DReflectionProbe::ReflectionRefreshMode QQuick3DReflectionProbe::refreshMode() const
{
    return m_refreshMode;
}

/*!
    \qmlproperty ReflectionTimeSlicing ReflectionProbe::timeSlicing

    Time slicing determines how the cube map render is timed.
    Options are \c {ReflectionTimeSlicing.None}, \c {ReflectionTimeSlicing.AllFacesAtOnce} and
    \c {ReflectionTimeSlicing.IndividualFaces}.

    \value ReflectionTimeSlicing.None
        All faces of the cube map are rendered and prefiltered during one frame.

    \value ReflectionTimeSlicing.AllFacesAtOnce
        All faces are rendered during one frame but the prefiltering
        is divided to subsquent frames with each mip level handled on
        their own frame. Rough surface reflections are thus refreshed
        every sixth frame while smooth surfaces have reflections
        that refresh every frame.

    \value ReflectionTimeSlicing.IndividualFaces
        Each face is rendered and prefiltered in a separate frame.
        Thus all reflections are refreshed every sixth frame.

    \note Use \c {ReflectionTimeSlicing.AllFacesAtOnce} or
    \c {ReflectionTimeSlicing.IndividualFaces} to increase performance.
*/
QQuick3DReflectionProbe::ReflectionTimeSlicing QQuick3DReflectionProbe::timeSlicing() const
{
    return m_timeSlicing;
}

/*!
    \qmlproperty bool ReflectionProbe::parallaxCorrection

    By default the reflections provided by the reflection probe are assumed to be from an infinite distance similar
    to the skybox. This works fine for environmental reflections but for tight spaces this causes perspective errors
    in the reflections. To fix this parallax correction can be turned on. The distance of the reflection is then
    determined by the \l ReflectionProbe::boxSize property.

    \sa boxSize
*/
bool QQuick3DReflectionProbe::parallaxCorrection() const
{
    return m_parallaxCorrection;
}

/*!
    \qmlproperty vector3d ReflectionProbe::boxSize

    Box size is used to determine which objects get their reflections from this ReflectionProbe. Objects that are
    inside the box are under the influence of this ReflectionProbe. If an object lies inside more than one reflection
    probe at the same time, the object is considered to be inside the nearest reflection probe.
    With \l ReflectionProbe::parallaxCorrection turned on the size is also used to calculate the distance of
    the reflections in the cube map.

    \sa parallaxCorrection
*/
QVector3D QQuick3DReflectionProbe::boxSize() const
{
    return m_boxSize;
}

/*!
    \qmlproperty bool ReflectionProbe::debugView

    If this property is set to true, a wireframe is rendered to visualize the reflection probe box.
*/
bool QQuick3DReflectionProbe::debugView() const
{
    return m_debugView;
}

void QQuick3DReflectionProbe::setQuality(QQuick3DReflectionProbe::ReflectionQuality reflectionQuality)
{
    if (m_quality == reflectionQuality)
        return;

    m_quality = reflectionQuality;
    m_dirtyFlags.setFlag(DirtyFlag::QualityDirty);
    emit qualityChanged();
    update();
}

void QQuick3DReflectionProbe::setClearColor(const QColor &clearColor)
{
    if (m_clearColor == clearColor)
        return;
    m_clearColor = clearColor;
    m_dirtyFlags.setFlag(DirtyFlag::ClearColorDirty);
    emit clearColorChanged();
    update();
}

void QQuick3DReflectionProbe::setRefreshMode(ReflectionRefreshMode newRefreshMode)
{
    if (m_refreshMode == newRefreshMode)
        return;
    m_refreshMode = newRefreshMode;
    m_dirtyFlags.setFlag(DirtyFlag::RefreshModeDirty);
    emit refreshModeChanged();
    update();
}

void QQuick3DReflectionProbe::setTimeSlicing(ReflectionTimeSlicing newTimeSlicing)
{
    if (m_timeSlicing == newTimeSlicing)
        return;
    m_timeSlicing = newTimeSlicing;
    m_dirtyFlags.setFlag(DirtyFlag::TimeSlicingDirty);
    emit timeSlicingChanged();
    update();
}

void QQuick3DReflectionProbe::setParallaxCorrection(bool parallaxCorrection)
{
    if (m_parallaxCorrection == parallaxCorrection)
        return;
    m_parallaxCorrection = parallaxCorrection;
    m_dirtyFlags.setFlag(DirtyFlag::ParallaxCorrectionDirty);
    emit parallaxCorrectionChanged();
    update();
}

void QQuick3DReflectionProbe::setBoxSize(const QVector3D &boxSize)
{
    if (m_boxSize == boxSize)
        return;
    m_boxSize = boxSize;
    m_dirtyFlags.setFlag(DirtyFlag::BoxDirty);
    emit boxSizeChanged();
    updateDebugView();
    update();
}

void QQuick3DReflectionProbe::setDebugView(bool debugView)
{
    if (m_debugView == debugView)
        return;
    m_debugView = debugView;
    emit debugViewChanged();
    updateDebugView();
}

QSSGRenderGraphObject *QQuick3DReflectionProbe::updateSpatialNode(QSSGRenderGraphObject *node)
{
    if (!node) {
        markAllDirty();
        node = new QSSGRenderReflectionProbe();
    }

    QQuick3DNode::updateSpatialNode(node);

    QSSGRenderReflectionProbe *probe = static_cast<QSSGRenderReflectionProbe *>(node);

    if (m_dirtyFlags.testFlag(DirtyFlag::QualityDirty)) {
        m_dirtyFlags.setFlag(DirtyFlag::QualityDirty, false);
        probe->reflectionMapRes = mapToReflectionResolution(m_quality);
    }

    if (m_dirtyFlags.testFlag(DirtyFlag::ClearColorDirty)) {
        m_dirtyFlags.setFlag(DirtyFlag::ClearColorDirty, false);
        probe->clearColor = m_clearColor;
    }

    if (m_dirtyFlags.testFlag(DirtyFlag::RefreshModeDirty)) {
        m_dirtyFlags.setFlag(DirtyFlag::RefreshModeDirty, false);
        switch (m_refreshMode) {
        case ReflectionRefreshMode::FirstFrame:
            probe->refreshMode = QSSGRenderReflectionProbe::ReflectionRefreshMode::FirstFrame;
            break;
        case ReflectionRefreshMode::EveryFrame:
            probe->refreshMode = QSSGRenderReflectionProbe::ReflectionRefreshMode::EveryFrame;
            break;
        }
    }

    if (m_dirtyFlags.testFlag(DirtyFlag::TimeSlicingDirty)) {
        m_dirtyFlags.setFlag(DirtyFlag::TimeSlicingDirty, false);
        switch (m_timeSlicing) {
        case ReflectionTimeSlicing::None:
            probe->timeSlicing = QSSGRenderReflectionProbe::ReflectionTimeSlicing::None;
            break;
        case ReflectionTimeSlicing::AllFacesAtOnce:
            probe->timeSlicing = QSSGRenderReflectionProbe::ReflectionTimeSlicing::AllFacesAtOnce;
            break;
        case ReflectionTimeSlicing::IndividualFaces:
            probe->timeSlicing = QSSGRenderReflectionProbe::ReflectionTimeSlicing::IndividualFaces;
            break;
        }
    }

    if (m_dirtyFlags.testFlag(DirtyFlag::ParallaxCorrectionDirty)) {
        m_dirtyFlags.setFlag(DirtyFlag::ParallaxCorrectionDirty, false);
        probe->parallaxCorrection = m_parallaxCorrection;
    }

    if (m_dirtyFlags.testFlag(DirtyFlag::BoxDirty)) {
        m_dirtyFlags.setFlag(DirtyFlag::BoxDirty, false);
        probe->boxSize = m_boxSize;
    }

    return node;
}

void QQuick3DReflectionProbe::markAllDirty()
{
    m_dirtyFlags = DirtyFlags(DirtyFlag::QualityDirty)
            | DirtyFlags(DirtyFlag::ClearColorDirty)
            | DirtyFlags(DirtyFlag::RefreshModeDirty)
            | DirtyFlags(DirtyFlag::ParallaxCorrectionDirty)
            | DirtyFlags(DirtyFlag::BoxDirty)
            | DirtyFlags(DirtyFlag::TimeSlicingDirty);
    QQuick3DNode::markAllDirty();
}

void QQuick3DReflectionProbe::updateDebugView()
{
    if (m_debugView) {
        if (!m_debugViewGeometry)
            m_debugViewGeometry = new QQuick3DGeometry(this);

        m_debugViewGeometry->clear();
        m_debugViewGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic, 0,
                                          QQuick3DGeometry::Attribute::ComponentType::F32Type);
        m_debugViewGeometry->setStride(12);
        m_debugViewGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
        m_debugViewGeometry->setBounds(-boxSize() / 2, boxSize() / 2);

        QVector<QVector3D> m_vertices;

        //Lines
        m_vertices.append(QVector3D(-boxSize().x() / 2, -boxSize().y() / 2, boxSize().z() / 2));
        m_vertices.append(QVector3D(boxSize().x() / 2, -boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(boxSize().x() / 2, -boxSize().y() / 2, boxSize().z() / 2));
        m_vertices.append(QVector3D(boxSize().x() / 2, boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(boxSize().x() / 2, boxSize().y() / 2, boxSize().z() / 2));
        m_vertices.append(QVector3D(-boxSize().x() / 2, boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(-boxSize().x() / 2, boxSize().y() / 2, boxSize().z() / 2));
        m_vertices.append(QVector3D(-boxSize().x() / 2, -boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(-boxSize().x() / 2, -boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(boxSize().x() / 2, -boxSize().y() / 2, -boxSize().z() / 2));

        m_vertices.append(QVector3D(boxSize().x() / 2, -boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(boxSize().x() / 2, boxSize().y() / 2, -boxSize().z() / 2));

        m_vertices.append(QVector3D(boxSize().x() / 2, boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(-boxSize().x() / 2, boxSize().y() / 2, -boxSize().z() / 2));

        m_vertices.append(QVector3D(-boxSize().x() / 2, boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(-boxSize().x() / 2, -boxSize().y() / 2, -boxSize().z() / 2));

        m_vertices.append(QVector3D(-boxSize().x() / 2, boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(-boxSize().x() / 2, boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(boxSize().x() / 2, boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(boxSize().x() / 2, boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(-boxSize().x() / 2, -boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(-boxSize().x() / 2, -boxSize().y() / 2, boxSize().z() / 2));

        m_vertices.append(QVector3D(boxSize().x() / 2, -boxSize().y() / 2, -boxSize().z() / 2));
        m_vertices.append(QVector3D(boxSize().x() / 2, -boxSize().y() / 2, boxSize().z() / 2));

        QByteArray vertexData;
        vertexData.resize(m_vertices.size() * 3 * sizeof(float));
        float *data = reinterpret_cast<float *>(vertexData.data());
        for (int i = 0; i < m_vertices.size(); i++) {
            data[0] = m_vertices[i].x();
            data[1] = m_vertices[i].y();
            data[2] = m_vertices[i].z();
            data += 3;
        }

        m_debugViewGeometry->setVertexData(vertexData);
        m_debugViewGeometry->update();

        if (!m_debugViewModel) {
            m_debugViewModel = new QQuick3DModel();
            m_debugViewModel->setParentItem(this);
            m_debugViewModel->setParent(this);
            m_debugViewModel->setCastsShadows(false);
            m_debugViewModel->setGeometry(m_debugViewGeometry);
        }

        if (!m_debugViewMaterial) {
            m_debugViewMaterial = new QQuick3DDefaultMaterial();
            m_debugViewMaterial->setParentItem(m_debugViewModel);
            m_debugViewMaterial->setParent(m_debugViewModel);
            m_debugViewMaterial->setDiffuseColor(QColor(3, 252, 219));
            m_debugViewMaterial->setLighting(QQuick3DDefaultMaterial::NoLighting);
            m_debugViewMaterial->setCullMode(QQuick3DMaterial::NoCulling);
            QQmlListReference materialsRef(m_debugViewModel, "materials");
            materialsRef.append(m_debugViewMaterial);
        }
    } else {
        if (m_debugViewModel) {
            delete m_debugViewModel;
            m_debugViewModel = nullptr;
            m_debugViewMaterial = nullptr;
        }

        if (m_debugViewGeometry) {
            delete m_debugViewGeometry;
            m_debugViewGeometry = nullptr;
        }
    }
}

quint32 QQuick3DReflectionProbe::mapToReflectionResolution(ReflectionQuality quality)
{
    switch (quality) {
    case ReflectionQuality::Low:
        return 8;
    case ReflectionQuality::Medium:
        return 9;
    case ReflectionQuality::High:
        return 10;
    case ReflectionQuality::VeryHigh:
        return 11;
    default:
        break;
    }
    return 7;
}

QT_END_NAMESPACE
