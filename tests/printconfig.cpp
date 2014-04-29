/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include <QGuiApplication>

#include "../src/config.h"
#include "../src/config.h"
#include "../src/output.h"
#include "../src/mode.h"
#include "../src/edid.h"

#include "../src/configmonitor.h"
using namespace KScreen;
QString typetoString(const Output::Type& type)
{
    switch (type) {
        case Output::Unknown:
            return QLatin1String("Unknown");
        case Output::Panel:
            return QLatin1String("Panel (Laptop)");
        case Output::VGA:
            return QLatin1String("VGA");
        case Output::DVII:
            return QLatin1String("DVI-I");
        case Output::DVIA:
            return QLatin1String("DVI-A");
        case Output::DVID:
            return QLatin1String("DVI-D");
        case Output::HDMI:
            return QLatin1String("HDMI");
        case Output::TV:
            return QLatin1String("TV");
        case Output::TVComposite:
            return QLatin1String("TV-Composite");
        case Output::TVSVideo:
            return QLatin1String("TV-SVideo");
        case Output::TVComponent:
            return QLatin1String("TV-Component");
        case Output::TVSCART:
            return QLatin1String("TV-SCART");
        case Output::TVC4:
            return QLatin1String("TV-C4");
        case Output::DisplayPort:
            return QLatin1String("DisplayPort");
        default:
            return QLatin1String("Invalid Type");

    };
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    KScreen::Config *m_config = KScreen::Config::current();
    if (!m_config) {
        qDebug() << "Config is invalid, probably backend couldn't load";
        return -1;
    }
    if (!m_config->screen()) {
        qDebug() << "No screen in the configuration, broken backend";
        return -1;
    }

    ConfigMonitor::instance()->addConfig(m_config);    qDebug() << "Screen:";
    qDebug() << "\tmaxSize:" << m_config->screen()->maxSize();
    qDebug() << "\tminSize:" << m_config->screen()->minSize();
    qDebug() << "\tcurrentSize:" << m_config->screen()->currentSize();

    OutputList outputs = m_config->outputs();
    Q_FOREACH(Output *output, outputs) {
        qDebug() << "\n-----------------------------------------------------\n";
        qDebug() << "Id: " << output->id();
        qDebug() << "Name: " << output->name();
        qDebug() << "Type: " << typetoString(output->type());
        qDebug() << "Connected: " << output->isConnected();
        if (!output->isConnected()) {
            continue;
        }
        qDebug() << "Enabled: " << output->isEnabled();
        qDebug() << "Primary: " << output->isPrimary();
        qDebug() << "Rotation: " << output->rotation();
        qDebug() << "Pos: " << output->pos();
        qDebug() << "MMSize: " << output->sizeMm();
        if (output->currentMode()) {
            qDebug() << "Size: " << output->currentMode()->size();
        }
        if (output->clones().isEmpty()) {
            qDebug() << "Clones: " << "None";
        } else {
            qDebug() << "Clones: " << output->clones().count();
        }
        qDebug() << "Mode: " << output->currentModeId();
        qDebug() << "Preferred Mode: " << output->preferredModeId();
        qDebug() << "Preferred modes: " << output->preferredModes();
        qDebug() << "Modes: ";

        ModeList modes = output->modes();
        Q_FOREACH(Mode* mode, modes) {
            qDebug() << "\t" << mode->id() << "  " << mode->name() << " " << mode->size() << " " << mode->refreshRate();
        }

        Edid* edid = output->edid();
        qDebug() << "EDID Info: ";
        if (edid && edid->isValid()) {
            qDebug() << "\tDevice ID: " << edid->deviceId();
            qDebug() << "\tName: " << edid->name();
            qDebug() << "\tVendor: " << edid->vendor();
            qDebug() << "\tSerial: " << edid->serial();
            qDebug() << "\tEISA ID: " << edid->eisaId();
            qDebug() << "\tHash: " << edid->hash();
            qDebug() << "\tWidth: " << edid->width();
            qDebug() << "\tHeight: " << edid->height();
            qDebug() << "\tGamma: " << edid->gamma();
            qDebug() << "\tRed: " << edid->red();
            qDebug() << "\tGreen: " << edid->green();
            qDebug() << "\tBlue: " << edid->blue();
            qDebug() << "\tWhite: " << edid->white();
        } else {
            qDebug() << "\tUnavailable";
        }
    }

    return app.exec();
}