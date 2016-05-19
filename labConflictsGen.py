import csv

idlist = []
roomlist = []

def csv_reader(filename = 'csv/labCsv/labPeriodcount.csv'):
    with open(filename,'r') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            tid = row['id'].split('/',1)
            idlist.append(tid[0])
            roomlist.append(tid[1])
            
def csv_maker(filename = 'csv/labCsv/labConflicts.csv'):
    with open(filename,'wb') as csvfile:
#        writer = csv.DictWriter(csvfile,fieldnames = idlist)
        writer = csv.writer(csvfile, delimiter = ',')
#        writer.writeheader()
        for tid1,room1 in zip(idlist,roomlist):
            rowline = []
            for tid2,room2 in zip(idlist,roomlist):
                if tid1 == tid2 or room1 == room2:
                    rowline.append('1')
                else:
                    rowline.append('0')
            writer.writerow(rowline)

csv_reader()
csv_maker()

