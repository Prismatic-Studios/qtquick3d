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

#include "glslmemorypool_p.h"
#include <cstring>
#include <cassert>
#include <cstdlib>

QT_BEGIN_NAMESPACE

using namespace GLSL;

MemoryPool::MemoryPool()
    : _blocks(nullptr),
      _allocatedBlocks(0),
      _blockCount(-1),
      _ptr(nullptr),
      _end(nullptr)
{ }

MemoryPool::~MemoryPool()
{
    if (_blocks) {
        for (int i = 0; i < _allocatedBlocks; ++i) {
            if (char *b = _blocks[i])
                std::free(b);
        }

        std::free(_blocks);
    }
}

void MemoryPool::reset()
{
    _blockCount = -1;
    _ptr = _end = nullptr;
}

void *MemoryPool::allocate_helper(size_t size)
{
    assert(size < BLOCK_SIZE);

    if (++_blockCount == _allocatedBlocks) {
        if (! _allocatedBlocks)
            _allocatedBlocks = DEFAULT_BLOCK_COUNT;
        else
            _allocatedBlocks *= 2;

        _blocks = (char **) realloc(_blocks, sizeof(char *) * _allocatedBlocks);

        for (int index = _blockCount; index < _allocatedBlocks; ++index)
            _blocks[index] = nullptr;
    }

    char *&block = _blocks[_blockCount];

    if (! block)
        block = (char *) std::malloc(BLOCK_SIZE);

    _ptr = block;
    _end = _ptr + BLOCK_SIZE;

    void *addr = _ptr;
    _ptr += size;
    return addr;
}

Managed::Managed()
{ }

Managed::~Managed()
{ }

void *Managed::operator new(size_t size, MemoryPool *pool)
{ return pool->allocate(size); }

void Managed::operator delete(void *)
{ }

void Managed::operator delete(void *, MemoryPool *)
{ }

QT_END_NAMESPACE
