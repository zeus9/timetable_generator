from Tkinter import *
import csv

def csv_maker(periods,teachers,rooms,Tsize,rate,Psize,limit,elite,C_Split,C_Faculty,lab_subs,labs,first_lab,Llimit,Lpsize,Ltsize,Lmrate,Lelite,Lc_split):
    filename = 'csv/variables.csv'
    filename2 = 'csv/labCsv/labVariables.csv'
    with open(filename,'wb') as csvfile:
        fields = ['No_of_periods_per_week','No_of_teachers','CSE_faculty_count','No_of_lab_subjects','No_of_rooms','No_of_labs','First_Lab_Room_No','Tournament_size','Mutation_rate','Population_size','Generation_limit', 'Elitism','Crossover_split']
        file_writer = csv.DictWriter(csvfile,fieldnames=fields)
        file_writer.writeheader()
        file_writer.writerow({'No_of_periods_per_week':periods,'No_of_teachers':teachers,'CSE_faculty_count':C_Faculty,'No_of_lab_subjects':lab_subs,'No_of_rooms':rooms,'No_of_labs':labs,'First_Lab_Room_No':first_lab,'Tournament_size':Tsize,'Mutation_rate':rate,'Population_size':Psize,'Generation_limit':limit,'Elitism':elite,'Crossover_split':C_Split})

    with open(filename2,'wb') as csvfile:
        fields = ['No_of_periods_per_week','No_of_teachers','No_of_lab_subjects','No_of_rooms','No_of_labs','First_Lab_Room_No','Tournament_size','Mutation_rate','Population_size','Generation_limit','Elitism','Crossover_split']
        file_writer = csv.DictWriter(csvfile,fieldnames=fields)
        file_writer.writeheader()
        file_writer.writerow({'No_of_periods_per_week':periods,'No_of_teachers':teachers,'No_of_lab_subjects':lab_subs,'No_of_rooms':rooms,'No_of_labs':labs,'First_Lab_Room_No':first_lab,'Tournament_size':Ltsize,'Mutation_rate':Lmrate,'Population_size':Lpsize,'Generation_limit':Llimit,'Elitism':Lelite,'Crossover_split':Lc_split})

    master.destroy()

        
master = Tk()


w = Canvas(master, width=1600, height=900)

Lhead1 = Label(master, text = "DEPARTMENT PARAMETERS", font = ("Helvetica",10,"bold"))
Lhead1.place(x=560,y=10)

L1 = Label(master, text = "No. of periods per week")
L1.place(x=60,y=50)
E1 = Entry(master, bd = 3)
E1.insert(0,30)
E1.place(x=60,y=70)

L2 = Label(master, text= "No. of teachers")
L2.place(x=300,y=50)
E2 = Entry(master, bd = 3)
E2.insert(0,55)
E2.place(x=300,y=70)

L10 = Label(master, text = "CSE Faculty Count")
L10.place(x=540,y=50)
E10 = Entry(master, bd=3)
E10.insert(0,43)
E10.place(x=540,y=70)

L3 = Label(master, text = "No. of rooms")
L3.place(x=60,y=120)
E3 = Entry(master, bd=3)
E3.insert(0,20)
E3.place(x=60,y=140)

L12 = Label(master, text = "No. of labs")
L12.place(x=60,y=190)
E12 = Entry(master, bd=3)
E12.insert(0,4)
E12.place(x=60,y=210)

L13 = Label(master, text = "First lab room no.")
L13.place(x=300,y=190)
E13 = Entry(master, bd=3)
E13.insert(0,16)
E13.place(x=300,y=210)

L11 = Label(master, text = "No. of lab subjects")
L11.place(x=540,y=190)
E11 = Entry(master, bd = 3)
E11.insert(0,40)
E11.place(x=540,y=210)

Lhead2 = Label(master, text = "GENETIC ALGORITHM PARAMETERS", font = ("Helvetica",10,"bold"))
Lhead2.place(x=550,y=310)

Lx = Label(master, text="THEORY")
Lx.place(x=300,y=350)

Ly = Label(master, text="LAB")
Ly.place(x=580,y=350)

L7 = Label(master, text = "Generation limit")
L7.place(x=60,y=400)
E7 = Entry(master, bd=3)
E7.insert(0,100)
E7.place(x=300,y=400)

L6 = Label(master, text = "Population size")
L6.place(x=60,y=440)
E6 = Entry(master, bd=3)
E6.insert(0,200)
E6.place(x=300,y=440)

L4 = Label(master, text = "Tournament size")
L4.place(x=60,y=480)
E4 = Entry(master,bd=3)
E4.insert(0,4)
E4.place(x=300,y=480)

L5 = Label(master, text = "Mutation rate")
L5.place(x=60,y=520)
E5 = Entry(master, bd=3)
E5.insert(0,0.05)
E5.place(x=300,y=520)

L8 = Label(master, text = "Elitism")
L8.place(x=60,y=560)
E8 = Entry(master, bd=3)
E8.insert(0,1)
E8.place(x=300,y=560)

L9 = Label(master, text = "Crossover Split")
L9.place(x=60,y=600)
E9 = Entry(master, bd=3)
E9.insert(0,16)
E9.place(x=300,y=600)

E14 = Entry(master, bd=3)
E14.insert(0,100)                       #Gen. limit
E14.place(x=580,y=400)

E15 = Entry(master, bd=3)
E15.insert(0,200)                        #Pop. size
E15.place(x=580,y=440)

E16 = Entry(master, bd=3)
E16.insert(0,2)                        #Tournament size
E16.place(x=580,y=480)

E17 = Entry(master, bd=3)
E17.insert(0,0.1)                       #Mutation rate
E17.place(x=580,y=520)

E18 = Entry(master, bd=3)
E18.insert(0,1)                         #Elitism
E18.place(x=580,y=560)

E19 = Entry(master, bd=3)
E19.insert(0,8)                         #Crossover split
E19.place(x=580,y=600)


Ltemp1 = Label(master, text ="/", font = ("Times",25))
Ltemp1.place(x=416,y=590)

Ltemp2 = Label(master, text ="No. of peroids per week", font = ("Times",10))
Ltemp2.place(x=430,y=603)

Ltemp3 = Label(master, text ="/", font = ("Times",25))
Ltemp3.place(x=704,y=590)

Ltemp4 = Label(master, text ="No. of peroids per week/2", font = ("Times",10))
Ltemp4.place(x=718,y=603)


submit=Button(master, text = "Submit", command = lambda: csv_maker(E1.get(),E2.get(),E3.get(),E4.get(),E5.get(),E6.get(),E7.get(),
                                                                   E8.get(),E9.get(),E10.get(),E11.get(),E12.get(),E13.get(),E14.get(),E15.get(),
                                                                   E16.get(),E17.get(),E18.get(),E19.get()))

submit.place(x=600, y=660)
#submit.pack()


w.pack()

mainloop()
