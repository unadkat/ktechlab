/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah
      zoltan_padrah@users.sourceforge.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "creatingtest.h"
#include <circuit.h>
#include <ecvoltagesource.h>
#include <resistor.h>
#include <electronicconnector.h>
#include <ecnode.h>

#define private public
#include <simulator.h>
#undef private

#include <QDebug>
#include <QtTest/QtTest>
#include <ktlconfig.h>


#define MAX_CURRENT_ERROR 1e-9


void CreatingTest::cleanupTestCase()
{
    qDebug() << "Cleaning up...";
    KtlConfig::destroy();
    Simulator::destroy();
}


void CreatingTest::initAndStep(Circuit* circ)
{
    Q_ASSERT(circ);

    circ->init();

    Simulator * s = Simulator::self();
    s->attachCircuit(circ);
    s->slotSetSimulating(true);
    s->step();
    circ->updateCurrents();
    s->slotSetSimulating(false);
}

void CreatingTest::emptyCircuitTest()
{
    Circuit circ;

    circ.init();

    Simulator::self()->attachCircuit(&circ);
    Simulator::self()->slotSetSimulating(true);
    Simulator::self()->step();
    circ.updateCurrents();

    Simulator::self()->slotSetSimulating(false);
    Simulator::self()->detachCircuit(&circ);
}

void CreatingTest::localVariablesTest()
{
    {
        Circuit circ;
        ECCell v1(circ);
        Resistor r1(circ);
        ElectronicConnector cc1(v1.pinByName("n1"), r1.pinByName("n1"));
        ElectronicConnector cc2(v1.pinByName("p1"), r1.pinByName("p1"));

        circ.init();

        Simulator::self()->attachCircuit(&circ);
        Simulator::self()->slotSetSimulating(true);
        Simulator::self()->step();
        circ.updateCurrents();
        Simulator::self()->slotSetSimulating(false);

        qDebug() << "cc1 current: " << cc1.wire(0)->current();
        qDebug() << "cc2 current: " << cc2.wire(0)->current();

        Q_ASSERT(qAbs(qAbs(cc1.wire(0)->current()) - 5) < MAX_CURRENT_ERROR);
        Q_ASSERT(qAbs(qAbs(cc2.wire(0)->current()) - 5) < MAX_CURRENT_ERROR);
        Q_ASSERT(qAbs(cc1.wire(0)->current() + cc2.wire(0)->current()) < MAX_CURRENT_ERROR);

        Q_ASSERT(circ.components().size() == 2);

        {
            Resistor r2(circ);

            initAndStep(&circ);

            qDebug() << "r2 pin voltage:" << r2.pinByName("n1")->pin()->voltage();

            Q_ASSERT(circ.components().size() == 3);
        }
        Q_ASSERT(circ.components().size() == 2);
    }
    Q_ASSERT(Simulator::self()->m_ordinaryCircuits->size() == 0);
}

QTEST_MAIN(CreatingTest)
#include "creatingtest.moc"
