/***************************************************************
QGVCore
Copyright (c) 2014, Bergont Nicolas, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
***************************************************************/

#include "QGVEdgePrivate.h"

QGVEdgePrivate::QGVEdgePrivate(Agedge_t *_agedge)
{
  setEdge(_agedge);
}

void QGVEdgePrivate::setEdge(Agedge_t *_agedge)
{
  _edge = _agedge;
}

Agedge_t* QGVEdgePrivate::edge() const
{
  return _edge;
}
