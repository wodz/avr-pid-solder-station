#! /usr/bin/python

import matplotlib
matplotlib.use('GTK')
		
from matplotlib.figure import Figure
from matplotlib.backends.backend_gtk import FigureCanvasGTK
import pygtk
import gtk
import gtk.glade
import gobject

import sys
import time
import serial
import threading
import Queue

class Data:
	def __init__(self):
		self.time = []
		self.temperature = []
		self.power = []
		self.setpoint = []

	pass

class UpdateData( threading.Thread ):
	def __init__(self,port,queue):
		self._abort = 0
		self._wait = 0
		self.queue = queue
		self.port = port
		self.count = 0
		threading.Thread.__init__ ( self )

	def run( self ):
		data = Data()
		time_start = time.time()
		while(True):
			if self._abort:
				self.port.write("log 0\r")
				self.port.close()
				return
		
			if self._wait:
				continue
			
			input_line = self.port.readline()
			time_current = time.time()
			try:
				Setpoint, Temperature, Power = input_line.split()
			except ValueError:
				continue

			Time = time_current-time_start
			Setpoint = int(Setpoint)
			Temperature = int(Temperature)
			Power = int(Power)

			data.time.append(Time)
			data.setpoint.append(Setpoint)
			data.temperature.append(Temperature)
			data.power.append(Power*100/511)
					
			self.queue.put(data)

	def abort( self ):
		self._abort = 1

	def wait( self,action ):
		if action:
			self._wait = 1
			self.port.write("log 0\r")
			self.port.readline()
		else:
			self.port.write("log 1\r")
			self.port.readline()
			self._wait = 0

class Main:

	def delete(self, widget, event, data=None):
		return False

	def destroy(self, widget, data=None):
		self.thread.abort()
		gtk.main_quit()
	
	def update(self):
		if (self.queue.empty()):
			return True

		data = self.queue.get()
		self.lines1[0].set_data(data.time,data.setpoint)
		self.lines1[1].set_data(data.time,data.temperature)
		self.lines2[0].set_data(data.time,data.power)
		self.x1.relim()
		self.x2.relim()
		self.x1.autoscale_view()
		self.x2.autoscale_view()
		self.canvas.draw()
		return True
		
	def save_settings(self,widget,port):
		self.thread.wait(1)
		port.write("save\r")
		port.readline()
		self.thread.wait(0)
		return 
	
	def set_settings(self,widget,port,P,I,D,T):
		self.thread.wait(1)
		port.write("set KP " + str(P.get_value_as_int()) + "\r")
		port.readline()

		port.write("set KI " + str(I.get_value_as_int()) + "\r")
		port.readline()

		port.write("set KD " + str(D.get_value_as_int()) + "\r")
		port.readline()

		port.write("set KT " + str(T.get_value_as_int()) + "\r")
		port.readline()

		self.thread.wait(0)
		return

	def get_settings(self,widget,port,P,I,D,T):
		self.thread.wait(1)
	
		port.write("eeprom\r")
		port.readline()
		words = port.readline().split()
		P.set_value( int(words[1]) )
		words = port.readline().split()
		I.set_value( int(words[1]) )
		words = port.readline().split()
		D.set_value( int(words[1]) )
		words = port.readline().split()
		T.set_value( int(words[1]) )
		port.readline()
		self.thread.wait(0)
		return

	def __init__(self):
		try:
			port = serial.Serial(port='/dev/ttyS0', baudrate=9600, bytesize=8, parity='N', stopbits=1, timeout=0.1)
		except serial.SerialException:
			sys.stderr.write("could not open port\n")
			sys.exit(1)

		port.write("log 1\r")
		port.readline()

		self.queue = Queue.Queue(1)
		self.thread = UpdateData(port,self.queue)
		self.thread.start()

		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.set_default_size(800,600)
		self.window.set_title("PID logger")

		self.window.connect("delete_event",self.delete)
		self.window.connect("destroy",self.destroy)
		
		## matlibplot
		self.figure = Figure(figsize=(5,4),dpi=100)
		self.x1 = self.figure.add_subplot(111)
		self.x2 = self.figure.add_subplot(111,sharex=self.x1,frameon=False)

		self.x1.set_xlabel('time')   
		self.x1.set_ylabel('temperature')   
		self.x1.set_title('temperature profile')   
		self.x1.grid(True)
		self.x1.yaxis.tick_left()
		self.x2.yaxis.tick_right()

		self.canvas = FigureCanvasGTK(self.figure)
		self.canvas.draw()	
		self.canvas.show()

		vbox = gtk.VBox(homogeneous=False, spacing=0)
		hbox0 = gtk.HBox()
		self.window.add(vbox)
		vbox.pack_start(self.canvas,expand=True,fill=True,padding=0)

		separator0 = gtk.HSeparator()
		vbox.pack_start(separator0, expand=False, fill=False, padding=10)

		#PID settings
		PAdjustment = gtk.Adjustment(value=0,lower=0,upper=32,step_incr=1,page_incr=1,page_size=0)
		IAdjustment = gtk.Adjustment(value=0,lower=0,upper=32,step_incr=1,page_incr=1,page_size=0)
		DAdjustment = gtk.Adjustment(value=0,lower=0,upper=32,step_incr=1,page_incr=1,page_size=0)
		TAdjustment = gtk.Adjustment(value=0,lower=0,upper=255,step_incr=1,page_incr=10,page_size=0)

		Plabel = gtk.Label("P")
		Ilabel = gtk.Label("I")
		Dlabel = gtk.Label("D")
		Tlabel = gtk.Label("PID threshold")

		Pspinbutton = gtk.SpinButton(adjustment=PAdjustment,digits=0)
		Ptooltips = gtk.Tooltips()
		Ptooltips.set_tip(Pspinbutton,"Proportional term of the PID function",tip_private=None)
		Ispinbutton = gtk.SpinButton(adjustment=IAdjustment,digits=0)
		Itooltips = gtk.Tooltips()
		Itooltips.set_tip(Ispinbutton,"Integral term of the PID function (in units of 1/100)",tip_private=None)
		Dspinbutton = gtk.SpinButton(adjustment=DAdjustment,digits=0)
		Dtooltips = gtk.Tooltips()
		Dtooltips.set_tip(Dspinbutton,"Derivative term of the PID function",tip_private=None)
		Tspinbutton = gtk.SpinButton(adjustment=TAdjustment,digits=0)
		Ttooltips = gtk.Tooltips()
		Ttooltips.set_tip(Tspinbutton,"Error threshold between PD and PID behaviour of the controller",tip_private=None)

		Pvbox = gtk.VBox()
		Pvbox.pack_start(Plabel)
		Pvbox.pack_start(Pspinbutton)

		Ivbox = gtk.VBox()
		Ivbox.pack_start(Ilabel)
		Ivbox.pack_start(Ispinbutton)

		Dvbox = gtk.VBox()
		Dvbox.pack_start(Dlabel)
		Dvbox.pack_start(Dspinbutton)

		Tvbox = gtk.VBox()
		Tvbox.pack_start(Tlabel)
		Tvbox.pack_start(Tspinbutton)

		hbox0.pack_start(Pvbox,expand=True,fill=False)
		hbox0.pack_start(Ivbox,expand=True,fill=False)
		hbox0.pack_start(Dvbox,expand=True,fill=False)
		hbox0.pack_start(Tvbox,expand=True,fill=False)

		vbox.pack_start(hbox0,expand=False,fill=False,padding=0)
		
		hbox1 = gtk.HBox()
		Setbutton = gtk.Button(label="Set settings",stock=None)
		Setbutton.connect("clicked",self.set_settings,port,Pspinbutton,Ispinbutton,Dspinbutton,Tspinbutton)
		Settooltips = gtk.Tooltips()
		Settooltips.set_tip(Setbutton,"Apply settings to the PID controller",tip_private=None)
		self.get_settings(self,port,Pspinbutton,Ispinbutton,Dspinbutton,Tspinbutton)

		Savebutton = gtk.Button(label="Save settings",stock=None)
		Savetooltips = gtk.Tooltips()
		Savetooltips.set_tip(Savebutton,"Store current settings in the eeprom of the controller",tip_private=None)
		Savebutton.connect("clicked",self.save_settings,port)

		Getbutton = gtk.Button(label="Get settings",stock=None)
		Gettooltips = gtk.Tooltips()
		Gettooltips.set_tip(Getbutton,"Get PID settings from eeprom of the controller",tip_private=None)
		Getbutton.connect("clicked",self.get_settings,port,PAdjustment,IAdjustment,DAdjustment,TAdjustment)

		hbox1.pack_start(Getbutton,expand=True,fill=False,padding=10)
		hbox1.pack_start(Setbutton,expand=True,fill=False,padding=10)
		hbox1.pack_start(Savebutton,expand=True,fill=False,padding=10)
	
		separator1 = gtk.HSeparator()
		vbox.pack_start(separator1,expand=False,fill=False,padding=10)
		vbox.pack_start(hbox1,expand=False,fill=False,padding=0)



		self.lines1 = self.x1.plot([0,0],[0,0],'r-',[0,0],[0,0],'b-')
		self.lines2 = self.x2.plot([0,0],[0,0],'g-')
		self.window.show_all()

	def main(self):
		source_id = gobject.idle_add(self.update)
		gtk.main()
		
if __name__ == "__main__":
	w = Main()
	w.main()
	
