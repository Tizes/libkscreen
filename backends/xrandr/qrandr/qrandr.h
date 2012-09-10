/*************************************************************************************
 *  Copyright (C) 2011 by Alex Fiestas <afiestas@kde.org>                            *
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


#ifndef XRANDR_H
#define XRANDR_H

#include "xlibandxrandr.h"

#include <QtCore/QCoreApplication>
#include <QObject>
#include <QPair>

namespace QRandR {
    class Screen;

class QRandR : public QObject
{
    Q_OBJECT

    public:
        static QRandR *self();
        static bool x11EventFilter(void *message, long int *result);

        QPair<int, int> version();
        Screen * screen();
        QList<Screen *> screens();
        Display *display();

    public:
        static bool has_1_3;
        static int s_Timestamp;

    private:
        QRandR();
        void setVersion(int major, int minor);
        void setDisplay(Display *display);
        void handleEvent(XEvent *event);

        void passEventToScreens(XRRScreenChangeNotifyEvent *event);

    private:
        static QRandR *s_instance;
        static QCoreApplication::EventFilter s_oldEventFilter;
        static int s_RRScreen, s_RRNotify;

        Display *m_display;
        QPair<int,int> m_version;
        QList<Screen *> m_screens;
};
}
#endif // XRANDR_H