from Tkinter import *
import csv

def csv_maker(periods,teachers,rooms,Tsize,rate,Psize,limit,elite):
    filename = 'csv/variables.csv'
    with open(filename,'wb') as csvfile:
        fields = ['No_of_periods_per_week','No_of_teachers','No_of_rooms','Tournament_size','Mutation_rate','Population_size','Generation_limit', 'Elitism']
        file_writer = csv.DictWriter(csvfile,fieldnames=fields)
        file_writer.writeheader()
        file_writer.writerow({'No_of_periods_per_week':periods,'No_of_teachers':teachers,'No_of_rooms':rooms,'Tournament_size':Tsize,'Mutation_rate':rate,'Population_size':Psize,'Generation_limit':limit,'Elitism':elite})
        master.destroy()
    #c = csv.writer(open("test.csv","wb"))

master = Tk()
#mat1 = Tk()

w = Canvas(master, width=1600, height=900)

L1 = Label(master, text = "No. of periods per week")
L1.place(x=60,y=0)
E1 = Entry(master, bd = 3)
E1.insert(0,50)
E1.place(x=60,y=20)

L2 = Label(master, text= "No. of teachers")
L2.place(x=300,y=0)
E2 = Entry(master, bd = 3)
E2.insert(0,21)
E2.place(x=300,y=20)

L3 = Label(master, text = "No. of rooms")
L3.place(x=540,y=0)
E3 = Entry(master, bd=3)
E3.insert(0,3)
E3.place(x=540,y=20)

L4 = Label(master, text = "Tournament size")
L4.place(x=60,y=70)
E4 = Entry(master,bd=3)
E4.insert(0,2)
E4.place(x=60,y=90)

L5 = Label(master, text = "Mutation rate")
L5.place(x=300,y=70)
E5 = Entry(master, bd=3)
E5.insert(0,0.05)
E5.place(x=300,y=90)

L6 = Label(master, text = "Population size")
L6.place(x=540,y=70)
E6 = Entry(master, bd=3)
E6.insert(0,100)
E6.place(x=540,y=90)

L7 = Label(master, text = "Generation limit")
L7.place(x=60,y=140)
E7 = Entry(master, bd=3)
E7.insert(0,100)
E7.place(x=60,y=160)


L8 = Label(master, text = "Elitism")
L8.place(x=300,y=140)
E8 = Entry(master, bd=3)
E8.insert(0,1)
E8.place(x=300,y=160)

submit=Button(master, text = "Submit", command = lambda: csv_maker(E1.get(),E2.get(),E3.get(),E4.get(),E5.get(),E6.get(),E7.get(),E8.get()))
submit.place(x=600, y=650)
#submit.pack()

w.pack()

mainloop()


