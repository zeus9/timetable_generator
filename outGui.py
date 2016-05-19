''' 
This is a makeshift gui to display the final master timetable
The input data values are hardcoded
'''

from Tkinter import *
import csv

rlist = []
hlist = []

def csv_reader(filename = 'csv/output.csv'):
    with open(filename,'r') as csvfile:
        spamreader = csv.reader(csvfile)
        for row in spamreader:
            #for i in range(1, 20):
            #    rlist.append(row['r'+i])
            hlist.append(row)

master = Tk()
w = Canvas(master, width=1600, height=900)

while(1):
    csv_reader()

    L = Label(master, text = "Master Timetable")
    L.place(x = 620, y = 8)

    b = 15
    for i in range(0, 31):
        a = 0
        b = b + 21
        for j in range(0, 21):
            a = a + 60
            L1 = Label(master, text = hlist[i][j])
            L1.place(x = a, y = b)
     

    w.delete("all")
    w.pack()
    mainloop()


