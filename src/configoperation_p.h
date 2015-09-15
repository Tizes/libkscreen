/*
 * Copyright (C) 2014  Daniel Vratil <dvratil@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <QObject>

#include "configoperation.h"
#include "backendinterface.h"

namespace KScreen
{

class ConfigOperationPrivate : public QObject
{
    Q_OBJECT

public:
    ConfigOperationPrivate(ConfigOperation *qq);
    virtual ~ConfigOperationPrivate();

    void requestBackend();
    virtual void backendReady(org::kde::kscreen::Backend *backend);

public Q_SLOTS:
    void doEmitResult();

private:
    QString error;
    bool isExec;

protected:
    ConfigOperation * const q_ptr;
    Q_DECLARE_PUBLIC(ConfigOperation)

};

}