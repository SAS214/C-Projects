/* COP 3502C Assignment 1
This program is written by: Shivam Shah */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"
#define MAXLENGTH 50

typedef struct monster 
{
    char *name;
    char *element; 
    int population;
}monster;

typedef struct region
{
    char *name; 
    int nmonsters;
    int total_population;
    monster **monsters;
}region;

typedef struct itinerary
{
    int nregions;
    region **regions; 
    int captures;
}itinerary;

typedef struct trainer 
{ 
    char *name; 
    itinerary *visits;
}trainer;

monster* createMonster(char *name, char *element, int population)
{
  monster *m = (struct monster*)malloc(sizeof(struct monster));
  int nlength = strlen(name)+1;
  m->name = (char*)malloc(nlength*sizeof(char));
  strcpy(m->name,name);
  int elength = strlen(element)+1;
  m->element = (char*)malloc(elength*sizeof(char));
  strcpy(m->element, element);
  m->population = population;
  
  return m;
}

monster** readMonsters(FILE* infile, int *monsterCount)
{
  //dynamically allocating array of poitners to structure..
  char name[MAXLENGTH];
  char element[MAXLENGTH];
  int p;
  fscanf(infile,"%d", monsterCount);
  fscanf(infile,"%s", name);// to skip monster
  
  monster** monsterList = (struct monster**)malloc(*monsterCount*(sizeof(struct monster*)));

  for(int i =0 ; i< *monsterCount; i++)
  {
    //minfo[i] = malloc(sizeof(monster*)); // added this line
    fscanf(infile, "%s", name);
    fscanf(infile,"%s" ,element);
    fscanf(infile, "%d", &p);
    monsterList[i] = createMonster(name, element, p);
  }
  return monsterList;
}

region** readRegions(FILE* infile, int *countRegions, monster** monsterList, int monsterCount)
{
  char rname[MAXLENGTH],mname[MAXLENGTH];
  int nm; //num monsters
  int tp; // total population
  fscanf(infile,"%d", countRegions);
  fscanf(infile,"%s", rname);// move after region
  
  region **rinfo = (struct region**)malloc(*countRegions*(sizeof(struct region*)));
  
  
  for(int i = 0; i<*countRegions; i++)
  {
    fscanf(infile,"%s", rname); //for rome
    int rlength = strlen(rname)+1;
    
    rinfo[i] = malloc(sizeof(region));
    rinfo[i]->name = malloc(rlength*sizeof(char));

    strcpy(rinfo[i]->name,rname);
    
    fscanf(infile,"%d", &nm);   // for # of monsters
    rinfo[i]->nmonsters = nm;
    fscanf(infile,"%s", mname); // to skip the monster word
    int hold  = 0;//total population
    
    rinfo[i]->monsters = malloc(nm*sizeof(monsterList));
    
    
    for(int j = 0; j<nm; j++) // scan the file monster certain amount of time
    {
      fscanf(infile,"%s", mname); 
      for(int k = 0; k<monsterCount; k++) //then check it 8 times
      {
        if(strcmp(mname,monsterList[k]->name)==0)
        {
          rinfo[i]->monsters[j] = monsterList[k];//creating seg fault(fixed) 
          hold += monsterList[k]->population; // was j changed to i changed to k
        }
      }
    } 
    rinfo[i]->total_population = hold;
    
  }
  
  return rinfo;
}

trainer* readTrainers(FILE* infile, int *trainerCount, region** regionList, int countRegions)
{
  int nc, nr;
  fscanf(infile,"%d", trainerCount);
  char tname[MAXLENGTH],reg[MAXLENGTH], waste[MAXLENGTH];
  fscanf(infile,"%s", tname); //skip trainer

  trainer *tinfo = (struct trainer*)malloc(*trainerCount*sizeof(struct trainer));

  for(int i = 0; i<*trainerCount; i++)
  {
    fscanf(infile,"%s",tname);
    int tlength = strlen(tname)+1;
    tinfo[i].name = (char*)malloc(tlength*sizeof(char));
    strcpy(tinfo[i].name,tname);
    
    tinfo[i].visits = malloc(*trainerCount*sizeof(itinerary));

    fscanf(infile,"%d %s", &nc, waste);
    tinfo[i].visits[i].captures = nc;//seg fault (fixed)

    fscanf(infile,"%d %s",&nr, waste);
    tinfo[i].visits[i].nregions = nr;
    
    tinfo[i].visits[i].regions = malloc(nr*sizeof(regionList));
    for(int j = 0; j<nr; j++)
    {
      
      fscanf(infile,"%s",reg);
      for(int k = 0; k<countRegions; k++)
      {
        if(strcmp(reg,regionList[k]->name)==0)
        {
          tinfo[i].visits[i].regions[j] = regionList[k];
        }
      }
    }
  }
  return tinfo;
}

void process_inputs(monster** monsterList, int monsterCount, region** regionList, int regionCount, trainer* trainerList, int trainerCount)
{
  float nm,an;
  int real;

  for(int i = 0; i<trainerCount; i++)
  {
    printf("%s\n", trainerList[i].name);
    for(int j = 0 ; j<trainerList[i].visits[i].nregions; j++)
    {
      printf("%s\n",trainerList[i].visits[i].regions[j]->name);
      
      for(int k = 0; k<trainerList[i].visits[i].regions[j]->nmonsters; k++)
      {
        nm = trainerList[i].visits[i].regions[j]->monsters[k]->population;
        an = trainerList[i].visits[i].regions[j]->total_population;

        real = round((nm/an)*trainerList[i].visits[i].captures);
        
        if(real>0.5)
        {
          printf("%d", real);
          
          printf(" %s\n", trainerList[i].visits[i].regions[j]->monsters[k]->name);
        }
      }
    }
    if(i<(trainerCount-1))
    {
      printf("\n");
    }
  }
}

void release_memory(monster** monsterList, int monsterCount, region** regionList, int regionCount, trainer* trainerList, int trainerCount)
{
  for(int i = 0; i<monsterCount; i++)//clear the monster struct
  {
    free(monsterList[i]->name);
    free(monsterList[i]->element);
    free(monsterList[i]);
  }
  free(monsterList);

  for(int i = 0; i< regionCount; i++)//clear the region struct
  {
    free(regionList[i]->name);
    free(regionList[i]->monsters);
    free(regionList[i]);
  }
  free(regionList);

  for(int i = 0; i<trainerCount; i++)//clear the trainer struct
  {
    free(trainerList[i].name);
    free(trainerList[i].visits[i].regions);
    free(trainerList[i].visits);
  }
  free(trainerList);
}

void writeOut(FILE* outFile,monster** monsterList, int monsterCount, region** regionList, int    regionCount, trainer* trainerList, int trainerCount)
{
    
    float nm,an;
    int real;
    
    for(int i = 0; i<trainerCount; i++)
    {
        fprintf(outFile,"%s\n", trainerList[i].name);
        for(int j = 0 ; j<trainerList[i].visits[i].nregions; j++)
        {
          fprintf(outFile,"%s\n",trainerList[i].visits[i].regions[j]->name);
          for(int k = 0; k<trainerList[i].visits[i].regions[j]->nmonsters; k++)
          {
            nm = trainerList[i].visits[i].regions[j]->monsters[k]->population;
            an = trainerList[i].visits[i].regions[j]->total_population;
    
            real = round((nm/an)*trainerList[i].visits[i].captures);
            
            if(real>0.5)
            {
              fprintf(outFile,"%d",real);
              fprintf(outFile," %s\n", trainerList[i].visits[i].regions[j]->monsters[k]->name);
            }
          }
        }
        if(i<(trainerCount-1))
        {
          fprintf(outFile,"\n");
        }
    }
}

int main()
{
    atexit(report_mem_leak);
    FILE *inFile, *outFile;
    int mcount, rcount,tcount; //monster count
    monster **m;
    region **r;
    trainer *t;

    
    inFile = fopen("in.txt", "r");
    outFile = fopen("out.txt", "w");

    m = readMonsters(inFile, &mcount);
    
    r = readRegions(inFile,&rcount,m,mcount);

    t = readTrainers(inFile,&tcount,r,rcount);
    
    process_inputs(m,mcount,r,rcount,t,tcount);
    
    writeOut(outFile,m, mcount, r, rcount, t, tcount);
    
    release_memory(m, mcount, r, rcount, t, tcount);

    fclose(inFile);
    fclose(outFile);
}