/*************************************************************************************
 *  Copyright 2014 Sebastian Kügler <sebas@kde.org>                                  *
 *                                                                                   *
 *  This library is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU Lesser General Public                       *
 *  License as published by the Free Software Foundation; either                     *
 *  version 2.1 of the License, or (at your option) any later version.               *
 *                                                                                   *
 *  This library is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 *  Lesser General Public License for more details.                                  *
 *                                                                                   *
 *  You should have received a copy of the GNU Lesser General Public                 *
 *  License along with this library; if not, write to the Free Software              *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA       *
 *************************************************************************************/

#include "waylandoutput.h"
#include "waylandbackend.h"
#include "waylandconfig.h"

#include <mode.h>
#include <edid.h>

#include <QRect>
#include <QGuiApplication>


using namespace KScreen;

WaylandOutput::WaylandOutput(QObject *parent)
    : QObject(parent)
    , m_edid(new Edid(QByteArray(), this))
    , m_id(-1)
    , m_completed(false)
    , m_output(nullptr)
    , m_protocolName(0)
    , m_protocolVersion(0)
{
}

WaylandOutput::~WaylandOutput()
{
    qDebug() << "bye bye output";
}

quint32 WaylandOutput::id() const
{
    return m_id;
}

void WaylandOutput::setId(const quint32 newId)
{
    m_id = newId;
}

bool WaylandOutput::enabled() const
{
    return m_output != nullptr;
}


KWayland::Client::OutputDevice* WaylandOutput::output() const
{
    return m_output;
}

void WaylandOutput::setOutput(KWayland::Client::Registry* registry, KWayland::Client::OutputDevice* op, quint32 name, quint32 version)
{
    qDebug() << "WL setOUtput" << registry << op << name;
    if (m_output == op) {
        return;
    }
    m_output = op;

    connect(m_output, &KWayland::Client::OutputDevice::changed,
            this, &WaylandOutput::update, Qt::QueuedConnection);
    connect(m_output, &KWayland::Client::OutputDevice::modeAdded,
            this, &WaylandOutput::updateModes, Qt::QueuedConnection);

    m_output->setup(registry->bindOutputDevice(name, version));
    qDebug() << "WL Client::OutputDevice bound";
    emit changed();
}


/*
KScreen::Edid WaylandOutput::edid()
{
    if (!m_edid) {
        m_edid(new KScreen::Edid(QByteArray(), this));
    }
    return m_edid;
    //return KScreen::Edid(QByteArray(), this);
    return 0;
}
*/

KScreen::OutputPtr WaylandOutput::toKScreenOutput(KScreen::ConfigPtr &parent) const
{
    KScreen::OutputPtr output(new KScreen::Output());
    output->setId(m_id);
    output->setName(QString::number(m_id));
//     qCDebug(KSCREEN_WAYLAND) << "toKScreenOutput OUTPUT";
    //updateKScreenOutput(output); // Doesn't seem to be needed, but verify!
    return output;
}

void WaylandOutput::updateKScreenOutput(KScreen::OutputPtr &output) const
{
    qCDebug(KSCREEN_WAYLAND) << "updateKScreenOutput OUTPUT";
    // Initialize primary output
    output->setEnabled(m_output->enabled());
    output->setConnected(true);
    output->setPrimary(true); // FIXME
    // FIXME: Rotation

    output->setName(m_output->manufacturer() + QStringLiteral("-") + m_output->model());
    // Physical size
    output->setSizeMm(m_output->physicalSize());
    //qCDebug(KSCREEN_WAYLAND) << "  ####### setSizeMm: " << physicalSize() << geometry();
    output->setPos(m_output->globalPosition());
    KScreen::ModeList modeList;
    Q_FOREACH (const KWayland::Client::OutputDevice::Mode &m, m_output->modes()) {
        KScreen::ModePtr mode(new KScreen::Mode());
        const QString modename = modeName(m);
        mode->setId(modename);
        mode->setRefreshRate(m.refreshRate);
        mode->setSize(m.size);
        mode->setName(modename);
        if (m.flags.testFlag(KWayland::Client::OutputDevice::Mode::Flag::Current)) {
            output->setCurrentModeId(modename);
        }
        modeList[modename] = mode;
    }

    output->setModes(modeList);
}

void WaylandOutput::update()
{
    //qDebug() << "update()";
    flush();
}

QString WaylandOutput::modeName(const KWayland::Client::OutputDevice::Mode &m) const
{
    return QString::number(m.size.width()) + QLatin1Char('x') + QString::number(m.size.height()) + QLatin1Char('@') + QString::number((int)(m.refreshRate/1000));
}


void WaylandOutput::updateModes()
{
    //qDebug() << "updateModes()";
    flush();
}

bool WaylandOutput::isComplete()
{
    // FIXME: we want smarter tracking when the whole initialization storm is done and ...
    // the data structures are complete (for now).
    return (m_id != -1 && m_output && m_output->modes().count() > 0);
}

void WaylandOutput::flush()
{
    qCDebug(KSCREEN_WAYLAND) << "FLUSH" << (m_output->isValid() ? "Valid" : "Invalid");
    if (isComplete() && !m_completed) {
        m_completed = true;

        qCDebug(KSCREEN_WAYLAND) << "_______________ " << (m_output->isValid() ? "Valid" : "Invalid");
        qCDebug(KSCREEN_WAYLAND) << "Output changes... ";
        qCDebug(KSCREEN_WAYLAND) << "  id:              " << id();
        qCDebug(KSCREEN_WAYLAND) << "  Pixel Size:      " << m_output->pixelSize();
        qCDebug(KSCREEN_WAYLAND) << "  Physical Size:   " << m_output->physicalSize();
        qCDebug(KSCREEN_WAYLAND) << "  Global Position: " << m_output->globalPosition();
        qCDebug(KSCREEN_WAYLAND) << "  Manufacturer   : " << m_output->manufacturer();
        qCDebug(KSCREEN_WAYLAND) << "  Model:           " << m_output->model();

        foreach (auto m, m_output->modes()) {
            QString modename = modeName(m);
            if (m.flags.testFlag(KWayland::Client::OutputDevice::Mode::Flag::Current)) {
                modename = modename + " (current)";
            }
            if (m.flags.testFlag(KWayland::Client::OutputDevice::Mode::Flag::Preferred)) {
                modename = modename + " (Preferred)";
            }
            //qCDebug(KSCREEN_WAYLAND) << "            Mode : " << modename;

        }

        emit complete();
    }
}
