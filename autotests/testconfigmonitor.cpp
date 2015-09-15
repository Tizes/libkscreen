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
#include <QtTest>
#include <QSignalSpy>

#include "../src/config.h"
#include "../src/output.h"
#include "../src/configmonitor.h"
#include "../src/getconfigoperation.h"
#include "../src/backendmanager_p.h"
#include <QtTest/qsignalspy.h>

#include "fakebackendinterface.h"

class TestConfigMonitor : public QObject
{
    Q_OBJECT
public:
    TestConfigMonitor()
    {
    }

    KScreen::ConfigPtr getConfig()
    {
        KScreen::GetConfigOperation *op = new KScreen::GetConfigOperation();
        if (!op->exec()) {
            qWarning("Failed to retrieve backend: %s", qPrintable(op->errorString()));
            return KScreen::ConfigPtr();
        }

        return op->config();
    }

private Q_SLOTS:
    void initTestCase()
    {
        setenv("KSCREEN_BACKEND", "Fake", 1);
        KScreen::BackendManager::instance()->shutdownBackend();
    }

    void cleanupTestCase()
    {
        KScreen::BackendManager::instance()->shutdownBackend();
    }

    void testChangeNotify()
    {
        //json file for the fake backend
        QByteArray path(TEST_DATA);
        path.append("/singleoutput.json");
        setenv("TEST_DATA", path, 1);

        // Prepare monitor
        KScreen::ConfigMonitor *monitor = KScreen::ConfigMonitor::instance();
        QSignalSpy spy(monitor, SIGNAL(configurationChanged()));

        // Get config and let it monitored
        KScreen::ConfigPtr config = getConfig();
        monitor->addConfig(config);
        QSignalSpy enabledSpy(config->output(1).data(), SIGNAL(isEnabledChanged()));


        auto iface = new org::kde::kscreen::FakeBackend(QLatin1String("org.kde.KScreen.Backend.Fake"),
                                                        QLatin1String("/fake"),
                                                        QDBusConnection::sessionBus());
        QVERIFY(iface->isValid());

        iface->setEnabled(1, false).waitForFinished();

        QTRY_VERIFY(!spy.isEmpty());

        QCOMPARE(spy.size(), 1);
        QCOMPARE(enabledSpy.size(), 1);
        QCOMPARE(config->output(1)->isEnabled(), false);

    }

};

QTEST_MAIN(TestConfigMonitor)

#include "testconfigmonitor.moc"