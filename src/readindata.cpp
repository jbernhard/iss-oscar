// ver 1.1
#include<iostream>
#include<sstream>
#include<string>
#include<fstream>
#include<cmath>
#include<iomanip>
#include<stdlib.h>

#include "main.h"
#include "readindata.h"
#include "arsenal.h"
#include "Table.h"
using namespace std;

int get_filelength(string filepath)
{
  ifstream oscar(filepath.c_str());

  string line;
  int length = 0;

  // skip the header
  while ( line != "END_OF_HEADER" )
    getline(oscar, line);

  // count number of lines after header
  while ( getline(oscar, line) )
    ++length;

  oscar.close();

  return length;
}

void strip(string& s) {
   size_t p = s.find_first_not_of(" ");
   s.erase(0, p);

   p = s.find_last_not_of(" ");
   if (string::npos != p)
      s.erase(p+1);
}


// OSCAR2008H.dat
// tau x y e p T R_qgp vx vy dsig_t dsig_x dsig_y [pi00 pi01 pi02 pi11 pi12 pi22 pi33]

void read_decdat(string path, int length, FO_surf* surf_ptr)
{
  cout<<" -- Read in information on freeze out surface...";
  double dummy;
  double zero = 0.0;
  string line;
  string key;
  string value;
  bool viscous;
  ostringstream decdat_stream;
  decdat_stream << path << "/OSCAR2008H.dat";
  ifstream decdat(decdat_stream.str().c_str());

  // parse the header
  while ( line != "END_OF_HEADER" ) {
    getline(decdat, line);

    istringstream linestr(line);

    // split each line into key:value pairs
    getline(linestr, key, ':');
    getline(linestr, value);
    strip(value);

    // check geometry
    if (key == "GEOM" && value != "scaling2d")
      throw " ERROR: iSS can only handle scaling2d geometry";

    // check grid
    if (key == "GRID" && value != "Euler")
      throw " ERROR: iSS can only handle Eulerian grid";

    // check viscosity
    if (key == "VISCOSITY") {
      if (value == "none") {
        viscous = false;
      } else if (value == "shear viscosity only") {
        viscous = true;
      } else {
        throw " ERROR: iSS can only handle shear viscosity";
      }
    }
  }

  // read data from remaining lines
  for(int i=0; i<length; i++)
  {
     // read time from first column
     decdat >> surf_ptr[i].tau;

     // next two columns are x,y -- ignore them in this function
     decdat >> dummy >> dummy;

     // energy density, pressure, temp
     decdat >> surf_ptr[i].Edec;
     decdat >> surf_ptr[i].Pdec;
     decdat >> surf_ptr[i].Tdec;

     // ignore R_qgp
     decdat >> dummy;

     // flow velocities
     decdat >> surf_ptr[i].vx;
     decdat >> surf_ptr[i].vy;

     // normal vector
     decdat >> surf_ptr[i].da0;
     decdat >> surf_ptr[i].da1;
     decdat >> surf_ptr[i].da2;

     if (viscous) {
       // read shear tensor components
       decdat >> surf_ptr[i].pi00;
       decdat >> surf_ptr[i].pi01;
       decdat >> surf_ptr[i].pi02;
       decdat >> surf_ptr[i].pi11;
       decdat >> surf_ptr[i].pi12;
       decdat >> surf_ptr[i].pi22;
       decdat >> surf_ptr[i].pi33;
     } else {
       // no viscosity
       // set all shear tensor components to zero
       surf_ptr[i].pi00 = zero;
       surf_ptr[i].pi01 = zero;
       surf_ptr[i].pi02 = zero;
       surf_ptr[i].pi11 = zero;
       surf_ptr[i].pi12 = zero;
       surf_ptr[i].pi22 = zero;
       surf_ptr[i].pi33 = zero;
     }

     // set baryon density, chemical potentials to zero
     surf_ptr[i].Bn = zero;
     surf_ptr[i].muB = zero;
     surf_ptr[i].muS = zero;
  }
  decdat.close();
  cout<<"done"<<endl;
  return;
}

void read_surfdat(string path, int length, FO_surf* surf_ptr)
{
  cout<<" -- Read spatial positions of freeze out surface...";
  ostringstream surfdat_stream;
  double dummy;
  string line;
  char rest_dummy[512];
  surfdat_stream << path << "/OSCAR2008H.dat";
  ifstream surfdat(surfdat_stream.str().c_str());

  // skip the header
  while ( line != "END_OF_HEADER" )
    getline(surfdat, line);

  // read data from remaining lines
  for(int i=0; i<length; i++)
  {
     // ignore first column [tau]
     surfdat >> dummy;

     // read positions
     surfdat >> surf_ptr[i].xpt;
     surfdat >> surf_ptr[i].ypt;

     // ignore rest of line
     surfdat.getline(rest_dummy, 512);
  }
  surfdat.close();
  cout<<"done"<<endl;
  return;
}

void read_decdat_mu(string path, int FO_length, int N_stable, double** particle_mu)
{
  cout<<" -- Read chemical potential for stable particles...";
  ostringstream decdat_mu_stream;
  double dummy;
  decdat_mu_stream << path << "/decdat_mu.dat";
  ifstream decdat_mu(decdat_mu_stream.str().c_str());

  //For backward compatibility: decdat_mu.dat can be one line or FO_length lines
  for(int j=0; j<FO_length; j++)
  {
    decdat_mu >> dummy;  //not used in the code plz ignore it

    if(decdat_mu.eof())
    {
      for(int k=j; k<FO_length; k++)
        for(int i=0; i<N_stable; i++)
           particle_mu[i][k]=particle_mu[i][j-1];
      break;
    }

    for(int i=0; i<N_stable; i++)
    {
       decdat_mu >> particle_mu[i][j];
    }
  }

  cout<<"done" << endl;
  return;
}

int read_resonance(particle_info* particle)
{
   int Nparticle=0;
   cout << " -- Read in particle resonance decay table...";
   ifstream resofile("EOS/pdg.dat");
   int local_i = 0;
   int dummy_int;
   while (!resofile.eof())
   {
      resofile >> particle[local_i].monval;
      resofile >> particle[local_i].name;
      resofile >> particle[local_i].mass;
      resofile >> particle[local_i].width;
      resofile >> particle[local_i].gspin;	      //spin degeneracy
      resofile >> particle[local_i].baryon;
      resofile >> particle[local_i].strange;
      resofile >> particle[local_i].charm;
      resofile >> particle[local_i].bottom;
      resofile >> particle[local_i].gisospin;     //isospin degeneracy
      resofile >> particle[local_i].charge;
      resofile >> particle[local_i].decays;
      for (int j = 0; j < particle[local_i].decays; j++)
      {
         resofile >> dummy_int;
         resofile >> particle[local_i].decays_Npart[j];
         resofile >> particle[local_i].decays_branchratio[j];
         resofile >> particle[local_i].decays_part[j][0];
         resofile >> particle[local_i].decays_part[j][1];
         resofile >> particle[local_i].decays_part[j][2];
         resofile >> particle[local_i].decays_part[j][3];
         resofile >> particle[local_i].decays_part[j][4];
      }

      //decide whether particle is stable under strong interactions
      if(particle[local_i].decays_Npart[0] == 1)
         particle[local_i].stable = 1;
      else
         particle[local_i].stable = 0;

      //add anti-particle entry
      if(particle[local_i].baryon == 1)
      {
         local_i++;
         particle[local_i].monval = -particle[local_i-1].monval;
         ostringstream antiname;
         antiname << "Anti-" << particle[local_i-1].name;
         particle[local_i].name = antiname.str();
         particle[local_i].mass = particle[local_i-1].mass;
         particle[local_i].width = particle[local_i-1].width;
         particle[local_i].gspin = particle[local_i-1].gspin;
         particle[local_i].baryon = -particle[local_i-1].baryon;
         particle[local_i].strange = -particle[local_i-1].strange;
         particle[local_i].charm = -particle[local_i-1].charm;
         particle[local_i].bottom = -particle[local_i-1].bottom;
         particle[local_i].gisospin = particle[local_i-1].gisospin;
         particle[local_i].charge = -particle[local_i-1].charge;
         particle[local_i].decays = particle[local_i-1].decays;
         particle[local_i].stable = particle[local_i-1].stable;
         for (int j = 0; j < particle[local_i].decays; j++)
         {
            particle[local_i].decays_Npart[j]=particle[local_i-1].decays_Npart[j];
            particle[local_i].decays_branchratio[j]=particle[local_i-1].decays_branchratio[j];
            for (int k=0; k< Maxdecaypart; k++)
            {
               int idx = 0;  
               for(int ii=0; ii < local_i; ii++) // find the index for decay particle
               {
                  if(particle[local_i-1].decays_part[j][k] == particle[ii].monval)
                  {
                     idx = ii;
                     break;
                  }
               }
               if(idx == local_i-1 && particle[local_i-1].stable == 0)  // check
               {
                  cout << "Error: can not find decay particle index for anti-baryon!" << endl;
                  cout << "particle monval : " << particle[local_i-1].decays_part[j][k] << endl;
                  exit(1);
               }
               if(particle[idx].baryon == 0 && particle[idx].charge == 0 && particle[idx].strange == 0)
                  particle[local_i].decays_part[j][k]= particle[local_i-1].decays_part[j][k];
               else
                  particle[local_i].decays_part[j][k]= -particle[local_i-1].decays_part[j][k];
            }
         }
       }
       local_i++;	// Add one to the counting variable "i" for the meson/baryon
   }
   resofile.close();
   Nparticle=local_i-1; //take account the final fake one
   for(int i=0; i < Nparticle; i++)
   {
      if(particle[i].baryon==0)
         particle[i].sign=-1;
      else
         particle[i].sign=1;
   }
   return(Nparticle);
}

void calculate_particle_mu(int Nparticle, FO_surf* FOsurf_ptr, int FO_length, particle_info* particle, double** particle_mu)
{
   int IEOS = 7;
   int Nstable_particle;
   int Idummy;
   char cdummy[256];
   if(IEOS!=7)
   {
      cout << "Error! IEOS = "<<IEOS << " is not support for PCE!"<<endl;
      exit(1);
   }
   else
   {
      cout << " -- Read particle table and calculating chemical potential for particles..." << endl;
      ifstream particletable("EOS/EOS_particletable.dat");
      particletable >> Nstable_particle;
      double *stable_particle_monval = new double [Nstable_particle];
      for(int i=0; i<Nstable_particle; i++)
      {
          particletable >> Idummy >> stable_particle_monval[i];
          particletable.getline(cdummy, 256);
      }
      particletable.close();

      for(int i=0; i<Nstable_particle; i++)
         for(int j=0; j<Nparticle; j++)
            if(particle[j].monval == stable_particle_monval[i])
            {
               particle[j].stable = 1;
               for(int k=0; k<FO_length; k++)
                   FOsurf_ptr[k].particle_mu[j] = particle_mu[i][k];
               break;
            }

      print_progressbar(-1);
      for(int i=0; i < Nparticle ; i++)
      {
         if(particle[i].stable==0)
         {
            for(int j=0; j < particle[i].decays; j++)
            {
               for(int k=0; k < abs(particle[i].decays_Npart[j]); k++)
               {
                  for(int l=0; l < Nparticle; l++)
                  {
                     if(particle[i].decays_part[j][k] == particle[l].monval)
                     {
                        for(int m=0; m<FO_length; m++)
                          FOsurf_ptr[m].particle_mu[i] += particle[i].decays_branchratio[j]*FOsurf_ptr[m].particle_mu[l];
                        break;
                     }
                     if(l==Nparticle-1)
                        cout<<"warning: can not find particle" <<  particle[i].name << endl;
                  }
               }
            }
         }
         print_progressbar((double)(i)/Nparticle);
      }
      print_progressbar(1);
   }
   return;
}

