/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#ifndef _ODE_JOINT_FIXED_H_
#define _ODE_JOINT_FIXED_H_

#include "joint.h"


// fixed

struct dxJointFixed : public dxJoint
{
    dQuaternion qrel;   // initial relative rotation body1 -> body2
    dVector3 offset;    // relative offset between the bodies
    // erp and cfm moved to joint.h by OSRF
    // dReal erp;          // error reduction parameter
    // dReal cfm;          // constraint force mix-in
    void  set ( int num, dReal value );
    dReal get ( int num );

    dxJointFixed ( dxWorld *w );
    virtual void getSureMaxInfo( SureMaxInfo* info );
    virtual void getInfo1 ( Info1* info );
    virtual void getInfo2 ( Info2* info );
    virtual dJointType type() const;
    virtual size_t size() const;

    void computeInitialRelativeRotation();
};


#endif
