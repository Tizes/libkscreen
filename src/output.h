/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *  Copyright (C) 2014 by Daniel Vrátil <dvratil@redhat.com>                         *
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

#ifndef OUTPUT_CONFIG_H
#define OUTPUT_CONFIG_H

#include "mode.h"
#include "types.h"
#include "kscreen_export.h"

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QMetaType>
#include <QStringList>
#include <QDebug>

namespace KScreen {

class Edid;

class KSCREEN_EXPORT Output : public QObject
{
    Q_OBJECT

    public:
        Q_ENUMS(Rotation)
        Q_ENUMS(Type)
        Q_PROPERTY(int id READ id CONSTANT)
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY outputChanged)
        Q_PROPERTY(Type type READ type WRITE setType NOTIFY outputChanged)
        Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY outputChanged)
        Q_PROPERTY(ModeList modes READ modes NOTIFY modesChanged)
        Q_PROPERTY(QPoint pos READ pos WRITE setPos NOTIFY posChanged)
        Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
        Q_PROPERTY(Rotation rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
        Q_PROPERTY(QString currentModeId READ currentModeId WRITE setCurrentModeId NOTIFY currentModeIdChanged)
        Q_PROPERTY(QString preferredModeId READ preferredModeId CONSTANT)
        Q_PROPERTY(bool connected READ isConnected WRITE setConnected NOTIFY isConnectedChanged)
        Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY isEnabledChanged)
        Q_PROPERTY(bool primary READ isPrimary WRITE setPrimary NOTIFY isPrimaryChanged)
        Q_PROPERTY(QList<int> clones READ clones WRITE setClones NOTIFY clonesChanged)
        Q_PROPERTY(KScreen::Edid* edid READ edid CONSTANT)
        Q_PROPERTY(QSize sizeMm READ sizeMm CONSTANT)
        Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)


        enum Type {
            Unknown,
            VGA,
            DVI,
            DVII,
            DVIA,
            DVID,
            HDMI,
            Panel,
            TV,
            TVComposite,
            TVSVideo,
            TVComponent,
            TVSCART,
            TVC4,
            DisplayPort
        };

        enum Rotation {
            None = 1,
            Left = 2,
            Inverted = 4,
            Right = 8
        };

        explicit Output();
        ~Output() override;

        OutputPtr clone() const;

        int id() const;
        void setId(int id);

        QString name() const;
        void setName(const QString& name);

        Type type() const;
        void setType(Type type);

        QString icon() const;
        void setIcon(const QString& icon);

        Q_INVOKABLE ModePtr mode(const QString &id) const;
        ModeList modes() const;
        void setModes(const ModeList &modes);

        QString currentModeId() const;
        void setCurrentModeId(const QString& mode);
        Q_INVOKABLE ModePtr currentMode() const;

        void setPreferredModes(const QStringList &modes);
        QStringList preferredModes() const;
        /**
         * Returns the preferred mode with higher resolution and refresh
         */
        Q_INVOKABLE QString preferredModeId() const;
        /**
         * Returns KScreen::Mode associated with preferredModeId()
         */
        Q_INVOKABLE ModePtr preferredMode() const;

        QPoint pos() const;
        void setPos(const QPoint& pos);

        /***
         * Returns actual size being rendered in the output
         *
         * The returned valued is after transformations have been applied to
         * the resolution of the current mode.
         *
         * For example if currentMode is 1280x800 but it is a vertical screen
         * the returned size will be 800x1280.
         *
         * If that same resolution (1280x800) is transformed and scale x2, the
         * value returned will be 2560x1600.
         *
         * This property reflects the currently active output configuration and
         * is not affected by current mode or orientation change made by user
         * until the config is applied.
         *
         * @since 5.4
         */
        QSize size() const;
        void setSize(const QSize& size);

        Rotation rotation() const;
        void setRotation(Rotation rotation);
        /**
         * A comfortable function that returns true when output is not rotated
         * or is rotated upside down.
         */
        Q_INVOKABLE inline bool isHorizontal() const {
            return ((rotation() == Output::None) || (rotation() == Output::Inverted));
        }

        bool isConnected() const;
        void setConnected(bool connected);

        bool isEnabled() const;
        void setEnabled(bool enabled);

        bool isPrimary() const;
        void setPrimary(bool primary);

        QList<int> clones() const;
        void setClones(QList<int> outputlist);

        void setEdid(const QByteArray &rawData);
        Edid* edid() const;

        /**
         * Returns the physical size of the screen in milimeters.
         *
         * @note Some broken GPUs or monitors return the size in centimeters instead
         * of millimeters. KScreen at the moment is not sanitizing the values.
         */
        QSize sizeMm() const;
        void setSizeMm(const QSize &size);

        /**
         * Returns a rectangle containing the currently set output position and
         * size.
         *
         * The geometry also reflects current orientation (i.e. if current mode
         * is 1920x1080 and orientation is @p KScreen::Output::Left, then the
         * size of the returned rectangle will be 1080x1920.
         *
         * This property contains the current settings stored in the particular
         * Output object, so it is updated even when user changes current mode
         * or orientation without applying the whole config/
         */
        QRect geometry() const;

        /**
         * returns the scaling factor to use for this output
         *
         * @since 5.9
         */
        qreal scale() const;

        /**
         * Set the scaling factor for this output.
         *
         * @arg factor Scale factor to use for this output, the backend may or may not
         * be able to deal with non-integer values, in that case, the factor gets rounded.
         *
         * @since 5.9
         */
        void setScale(qreal factor);

        void apply(const OutputPtr &other);
    Q_SIGNALS:
        void outputChanged();
        void posChanged();
        void sizeChanged();
        void currentModeIdChanged();
        void rotationChanged();
        void isConnectedChanged();
        void isEnabledChanged();
        void isPrimaryChanged();
        void clonesChanged();
        void scaleChanged();

        /** The mode list changed.
         *
         * This may happen when a mode is added or changed.
         *
         * @since 5.8.3
         */
        void modesChanged();

    private:
        Q_DISABLE_COPY(Output)

        class Private;
        Private * const d;

        Output(Private *dd);
};

} //KScreen namespace

KSCREEN_EXPORT QDebug operator<<(QDebug dbg, const KScreen::OutputPtr &output);

Q_DECLARE_METATYPE(KScreen::OutputList)
Q_DECLARE_METATYPE(KScreen::Output::Rotation)
Q_DECLARE_METATYPE(KScreen::Output::Type)

#endif //OUTPUT_H
