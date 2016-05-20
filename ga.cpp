#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>

#define MAX_PERIODS_PER_WEEK 1100
#define MAX_ROOMS 50
#define MAX_TEACHERS 150
#define POSITIVE_INFINITY 9999
#define EMPTY -1
#define BLOCKED -2
#define labslot 2 	// length of a lab hour

using namespace std;

long long randomoffset;
int elapsedgenerations;

//nperiodsperweek always multiple of 5
int nperiodsperweek, nteachers, nrooms;
int nLabSubjects, nLabs, firstLabRoom, lastLabRoom;
int populationsize, generationlimit;											// Variables that are obtained from
int tournamentsize, tempint;													// the python GUI after input
double mutationrate;
int elitism, crossoversplit, csefaculty;

vector <string> teachers, labTeachers;
map <string, int> teacherid, labTeacherid;


class individual 
{
	public:
		int table[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
		double fitness;															// Each chromosome of population is represented by a class called
																				// individual. The table attribute of object of class holds the master time
		individual () 															// table of the particular chromosome.
		{ fitness = 0; }
};


int initial[MAX_ROOMS][MAX_PERIODS_PER_WEEK];	//all nRooms are id'd with periodcount
int labInitial[MAX_ROOMS][MAX_PERIODS_PER_WEEK];	//different from labInitial in labGa.cpp. how? => has all nRooms and the lab rooms alone are id'd with labPeriodcount  
int availability[MAX_TEACHERS][MAX_PERIODS_PER_WEEK];
int periodcount[MAX_ROOMS][MAX_TEACHERS];
int labPeriodcount[MAX_ROOMS][MAX_TEACHERS];
bool conflicts[MAX_TEACHERS][MAX_TEACHERS];
individual elite;
vector <individual> population;


string return_tID(string labSub)
{
	string tid;
	for(int m = 0; m<labSub.size(); m++)
	{			
		if(labSub[m] == '/')
		{
			tid = labSub.substr(0,m);
			break;
		}
	}
	return tid;
}


int return_roomNo(string labSub)
{
	int room;
	for(int m = 0; m < labSub.size(); m++)
	{
		if(labSub[m] == 'r')
		{
			room = stoi(labSub.substr(m+1));
			break;
		}
	}
	return room;
}


int randomint(int lower, int upper)
{
	srand(time(0)+randomoffset);														// The randomint function takes the upper and lower bound as arguments
	randomoffset = (randomoffset+1)%2823401239LL;									    // and returns a different random value everytime it is called.
	if(upper<lower) return lower;					
	return rand()%(upper-lower+1)+lower;
}


bool randombool(double chance)														// Calculates probability for mutation to occur.
{	
	if(randomint(0,1000000) < (long long) 1000000*chance) return true; else return false;
}


//********* The getminfitnessid() function is used to calculate the fitness of each chromosome in the population. Lower the fitness value, fitter the chromosome. **********//

int getminfitnessid()
{
	double minvalue = POSITIVE_INFINITY;
	int minid = 0, count = 1;
	double tempfitness, first2Hours, confAvail, consecutiveHours, labTheory;

	for(int i = 0; i<population.size(); i++)
	{//cout<<i<<endl;
		tempfitness = 0, first2Hours = 0, confAvail = 0, consecutiveHours = 0, labTheory = 0;
		
		//calculate conflicts
		for(int j = 0; j<nperiodsperweek; j++)
		{
			for(int k = 0; k<nrooms; k++)
			{
				if(population[i].table[k][j] == EMPTY || population[i].table[k][j] == BLOCKED)
					continue;
				else
				{
					for(int l = 0; l<nrooms; l++)
					{
						if(population[i].table[l][j] == EMPTY || population[i].table[l][j] == BLOCKED)
								continue;
						else
						{							
							if(k!=l)
							{							
								if(conflicts[population[i].table[k][j]][population[i].table[l][j]] != 0)			// Checks if the same teacherid shows up in two
									confAvail += 1;																	// rooms during the same hour in the time table.
							}

							if(j == count*nperiodsperweek/5-1 )													
							{	
								++count;																			
							}
							else
							{
								if(population[i].table[k][j] == population[i].table[l][j+1] && population[i].table[k][j]<csefaculty)
									consecutiveHours++;
							}
						}																							// Checks if same faculty has 2 consecutive
					}																								// hours of class
				}
			}
			
			for(int l = 0; l<csefaculty; l++)
			{																										// Checks availability of faculty at
				if(availability[l][j]==0) 																			// specific periods of the day.
					confAvail += 1;
			}
		}

		// constraint for no different subject theory class after a lab
		int sameClass;
		for(int a = firstLabRoom; a <= lastLabRoom; a++)
		{
			for(int b = 0; b <= nperiodsperweek-labslot; b += labslot)
			{
				if(labInitial[a][b-1] != EMPTY)
					sameClass = return_roomNo(labTeachers[labInitial[a][b-1]]);
				else 
					continue;
				
				if(b%(nperiodsperweek/5) != 0)
					for(int c = 0; c < nrooms; c++)
					{
						if((c <= firstLabRoom  ||  c >= lastLabRoom)  &&  c != sameClass)
						{
							if(population[i].table[c][b] == population[i].table[a][b-1])
								labTheory++;
						}
					}
			}
		}


		int firstPeriod, secondPeriod;
		for(int m = 0; m < nrooms; m++)
			for(int n = 0; n < 5; n++)
			{
				firstPeriod = n*nperiodsperweek/5;
				secondPeriod = n*nperiodsperweek/5+1;													
				if(population[i].table[m][firstPeriod] == EMPTY)													// Checks if first 2 hours of each day of
					first2Hours += 1;																				// the week is free.
				if(population[i].table[m][secondPeriod] == EMPTY)	
					first2Hours += 1;
			}
		

		tempfitness = 0.65*confAvail + 0.05*first2Hours + 0.15*consecutiveHours + 0.15*labTheory;
		//cout<<"confAvail : "<<confAvail<<endl;																// Calculates overall fitness of table.
		//cout<<"first2Hours : "<<first2Hours<<endl;
		//cout<<"consecutiveHours : "<<consecutiveHours<<endl;
		//cout<<"labTheory : "<<labTheory<<endl;

		population[i].fitness = tempfitness;
		if(tempfitness<minvalue)																					
		{
			minvalue = tempfitness;
			minid = i;
		}
		
	}

	return minid;																								// Fittest chromosome of population returned to main function
}


// ****** The tournamentselection() function picks the winner(chromosome with lesser fitness value) of two "tournaments" conducted and sends them to the crossover() function ****** //

int tournamentselection()
{
	double tournamentminfitness = POSITIVE_INFINITY;
	int tournamentwinnerid = 0;
	int tempint;
	for(int i = 0; i<tournamentsize; i++)
	{																												

		tempint = randomint(0,population.size()-1);
		if(population[tempint].fitness < tournamentminfitness)
		{
			tournamentminfitness = population[tempint].fitness;													// Tournament size decides selection pressure.
			tournamentwinnerid = tempint;																		// Lesser the size, greater pressure and lesser chance
		}																										// for less fit chromosome to participate in crossover operation.
	}
	return tournamentwinnerid;
}


// **** The crossover() function combines two chromosomes to form a new one causing a crossovered offsprings, which is hoped to have a higher fitness value.
individual crossover(int a, int b)
{
	individual offspring;
	for(int i = 0; i<nrooms; i++)
	{


//cout<< "\n\n " << i ;
/*cout<<"\npopulation a";
cout << "\n\t";
for(int a = 0; a < nrooms; a++)
{
	cout << "r" << a << "\t";
}
for(int p = 0; p < nperiodsperweek; p++)
{
	cout<<endl<<p<<"\t"	;
	for(int q = 0; q < nrooms; q++)
	{
		if(population[a].table[q][p] == EMPTY)
			cout<<"_\t";
		else if(population[a].table[q][p] == BLOCKED)
			cout<<"bkd\t";
		else
			cout<<teachers[population[a].table[q][p]]<<"\t";
	}
}
*/

		vector <int> weekperiod;
		for(int j = 0; j<nperiodsperweek; j++)
		{
			if(initial[i][j] == EMPTY)
			{																										// Weekperiod vector is filled with possible  
				weekperiod.push_back(population[b].table[i][j]);													// entries in the offspring that can be contributed by
			}																										// chromosome 'b'.
		}
		
		//cout<<i;
		/*print weekperiod
		for(int p = 0; p < nperiodsperweek; p++)
			cout<<" "<<weekperiod[i]<<" ";
		cout<<endl<<"c1"<<endl;
		*/

		for(int j = 0; j<nperiodsperweek; j++)
		{//cout<<" "<<j<<endl;

			if(initial[i][j] != EMPTY)// || initial[i][j] == BLOCKED)
			{	//cout<<"c1\n";	
				offspring.table[i][j] = initial[i][j];
				//cout<<"c2"<<endl;
			}
			else 
			{
				if(j < crossoversplit)
				{
					//cout<<"c3"<<endl;
					offspring.table[i][j] = population[a].table[i][j];		
					//cout<<"c4"<<endl;										// Based on the crossover split value, 
					weekperiod.erase(find(weekperiod.begin(),weekperiod.end(),offspring.table[i][j]));				// offspring is formed with first, portion of
					//cout<<"c5"<<endl;																									//'a' chromosome and then a portion of 'b'
				}																									// chromosome.
				else 
				{
					//cout<<"c5"<<endl;
					offspring.table[i][j] = weekperiod[0];
					weekperiod.erase(weekperiod.begin());										
					//cout<<"c6"<<endl;
				}
			}//scout<<"c7"<<endl;
		}
		

/*
	//output offspring
for(int p = 0; p < nperiodsperweek; p++)
{
	cout<<endl<<p<<"\t"	;
	for(int q = 0; q < nrooms; q++)
	{
		if(offspring.table[q][p] == EMPTY)
			cout<<"_\t";
		else if(offspring.table[q][p] == BLOCKED)
			cout<<"bkd\t";
		else
			cout<<teachers[offspring.table[q][p]]<<"\t";
	}
}
*/

	}
	return offspring;
}


// ***** reads the variable values from the csv *****//
void get_variables(string filename = "csv/variables.csv")
{
	ifstream in(filename);
	string line1,var1;
	int val,flag = 0;
	stringstream linestream;

	if(getline(in,line1,'\n'))
	{
		flag++;
		getline(in,var1,',');
		val = stoi(var1);
		nperiodsperweek = val; 	
			
		getline(in,var1,',');
		val = stoi(var1);
		nteachers = val;

		getline(in,var1,',');
		val = stoi(var1);
		csefaculty = val;

		getline(in,var1,',');
		val = stoi(var1);
		nLabSubjects = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		nrooms = val;

		getline(in,var1,',');
		val = stoi(var1);
		nLabs = val;		

		getline(in,var1,',');
		val = stoi(var1);
		firstLabRoom = val-1;

		getline(in,var1,',');
		val = stoi(var1);
		tournamentsize = val;
		
		getline(in,var1,',');
		linestream<<var1;
		linestream>>mutationrate;
		
		getline(in,var1,',');
		val = stoi(var1);
		populationsize = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		generationlimit = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		elitism = val;

		getline(in,var1,',');
		val = stoi(var1);
		crossoversplit = val;
	}
	if(flag == 0)
		cout<<"\nerror: file for variables not found\n";

}


// ***** Reads the periodcount values from the csv file ***** //
void get_periodcount(string filename = "csv/periodcount.csv")
{
	ifstream in;
	string tempstring;
	in.open(filename);
	string var1, var2, line1, line2;
	
	if(getline(in,line1,'\n'))
	{
		for(int i = 0; i < nteachers && in.good(); i++)
		{
			getline(in,line1,'\n');

			stringstream linestream(line1);

			getline(linestream,var1,',');


			tempstring.assign(var1);
			
			teachers.push_back(tempstring);
			teacherid[tempstring] = i;
			
			for(int j = 0; j < nrooms && linestream.good(); j++)
			{
				getline(linestream,var1,',');
				
				int val = stoi(var1);
				periodcount[j][i] = val;
			}
		}
	}
	else cout<<"\n error: file for periodcount not found\n";
}


void get_labPeriodcount(string filename = "csv/labCsv/labPeriodcount.csv")
{
	ifstream in;
	string tempstring;
	in.open(filename);
	string var1, var2, line1, line2;
	
	if(getline(in,line1,'\n'))
	{
		for(int i = 0; i < nLabSubjects && in.good(); i++)
		{
			getline(in,line1,'\n');

			stringstream linestream(line1);

			getline(linestream,var1,',');


			tempstring.assign(var1.begin(),var1.end());
			
			labTeachers.push_back(tempstring);
			labTeacherid[tempstring] = i;
			
			for(int j = 0; j < nLabs && linestream.good(); j++)
			{
				getline(linestream,var1,',');
				
				int val = stoi(var1);

				labPeriodcount[j][i] = val;	

			}
		}
	}
	else cout<<"\n error: file for labPeriodcount not found\n";
}


// **** Reads the initial matrix from the csv file **** //
void get_initial(string filename = "csv/initialLabs.csv")
{
	string var1, line1, tempstring, tId;
	int roomNo;
	ifstream infile(filename);

	if(getline(infile,line1,'\n'))
	{
		for(int i = 0; i < nperiodsperweek && infile.good(); i++)
		{
			getline(infile,line1,'\n');		// to ignore the first line of room nos
   			stringstream linestream(line1);			
			getline(linestream,var1,',');	

			for(int j = 0; j < nrooms; j++)
			{

     			getline(linestream,var1,',');
				
				//cout<<var1<<"\t";

				// to get rid of error caused by extra '\n' in final token from a line in the file
				//	tempstring.assign(var1); is not used
				tempstring.assign(var1.begin(),var1.end());

				if(tempstring == "_")
					initial[j][i] = EMPTY;
				else
				{
					if(j >= firstLabRoom && j <= lastLabRoom)
					{						
						tId = return_tID(tempstring);
						roomNo = return_roomNo(tempstring);
						initial[roomNo-1][i] = BLOCKED;
						initial[j][i] = teacherid[tId];
					}	
					else
						initial[j][i] = teacherid[tempstring];
				}
			}
		}
	}
	else cout<<"\n error: file for initial matrix not found";
}


// **** Reads the initial matrix from the csv file **** //
void get_labInitial(string filename = "csv/initialLabs.csv")
{
	string var1, line1, tempstring;
	int roomNo;
	ifstream infile(filename);

	if(getline(infile,line1,'\n'))
	{
		for(int i = 0; i < nperiodsperweek && infile.good(); i++)
		{//cout<<i<<endl;

			getline(infile,line1,'\n');	// to ignore the initial line of room nos
   			stringstream linestream(line1);			
			getline(linestream,var1,',');	

			for(int j = 0; j < nrooms; j++)
			{
     			getline(linestream,var1,',');
				
				// to get rid of error caused by extra '\n' in final token from a line in the file
				//if(j == nrooms-1)
					tempstring.assign(var1.begin(),var1.end());
				//else
					//tempstring.assign(var1);	//can remove this line completely
				
				//cout<<tempstring<<endl;
				
				if(tempstring == "_")
				{
					labInitial[j][i] = EMPTY;
				}
				else
				{
					if(j >= firstLabRoom && j <= lastLabRoom)
					{
						//cout<<j<<" "<<i<<" "<<tempstring<<" \n";
						//tId = return_tID(tempstring);
						roomNo = return_roomNo(tempstring);
						labInitial[roomNo-1][i] = BLOCKED;
						labInitial[j][i] = labTeacherid[tempstring];
					}	
					else
					{
						labInitial[j][i] = teacherid[tempstring];
					}	
						
				}					
			}
		}
	}
	else cout<<"\n error: file for initial matrix not found";
}


void write_output(int output_flag, int minid, string filename = "csv/output.csv")
{
	
	ofstream out;
	out.open(filename);

	out << "p_no,";
	for(int i = 0; i < nrooms; i++)
	{
		out << "r" << i+1;
		if( i != nrooms-1)
			out << ",";
		else
			out << "\n";
	}

	for(int i = 0; i < nperiodsperweek; i++)
	{
		out << "p" << i+1 << ",";
		for(int j = 0; j < nrooms; j++)
		{
			if(population[minid].table[j][i] == EMPTY)
				out << "_";
			else if(population[minid].table[j][i] == BLOCKED)
				out << "BKD";
			else if(j >= firstLabRoom && j <= lastLabRoom)
				out << labTeachers[labInitial[j][i]];
			else
				out << teachers[population[minid].table[j][i]];
		
			if( j != nrooms-1)
				out << ",";
			else
				out << "\n";
		}
	}
	out.close();

	if(output_flag)
		cout<<"\n\nOutput written to : " << filename;
	else	
		return;
	
}




int main()
{
	randomoffset = 0;
	get_variables();
	//string tempstring;
	lastLabRoom = firstLabRoom+nLabs-1;
	//cout<<firstLabRoom<<" "<<lastLabRoom<<" "<<nrooms<<endl;
	//cout << crossoversplit;
	get_periodcount();
	get_labPeriodcount();
	get_initial();
	get_labInitial();

/*
	output initial and periodcount matrices

	for(int i = 0; i < nteachers; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j< nrooms; j++)
		{
			cout<<periodcount[j][i]<<" ";
		}
	}
	cout<<endl<<endl<<endl;

*/	
	/*// print labteachers
	for(int i = 0; i < labTeachers.size(); i++)
		cout<< i << " " << labTeachers[i]<< " \n";
	*/

	cout<<"\ninitial matrix : \n";
	for(int i = 0; i < nperiodsperweek; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j< nrooms; j++)
		{
			if(initial[j][i] == EMPTY)
				cout<<"_\t";
			else if(initial[j][i] == BLOCKED)
				cout<<"BKD\t";
			else	
			{
				if(j >= firstLabRoom && j <= lastLabRoom)
					{
						cout<<labTeachers[labInitial[j][i]]<<"\t";
						//cout<<teachers[initial[j][i]]<<"\t";
					}	
				else	
					cout<<teachers[initial[j][i]]<<"\t";
			}	
		}
	}

	//return 0;}
	

	for(int i = 0; i<nteachers; i++)
	{
		for(int j = 0; j<nperiodsperweek; j++)
		{
				availability[i][j] = 1;
		}
	}
	

	for(int i = 0; i<nteachers; i++)
	{
		for(int j = 0; j<nteachers; j++)
		{
			if(i!=j)
				conflicts[i][j] = false;
			else 
				conflicts[i][j] = true;
		}
	}
	
	cout << "\nFinished input" << endl;
	
	//insert to population
	for(int i = 0; i<populationsize; i++)
	{
		individual newindividual;
		vector <int> weekperiod;
		for(int j = 0; j<nrooms; j++)
		{
			for(int k = 0; k<nteachers; k++)
			{
				weekperiod.insert(weekperiod.end(), periodcount[j][k],k);
			}

			for(int k = 0; k < nperiodsperweek/5 ; k++)
			{
				for(int l = k; l < nperiodsperweek; l += nperiodsperweek/5)
				{
					if(initial[j][l] == EMPTY && weekperiod.size()>0)	//Enter teacher id's in empty slots of initial while weekperiod has elements left.
					{
						tempint = randomint(0,weekperiod.size()-1);
						newindividual.table[j][l] = weekperiod[tempint];
						weekperiod.erase(weekperiod.begin()+tempint);
					}
					else 
						newindividual.table[j][l] = initial[j][l];	//copy elements from initial when week period is empty
				}	
			}	
		}
		population.push_back(newindividual);
	}

/*	
	//	Old insert to population 
	for(int i = 0; i<populationsize; i++)
	{
		individual newindividual;
		vector <int> weekperiod;
		for(int j = 0; j<nrooms; j++)
		{
			for(int k = 0; k<nteachers; k++)
			{
				weekperiod.insert(weekperiod.end(), periodcount[j][k],k);
			}

			for(int k = 0; k<nperiodsperweek; k++)
			{
				
				if(initial[j][k] == EMPTY && weekperiod.size()>0)	//Enter teacher id's in empty slots of initial while weekperiod has elements left.
				{
					tempint = randomint(0,weekperiod.size()-1);
					newindividual.table[j][k] = weekperiod[tempint];
					weekperiod.erase(weekperiod.begin()+tempint);
				}
				else 
					newindividual.table[j][k] = initial[j][k];	//copy elements from initial when week period is empty
			}	
		}
		population.push_back(newindividual);
*/
/*
//display individual for checking
	cout<<endl<<endl<<i<<endl;
	for(int k = 0; k<nperiodsperweek; k++)
	{
		for(int j = 0; j<nrooms; j++)
		{
			if(population[i].table[j][k] == EMPTY)
				cout<<"_\t";
			else if(population[i].table[j][k] == BLOCKED)
				cout<<"BKD\t";
			else
				cout << teachers[population[i].table[j][k]] << "\t";
		}
		cout << endl;
	}	


}
*/	


	//algorithm
	cout << "Starting genetic algorithm..." << endl;
		
	elapsedgenerations = 0;
	int elitismoffset = 0;
	
	if(elitism) 
		elitismoffset = 1;
	
	while(elapsedgenerations < generationlimit)
	{
		vector <individual> newpopulation;
		
		//compute fitness, find minimum
		int minid = getminfitnessid();
		
		//output stuff
		write_output(0,minid);	
		/*cout << endl << "fitness: " << population[minid].fitness << endl;

	for(int i = 0; i <= nrooms; i++)
		cout << "r" << i << "\t";
	cout << endl << endl;

	for(int i = 0; i<nperiodsperweek; i++)
	{	cout << "p" << i+1 << "\t";
		for(int j = 0; j<nrooms; j++)
		{
			if(population[minid].table[j][i] == EMPTY)
				cout<<"_\t";
			else if(population[minid].table[j][i] == BLOCKED)
				cout<<"BKD\t";
			else if(j >= firstLabRoom && j <= lastLabRoom)
				cout << labTeachers[labInitial[j][i]] << "\t";
			else
				cout << teachers[population[minid].table[j][i]] << "\t";
		}
		cout << endl;
	}
*/



		double minvalue = population[minid].fitness;
		if(elitism)																		// if Eltisim = 1, it algorithm will isolate the fittest chromosome of
		{																				// population from crssover and mutation processes.
			newpopulation.push_back(population[minid]);
		}
			
			//cout<<"shit0\n";
		
/*
//print initial	
	for(int i = 0; i < nperiodsperweek; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j< nrooms; j++)
		{
			if(initial[j][i] == EMPTY)
				cout<<"_\t";
			else if(initial[j][i] == BLOCKED)
				cout<<"BKD\t";
			else	
			{
				if(j >= firstLabRoom && j <= lastLabRoom)
					{
						cout<<teachers[initial[j][i]]<<"\t";
					}	
				else	
					cout<<teachers[initial[j][i]]<<"\t";
			}	
		}
	}

elapsedgenerations++;}
*/
		//crossover;
		for(int i = elitismoffset; i<population.size(); i++)
		{//cout<<"shit1\n";
			int a = tournamentselection();
			int b = tournamentselection();
			//cout<<"shit2\n";
			individual offspring = crossover(a,b);
			//cout<<"shit3\n";
			newpopulation.push_back(offspring);
		}
//cout<<"hi\n";
		
		//mutate;
		for(int i = elitismoffset; i<population.size(); i++)
		{
			for(int j = 0; j<nrooms; j++)
			{
				if(randombool(mutationrate))
				{
					int a, b;
					do 
					{//cout<<"shit4\n";
						a = randomint(0,nperiodsperweek-1);
						b = randomint(0,nperiodsperweek-1);
					} while(initial[j][a] != EMPTY || initial[j][a] == BLOCKED || initial[j][b] != EMPTY || initial[j][b] == BLOCKED);
					swap(newpopulation[i].table[j][a],newpopulation[i].table[j][b]);
				}
			}
		}
		
		population = newpopulation;

		elapsedgenerations++;
		
		cout << "Computed generation " << elapsedgenerations << ", minimum fitness at start: " << minvalue << endl;
	}
	
	
	int minid = getminfitnessid();
	
	cout << endl << "RESULT (fitness: " << population[minid].fitness << ")" << endl;

	for(int i = 0; i <= nrooms; i++)
		cout << "r" << i << "\t";
	cout << endl << endl;

	for(int i = 0; i<nperiodsperweek; i++)
	{	cout << "p" << i+1 << "\t";
		for(int j = 0; j<nrooms; j++)
		{
			if(population[minid].table[j][i] == EMPTY)
				cout<<"_\t";
			else if(population[minid].table[j][i] == BLOCKED)
				cout<<"BKD\t";
			else if(j >= firstLabRoom && j <= lastLabRoom)
				cout << labTeachers[labInitial[j][i]] << "\t";
			else
				cout << teachers[population[minid].table[j][i]] << "\t";
		}
		cout << endl;
	}
	
	write_output(1,minid);

	cout << endl << endl;
	return 0;
}
