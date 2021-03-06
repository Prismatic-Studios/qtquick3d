/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#include "qquick3dskin_p.h"
#include "qquick3dobject_p.h"
#include "qquick3dscenemanager_p.h"

#include <QtQuick3DRuntimeRender/private/qssgrendergraphobject_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderskin_p.h>

#include <QtQuick3DUtils/private/qssgutils_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Skin
    \inherits Object3D
    \inqmlmodule QtQuick3D
    \brief Defines a skinning animation.

    A skin defines how a model can be animated using \l {Vertex Skinning}
    {skeletal animation}. It contains a list of \l {Node}s and an optional list
    of the Inverse Bind Pose Matrices.
    Each \l {Node}'s transform becomes a transform of the bone with the
    corresponding index in the list.

    \qml
    Skin {
        id: skin0
        joints: [
            node0,
            node1,
            node2
        ]
        inverseBindPoses: [
            Qt.matrix4x4(...),
            Qt.matrix4x4(...),
            Qt.matrix4x4(...)
        ]
    }
    \endqml

    \note \l {Skeleton} and \l {Joint} will be deprecated.
*/

QQuick3DSkin::QQuick3DSkin(QQuick3DObject *parent)
    : QQuick3DObject(*(new QQuick3DObjectPrivate(QQuick3DObjectPrivate::Type::Skin)), parent)
{
}

QQuick3DSkin::~QQuick3DSkin()
{
}

/*!
    \qmlproperty List<QtQuick3D::Node> Skin::joints

    This property contains a list of nodes used for a hierarchy of joints.
    The order in the list becomes the index of the joint, which is used in the
    \c SkinSemantic \l {QQuick3DGeometry::addAttribute}{custom geometry attribute}.

    \sa {QQuick3DGeometry::addAttribute}, {Qt Quick 3D - Simple Skinning Example}
*/
QQmlListProperty<QQuick3DNode> QQuick3DSkin::joints()
{
    return QQmlListProperty<QQuick3DNode>(this,
                                            nullptr,
                                            QQuick3DSkin::qmlAppendJoint,
                                            QQuick3DSkin::qmlJointsCount,
                                            QQuick3DSkin::qmlJointAt,
                                            QQuick3DSkin::qmlClearJoints);
}

void QQuick3DSkin::onJointChanged(QQuick3DNode *node)
{
    for (int i = 0; i < m_joints.count(); ++i) {
        if (m_joints.at(i) == node) {
            QMatrix4x4 jointGlobal = m_joints.at(i)->sceneTransform();
            if (m_inverseBindPoses.count() > i)
                jointGlobal *= m_inverseBindPoses.at(i);
            m_boneMatrices[i] = jointGlobal;
            m_boneNormalMatrices[i] = jointGlobal.normalMatrix();
            markDirty();
        }
    }
}

void QQuick3DSkin::onJointDestroyed(QObject *object)
{
    for (int i = 0; i < m_joints.count(); ++i) {
        if (m_joints.at(i) == object) {
            m_joints.removeAt(i);
            m_boneMatrices.removeAt(i);
            m_boneNormalMatrices.removeAt(i);
            markDirty();
            break;
        }
    }
}

void QQuick3DSkin::qmlAppendJoint(QQmlListProperty<QQuick3DNode> *list, QQuick3DNode *joint)
{
    if (joint == nullptr)
        return;
    QQuick3DSkin *self = static_cast<QQuick3DSkin *>(list->object);
    int index = self->m_joints.count();
    self->m_joints.push_back(joint);
    QMatrix4x4 M = joint->sceneTransform();
    if (index < self->m_inverseBindPoses.count())
        M *= self->m_inverseBindPoses.at(index);
    self->m_boneMatrices.push_back(M);
    self->m_boneNormalMatrices.push_back(M.normalMatrix());
    self->markDirty();

    connect(joint, &QQuick3DNode::sceneTransformChanged, self,
            [self, joint]() { self->onJointChanged(joint); });
    connect(joint, &QQuick3DNode::destroyed, self, &QQuick3DSkin::onJointDestroyed);
}

QQuick3DNode *QQuick3DSkin::qmlJointAt(QQmlListProperty<QQuick3DNode> *list, qsizetype index)
{
    QQuick3DSkin *self = static_cast<QQuick3DSkin *>(list->object);
    return self->m_joints.at(index);
}

qsizetype QQuick3DSkin::qmlJointsCount(QQmlListProperty<QQuick3DNode> *list)
{
    QQuick3DSkin *self = static_cast<QQuick3DSkin *>(list->object);
    return self->m_joints.count();
}

void QQuick3DSkin::qmlClearJoints(QQmlListProperty<QQuick3DNode> *list)
{
    QQuick3DSkin *self = static_cast<QQuick3DSkin *>(list->object);
    for (const auto &joint : qAsConst(self->m_joints)) {
        joint->disconnect(self, SLOT(onJointDestroyed(QObject*)));
    }
    self->m_joints.clear();
    self->markDirty();
}


/*!
    \qmlproperty List<matrix4x4> Skin::inverseBindPoses

    This property contains a list of Inverse Bind Pose matrixes used for the
    skinning animation. Each inverseBindPose matrix means the inverse of the
    global transform of the corresponding node in \l {Skin::joints},
    used initially.

    \note This property is an optional property. That is, if some or all of the
    matrices are not set, identity values will be used.
*/
QList<QMatrix4x4> QQuick3DSkin::inverseBindPoses() const
{
    return m_inverseBindPoses;
}

void QQuick3DSkin::setInverseBindPoses(const QList<QMatrix4x4> &poses)
{
    if (m_inverseBindPoses == poses)
        return;

    m_inverseBindPoses = poses;

    for (int i = 0; i < m_joints.count(); ++i) {
        QMatrix4x4 jointGlobal = m_joints.at(i)->sceneTransform();
        if (m_inverseBindPoses.count() > i)
            jointGlobal *= m_inverseBindPoses.at(i);
        m_boneMatrices[i] = jointGlobal;
        m_boneNormalMatrices[i] = jointGlobal.normalMatrix();
    }

    markDirty();
    emit inverseBindPosesChanged();
}

void QQuick3DSkin::markDirty()
{
    if (!m_dirty) {
        m_dirty = true;
        update();
    }
}


void QQuick3DSkin::markAllDirty()
{
    m_dirty = true;
    QQuick3DObject::markAllDirty();
}

QSSGRenderGraphObject *QQuick3DSkin::updateSpatialNode(QSSGRenderGraphObject *node)
{
    if (!node) {
        markAllDirty();
        node = new QSSGRenderSkin();
    }

    auto skinNode = static_cast<QSSGRenderSkin *>(node);

    if (m_dirty) {
        m_dirty = false;
        skinNode->boneMatrices = m_boneMatrices;
        skinNode->boneNormalMatrices = m_boneNormalMatrices;
    }

    return node;
}

QT_END_NAMESPACE
