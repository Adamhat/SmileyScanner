import serial
import time
from tkinter import *
from tkinter.ttk import *
from threading import Thread
from datetime import datetime
import os


class Controller(object):
    def __init__(self, arduino_port, baud, fileName):
        self.fileName = fileName
        self.playing = False
        self.ser = serial.Serial(arduino_port, baud)
        print("Connected to Arduino port: " + arduino_port)
        self.file = open(self.fileName, "a")
        print("Created file")
        self.data = str(self.ser.readline())[2:][:-5]
        dt_string2 = now.strftime(",PROGRAM START: %H:%M:%S")
        print(self.data + ",Time Taken" + dt_string2)
        self.file.write(self.data + ",Time Taken" + dt_string2 + "\n")
        self.file.close()

    def record(self):
        if self.playing:
            self.stop()
        else:
            self.file = open(self.fileName, "a")
            self.playing = True

        def play():
            print("Started Data Collection")
            while self.playing:
                self.data = str(self.ser.readline())[2:][:-5]
                now = datetime.now()
                dt_string2 = now.strftime(",%H:%M:%S")
                print(self.data + dt_string2)
                self.file.write(self.data + dt_string2 + "\n")

                time.sleep(1)

        self.thread = Thread(target=play)
        self.thread.start()

    def stop(self):
        if self.playing:
            self.playing = False
            print("Data collection complete!")
            self.thread.join(0.01)
            self.file.close()
            os._exit(0)


master = Tk()
master.geometry("250x150")

now = datetime.now()
dt_string = now.strftime("%d-%m-%Y.csv")
controller = Controller("COM8", 9600, dt_string)

master.title("Main Menu")

label = Label(master,
              text="Transmitter/Receiver")

label.pack(pady=10)

btn = Button(master,
             text="Record Data",
             command=controller.record)

btn2 = Button(master,
              text="Stop",
              command=controller.stop)

btn.pack(pady=10)

btn2.pack(pady=10)

mainloop()
