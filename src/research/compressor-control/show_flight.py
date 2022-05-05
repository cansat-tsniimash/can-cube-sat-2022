# -*- coding: utf-8 -*-
"""
This example demonstrates many of the 2D plotting capabilities
in pyqtgraph. All of the plots may be panned/scaled by dragging with 
the left/right mouse buttons. Right click on any plot to show a context menu.
"""

from csv import DictReader
import subprocess
import os

from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import pyqtgraph as pg


REAL_TIME_OFFSET_S = 315965014 - 213


this_dir_path = os.path.split(__file__)[0]
sp = subprocess.run(
    f"{this_dir_path}/Debug/compressor-control",
    check=True, capture_output=True
)
data = sp.stdout

with open("data.csv", mode="wb") as stream:
    stream.write(data)

with open("data.csv") as stream:
    reader = DictReader(stream)
    data = list(reader)

with open("PLD_MS5611_DATA-13-2.csv") as stream:
    reader = DictReader(stream)
    real_inner_data = list(reader)

with open("PLD_MS5611_DATA-13-1.csv") as stream:
    reader = DictReader(stream)
    real_outer_data = list(reader)

with open("valve_history.csv") as stream:
    reader = DictReader(stream)
    real_valve_data = list(reader)

with open("pump_history.csv") as stream:
    reader = DictReader(stream)
    real_pump_data = list(reader)


t = np.array([float(x['minutes']) for x in data])
inner_pressure = np.array([float(x['inner_pressure']) for x in data])
outer_pressure = np.array([float(x['outer_pressure']) for x in data])
altitude = np.array([float(x['altitude']) for x in data])
state = np.array([float(x['state']) for x in data])
pump_on = np.array([float(x['pump_on']) for x in data])
valve_open = np.array([float(x['valve_open']) for x in data])

real_alt_t = np.array([(float(x['time_s']) - REAL_TIME_OFFSET_S) for x in real_outer_data])
real_alt = np.array([float(x['altitude']) for x in real_outer_data])
real_outer_pressure = np.array([float(x['pressure']) for x in real_outer_data])

real_inner_t = np.array([(float(x['time_s']) - REAL_TIME_OFFSET_S) for x in real_inner_data])
real_inner_pressure = np.array([float(x['pressure']) for x in real_inner_data])

real_pump_t = np.array([(float(x['seconds'])) for x in real_pump_data])
real_pump_state = np.array([(float(x['state'])) for x in real_pump_data])

real_valve_t = np.array([(float(x['seconds'])) for x in real_valve_data])
real_valve_state = np.array([(float(x['state'])) for x in real_valve_data])


pg.setConfigOptions(antialias=True)
app = QtGui.QApplication([])
win = pg.PlotWidget()
win.show()


p1 = win.plotItem
p2 = pg.ViewBox()
p1.showAxis('right')
p1.scene().addItem(p2)
p1.getAxis('right').linkToView(p2)
p2.setXLink(p1)

## Handle view resizing 
def updateViews():
    global p1, p2, p3
    p2.setGeometry(p1.vb.sceneBoundingRect())
    p2.linkedViewChanged(p1.vb, p2.XAxis)

updateViews()
p1.vb.sigResized.connect(updateViews)
win.addLegend()

t = t*60
p1.setYRange(-120_000, 220_000)
p1.plot(x=t, y=inner_pressure, pen='#cccccc', name='pressure_in')
p1.plot(x=real_inner_t, y=real_inner_pressure, pen=pg.mkPen('g', width=2, style=QtCore.Qt.DashLine), name="real_pressure_out")
p1.plot(x=t, y=outer_pressure, pen='r', name='pressure_out')
p1.plot(x=real_alt_t, y=real_outer_pressure, pen=pg.mkPen('r', width=2, style=QtCore.Qt.DashLine), name="real_pressure_out")
p2.addItem(pg.PlotCurveItem(x=t, y=altitude, pen='b', name="altitude"))
p2.addItem(pg.PlotCurveItem(x=real_alt_t, y=real_alt, pen=pg.mkPen('b', width=2, style=QtCore.Qt.DashLine), name="real_altitude"))
p2.addItem(pg.PlotCurveItem(x=t, y=state*1000-3000, pen='w', name="state", stepMode='left'))
p2.addItem(pg.PlotCurveItem(x=t, y=pump_on*2000-6000, pen='r', name="pump_state", stepMode='left'))
p2.addItem(pg.PlotCurveItem(x=t, y=valve_open*1000-6000, pen='g', name="valve_state", stepMode='left'))
p2.addItem(pg.PlotCurveItem(x=real_valve_t, y=real_valve_state*2000-10000, pen=pg.mkPen('r', width=2, style=QtCore.Qt.DashLine), name="real_valve_state", stepMode='left'))
p2.addItem(pg.PlotCurveItem(x=real_pump_t, y=real_pump_state*1000-10000, pen=pg.mkPen('g', width=2, style=QtCore.Qt.DashLine), name="real_pump_state", stepMode='left'))


if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()