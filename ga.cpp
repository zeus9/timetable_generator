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


using namespace std;

long long randomoffset;
int elapsedgenerations;

//nperiodsperweek always multiple of 5
int nperiodsperweek, nteachers, nrooms;
int populationsize, generationlimit;											// Variables that are obtained from
int tournamentsize, tempint;													// the python GUI after input
double mutationrate;
int elitism, crossoversplit, csefaculty;

vector <string> teachers;
map <string, int> teacherid;


class individual 
{
	public:
		int table[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
		double fitness;															// Each chromosome of population is represented by a class called
																				// individual. The table attribute of object of class holds the master time
		individual () 															// table of the particular chromosome.
		{ fitness = 0; }
};


int initial[MAX_ROOMS][MAX_PERIODS_PER_WEEK];								
int availability[MAX_TEACHERS][MAX_PERIODS_PER_WEEK];
int periodcount[MAX_ROOMS][MAX_TEACHERS];
bool conflicts[MAX_TEACHERS][MAX_TEACHERS];
individual elite;
vector <individual> population;

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
	
	for(int i = 0; i<population.size(); i++)
	{
		double tempfitness = 0, first2Hours = 0, confAvail = 0, consecutiveHours = 0;
		
		//calculate conflicts
		for(int j = 0; j<nperiodsperweek; j++)
		{
			for(int k = 0; k<nrooms; k++)
			{
				if(population[i].table[k][j] == EMPTY)
					continue;
				else
				{
					for(int l = 0; l<nrooms; l++)
					{
						if(population[i].table[l][j] == EMPTY)
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
		

		tempfitness = 0.7*confAvail + 0.1*first2Hours + 0.2*consecutiveHours;
		//cout<<"confAvail : "<<confAvail<<endl;																// Calculates overall fitness of table.
		//cout<<"first2Hours : "<<first2Hours<<endl;
		//cout<<"consecutiveHours : "<<consecutiveHours<<endl;


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
		vector <int> weekperiod;
		for(int j = 0; j<nperiodsperweek; j++)
		{
			if(initial[i][j] == EMPTY)
			{																										// Weekperiod vector is filled with possible  
				weekperiod.push_back(population[b].table[i][j]);													// entries in the offspring that can be contributed by
			}																										// chromosome 'b'.
		}
		
		for(int j = 0; j<nperiodsperweek; j++)
		{
			if(initial[i][j] != EMPTY)
			{
				offspring.table[i][j] = initial[i][j];
			}
			
			else 
			{
				if(j<crossoversplit)
				{
					offspring.table[i][j] = population[a].table[i][j];												// Based on the crossover split value, 
					weekperiod.erase(find(weekperiod.begin(),weekperiod.end(),offspring.table[i][j]));				// offspring is formed with first, portion of
																													//'a' chromosome and then a portion of 'b'
				}																									// chromosome.
				
				else 
				{
					offspring.table[i][j] = weekperiod[0];
					weekperiod.erase(weekperiod.begin());										
				}
			}
		}
		
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
		nrooms = val;
		
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

		getline(in,var1,',');
		val = stoi(var1);
		csefaculty = val;

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
			
			for(int j = 0; j < nrooms && in.good(); j++)
			{
				getline(linestream,var1,',');
				
				int val = stoi(var1);
				periodcount[j][i] = val;

			}
		}
	}
	else cout<<"\n error: file for periodcount not found\n";
}


// **** Reads the initial matrix from the csv file **** //
void get_initial(string filename = "csv/initial.csv")
{
	string var1, line1, tempstring;
	ifstream infile(filename);

	if(getline(infile,line1,'\n'))
	{
		for(int i = 0; i < nperiodsperweek && infile.good(); i++)
		{

			getline(infile,line1,'\n');	// to ignore the initial token of hour no. in the week
   			stringstream linestream(line1);			
			getline(linestream,var1,',');	

			for(int j = 0; j < nrooms; j++)
			{

     			getline(linestream,var1,',');
				
				// to get rid of error caused by extra '\n' in final token from a line in the file
				if(j == nrooms-1)
					tempstring.assign(var1.begin(),var1.end());
				else
					tempstring.assign(var1);
				

				if(tempstring == "_")
					initial[j][i] = EMPTY;
				else
					initial[j][i] = teacherid[tempstring];
			}
		}
	}
	else cout<<"\n error: file for initial matrix not found";
}


int main()
{
	randomoffset = 0;
	get_variables();
	string tempstring;
	
	get_periodcount();
	
	get_initial();

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


	for(int i = 0; i < nrooms; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j< nperiodsperweek; j++)
		{
			cout<<initial[i][j]<<" ";
		}
	}
*/

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
	
	cout << "Finished input" << endl;
	
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

/*
//display individual for checking
	for(int k = 0; k<nperiodsperweek; k++)
	{
		for(int j = 0; j<nrooms; j++)
		{
			if(population[i].table[j][k] == EMPTY)
				cout<<"_\t";
			else
				cout << teachers[population[i].table[j][k]] << "\t";
		}
		cout << endl;
	}
*/
	
	}
	


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
		double minvalue = population[minid].fitness;
		if(elitism)																		// if Eltisim = 1, it algorithm will isolate the fittest chromosome of
		{																				// population from crssover and mutation processes.
			newpopulation.push_back(population[minid]);
		}
			
			
		//crossover;
		for(int i = elitismoffset; i<population.size(); i++)
		{
			int a = tournamentselection();
			int b = tournamentselection();
			individual offspring = crossover(a,b);
			newpopulation.push_back(offspring);			
		}

		
		//mutate;
		for(int i = elitismoffset; i<population.size(); i++)
		{
			for(int j = 0; j<nrooms; j++)
			{
				if(randombool(mutationrate))
				{
					int a, b;
					do 
					{
						a = randomint(0,nperiodsperweek-1);
						b = randomint(0,nperiodsperweek-1);
					} while((initial[j][a]!=EMPTY) || (initial[j][b]!=EMPTY));
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

	for(int i = 0; i<nperiodsperweek; i++)
	{
		for(int j = 0; j<nrooms; j++)
		{
			if(population[minid].table[j][i] == EMPTY)
				cout<<"_\t";
			else
				cout << teachers[population[minid].table[j][i]] << "\t";
		}
		cout << endl;
	}
	
return 0;

}
