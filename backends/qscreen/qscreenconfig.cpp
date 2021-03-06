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

#include "qscreenconfig.h"
#include "qscreenoutput.h"
#include "qscreenscreen.h"
#include "qscreenbackend.h"

#include <mode.h>

#include <QRect>
#include <QGuiApplication>
#include <QScreen>

using namespace KScreen;

QScreenConfig::QScreenConfig(QObject *parent)
    : QObject(parent)
    , m_screen(new QScreenScreen(this))
    , m_blockSignals(true)
{
    foreach(const QScreen * qscreen, QGuiApplication::screens()) {
        screenAdded(qscreen);
    }
    m_blockSignals = false;
    connect(qApp, &QGuiApplication::screenAdded, this, &QScreenConfig::screenAdded);
    connect(qApp, &QGuiApplication::screenRemoved, this, &QScreenConfig::screenRemoved);
}

QScreenConfig::~QScreenConfig()
{
    foreach(auto output, m_outputMap.values()) {
        delete output;
    }
}

ConfigPtr QScreenConfig::toKScreenConfig() const
{
    ConfigPtr config(new Config);
    config->setScreen(m_screen->toKScreenScreen());
    updateKScreenConfig(config);
    return config;
}

int QScreenConfig::outputId(const QScreen *qscreen)
{
    QList<int> ids;
    foreach(auto output, m_outputMap.values()) {
        if (qscreen == output->qscreen()) {
            return output->id();
        }
    }
    m_lastOutputId++;
    return m_lastOutputId;
}

void QScreenConfig::screenAdded(const QScreen *qscreen)
{
    qCDebug(KSCREEN_QSCREEN) << "Screen added" << qscreen << qscreen->name();
    QScreenOutput *qscreenoutput = new QScreenOutput(qscreen, this);
    qscreenoutput->setId(outputId(qscreen));
    m_outputMap.insert(qscreenoutput->id(), qscreenoutput);

    if (!m_blockSignals) {
        Q_EMIT configChanged(toKScreenConfig());
    }
}

void QScreenConfig::screenRemoved(QScreen *qscreen)
{
    qCDebug(KSCREEN_QSCREEN) << "Screen removed" << qscreen << QGuiApplication::screens().count();
    // Find output matching the QScreen object and remove it
    int removedOutputId = -1;
    foreach(auto output, m_outputMap.values()) {
        if (output->qscreen() == qscreen) {
            removedOutputId = output->id();
            m_outputMap.remove(removedOutputId);
            delete output;
        }
    }
    Q_EMIT configChanged(toKScreenConfig());
}

void QScreenConfig::updateKScreenConfig(ConfigPtr &config) const
{
    KScreen::ScreenPtr screen = config->screen();
    m_screen->updateKScreenScreen(screen);
    config->setScreen(screen);

    //Removing removed outputs
    KScreen::OutputList outputs = config->outputs();
    Q_FOREACH(const KScreen::OutputPtr &output, outputs) {
        if (!m_outputMap.keys().contains(output->id())) {
            config->removeOutput(output->id());
        }
    }

    // Add KScreen::Outputs that aren't in the list yet, handle primaryOutput
    KScreen::OutputList kscreenOutputs = config->outputs();
    foreach(QScreenOutput *output, m_outputMap.values()) {
        KScreen::OutputPtr kscreenOutput = kscreenOutputs[output->id()];

        if (!kscreenOutput) {
            kscreenOutput = output->toKScreenOutput();
            kscreenOutputs.insert(kscreenOutput->id(), kscreenOutput);
        }
        output->updateKScreenOutput(kscreenOutput);
        if (QGuiApplication::primaryScreen() == output->qscreen()) {
            config->setPrimaryOutput(kscreenOutput);
        }
    }
    config->setOutputs(kscreenOutputs);
}

QMap< int, QScreenOutput * > QScreenConfig::outputMap() const
{
    return m_outputMap;
}
