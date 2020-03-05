/*

KRAMER ELWELL, RODNEY DUPLESSIS, RAPHAEL RADNA
201B FINAL PROJECT
HEATWAVES - ALLOSPHERE MARINE RESEARCH SIMULATIN 
Winter 2020

*/

#include "al/app/al_App.hpp"
#include "al/system/al_Time.hpp"
#include "al/graphics/al_Image.hpp"
#include "al/io/al_CSVReader.hpp"
#include "al/math/al_Interpolation.hpp"
using namespace al;

#include <fstream>
#include <vector>
using namespace std;


// GLOBALS  
#define DUR 1200  
#define TIME_MOD 5 



typedef struct {        // Struct for a single Temperature datapoint
  double site, date, temp;
} temperatures;


typedef struct {     // Struct for a single biodiversity datapoint
  double date, site, transect, quad, size, count, comName, taxPhylum, mobility, growth;        
} biodiversities;


struct Species {
  vector<biodiversities> data;
  // vector<float> vdate, vsite, vtransect, vquad, vcount; 

/*
  float cMax, cMin, cAve;
  float taxPhylum;

  float x = cos((data[i].site / 11) * M_2PI) * data[i].transect;
  float y = quad;
  float z = sin((data[i].site / 11) * M_2PI) * data[i].transect;
  Vec3f origin = (x, y, z);

  void init() {
    for (int i = 0; i < data.size(); i++) {
    vdate.push_back(data[i].date);
    vsite.push_back(data[i].site);
    vtransect.push_back(data[i].transect);
    vquad.push_back(data[i].quad);
    vcount.push_back(data[i].count);
    }

    cAve = 0;
    for (auto c : vcount) {
      cAve += c;
    }
    cAve /= vcount.size();

    cMax = *max_element(vcount.begin(), vcount.end());
    cMin = *min_element(vcount.begin(), vcount.end());

    taxPhylum = data[0].taxPhylum;
    origin = (0, 0, 0);
  };

  void printCountAve() { std::cout << "Average: " << cAve << std::endl; };
  void printCountMax() { std::cout << "Maximum: " << cMax << std::endl; };
  void printCountMin() { std::cout << "Minimum: " << cMin << std::endl; };

  */
};


struct Heat {
  vector<temperatures> data;

  float prevVal = -1;
  float currentTemp, ave, max, min;
  vector<double> sites;
  vector<double> dates;
  vector<double> temps;

  // HeatSite site[3];

  void init () {
    float ave = 0;
    for (auto temp : temps) {
      ave += temp;
    }
    ave /= temps.size();

    max = *max_element(temps.begin(), temps.end());
    min = *min_element(temps.begin(), temps.end());
  };
    

  void printAve() { std::cout << "Average: " << ave << std::endl; }
  void printMax() { std::cout << "Maximum: " << max << std::endl; }
  void printMin() { std::cout << "Minimum: " << min << std::endl; }
  

  float update (float time, vector<float> v) {
    float x = v[0];
    float y = v[1];

    float eTime = ((int(time)*1000) % 200) / 200;

    if (v[int(time * TIME_MOD) % v.size()] != prevVal) {
      x = v[int(time * TIME_MOD)];
      y = v[int(time * TIME_MOD) + 1];
    }
    while (currentTemp != y) { currentTemp = (y - x) * eTime + x; }
    if (currentTemp == y) { prevVal = x; }
    std::cout << "Current temp is: " << currentTemp << std::endl;
  };


};




 









struct MyApp : App {
  Heat heat;
  Species species[58];
  Clock c;


  void onCreate() override {
    CSVReader temperatureData;
    // temperatureData.addType(CSVReader::STRING);             // Site
    // temperatureData.addType(CSVReader::STRING);             // Location
    // temperatureData.addType(CSVReader::STRING);             // Date
    // temperatureData.addType(CSVReader::STRING);             // Column Position
    // temperatureData.addType(CSVReader::REAL);               // Temperature (c)
    // temperatureData.addType(CSVReader::STRING);             // category
    temperatureData.addType(CSVReader::REAL);               // Site
    temperatureData.addType(CSVReader::REAL);               // Date
    temperatureData.addType(CSVReader::REAL);               // Temperature (c)
    temperatureData.readFile("../data/temperature3.csv");

    std::vector<double> temp = temperatureData.getColumn(2);
    for (auto t : temp) {
      heat.temps.push_back(t);
    }

    // std::vector<temperatures> tRows = temperatureData.copyToStruct<temperatures>();
    // for (auto t : tRows) {
    //   heat[int(t.site)].data.push_back(t);
    // };

    // std::vector<temperatures> temps = temperatureData.copyToStruct<temperatures>();
    // for (auto temp : temps) {
    //   heat.site[int(temp.site)].temps.push_back(temp); 
    // }


    CSVReader bioDiversityData;
    bioDiversityData.addType(CSVReader::REAL);              // Date                 | yymmdd Sequential
    bioDiversityData.addType(CSVReader::REAL);              // Site                 | 0 - 10
    bioDiversityData.addType(CSVReader::REAL);              // Transect             | 1 - 8
    bioDiversityData.addType(CSVReader::REAL);              // Quad                 | 0 - 6
    bioDiversityData.addType(CSVReader::REAL);              // Size                 | float > 0
    bioDiversityData.addType(CSVReader::REAL);              // Count                | float > 0 
    bioDiversityData.addType(CSVReader::REAL);              // Common Name          | 0 - 57
    bioDiversityData.addType(CSVReader::REAL);              // Taxonomy | Phylum    | 0 - 7
    bioDiversityData.addType(CSVReader::REAL);              // Mobility             | 0 or 1
    bioDiversityData.addType(CSVReader::REAL);              // Growth_Morph         | 0 or 1
    bioDiversityData.readFile("../data/biodiverse.csv");

    std::vector<biodiversities> bRows = bioDiversityData.copyToStruct<biodiversities>();
    for (auto b : bRows) {
      species[int(b.comName)].data.push_back(b);
    };

    // for (auto s : species) {
    //   cout  << s.data[0].comName << " : " << s.data[0].date << " | " << s.data[0].site 
    //         << " | " << s.data[0].transect  << " | " << s.data[0].quad      << " | " << s.data[0].size
    //         << " | " << s.data[0].count     << " | " << s.data[0].taxPhylum << " | " << s.data[0].taxPhylum
    //         << " | " << s.data[0].mobility  << " | " << s.data[0].growth    << " | " << s.data.size() << endl;
    // }



    // for (auto s : species){
    //   s.init();
    //   s.printCountMax();
    // }

    // heat.init();
    //heat.printMax();


  }


  float currentTemp = -1;
  void onAnimate(double dt) override {
    float currentTime = c.now();

    if (float(heat.temps[int(currentTime * TIME_MOD) % heat.temps.size()]) != currentTemp) {
      std::cout << int(currentTime * TIME_MOD) % heat.temps.size() << ": Current Temperature: " << heat.temps[int(currentTime * TIME_MOD)] << std::endl;
      currentTemp = float(heat.temps[int(currentTime * TIME_MOD)]);
    }

    c.update();
  }
  

  void onDraw(Graphics& g) override {
    vsync(true);
    g.clear(0.25); 
  }
};


int main() {
  MyApp app;
  app.start();
}





/*
--------------------------------------------------------------------------------------------------------;
SCRATCH NOTES ------------------------------------------------------------------------------------------;
--------------------------------------------------------------------------------------------------------;


vector<Species> phylumG1, phylumG2, phylumG3, phylumG4; 
phylumG1.size()... 

>> vector <Species> or Species species[58] probably
    >> Species has a...
        >> Vector <CSVdata>
            >> One line of CSV data

Need to use vsync for firm concept of time... is there a millis function? Solid/unwavering time elapsed function?

Total unique species = 58

unique kingdoms = 2
unique phylums = 8                                                  Best split - each of us sonifies 2 phylums
unique class = 13
unique order = 27
unique family = 42
unique genus = 53

Echinodermata + Porifera  =   112,200 Total count                   Primary gestural character
Cnidaria + Arthropoda     =   52,360 total count                    less active / acommpanimental role
Ochrophyta + Annelida     =   79,644 total count                    2 secondary gestural characters
Mollusca + Chordata       =   80,784 total count 
             

>>  heat temp sites = 3                                             heat wave time interpolated over 3d space
      2 depths
      2 radiuses
    Common sites with bioDiversity  = 3
    bioDiversity unique sites       = 8


>> Polar spatialization? 2PI / site# (0-10) = vector from origin to draw transects? 


>> timeline logic                                                   event in time where effects of position and size change
    year
    month
    date


>>  potential spatial logic - defines birth origin and a "home" space  
    ("I [are boid] want to stay close to this point unless a behavor [flee] tells me otherwise")  

      transect 1 - 8 >>> mapped to x/z axis >>> 8 points >> distance from origin >> normal vector * transect              
      quadrant 0 - 40 >>> mapped to y axis >>> 6 points >> elevation on the Y
      site 1 - 11 >>> mapped to x/z >>> 11 points divided into 2PI to get vector from origin [one site ever 32.72 deg]


>> neighborhood size - defines flock size
  count
  size
  area

need to do something like... 
float increment = pieceTime * 1000 / temps.size(); 
interpolate from temp[0] to temp[1] over increment

but... need to do that for EVERY parameter... parameter parent struct? so struct temperatures : parameters {} or similar... 
then a species struct that includes all of these parameter structs that pulls all the data based on an if?



count = 1;
Vec2f interop (clock c, float dur, int arraySize) {
  inc = dur / arraySize;
  if (c.now % inc == 0) {count++}

  Vec2f count+1, inc 



  Vec2f out = destination, duration;
  return out; 
}

onAnimate

parameters();

  void parameters (clock c) {
    heat = heatFUNCTION(heatdata)
    parameter1 = bioFUNCTION(p1data);
  }

  bioFUNCTION (vector v) {

  }



  Vec2f stepCalc (Clock c, int dur,) {


  }



float hTime = lerp(interop(clock, PIECE_TIME));
float spec1 = lerp(interop(clock, PIECE_TIME));


Time scale from 0 - 1 in heatTime
map sequential data in parameter to 0 - 1



    //int count = 0;
    // vector<biodiversities> rows = bioDiversityData.copyToStruct<biodiversities>();
    // for (auto row : rows) {
    //   std::cout << row.date << " " << row.comName << " " << row.site << std::endl;     
    // }

    std::vector<temperatures> cols = temperatureData.copyToStruct<temperatures>();

    // std::cout << cols[3].temp << std::endl;                                
    
    // std::cout << "the count =" << count << std::endl;
                                                     


    // for (auto name : temperatureData.getColumnNames()) {
    //   std::cout << name << " | ";
    // }
    // std::cout << std::endl << "---------------" << std::endl;


    // for (auto name : bioDiversityData.getColumnNames()) {
    //   std::cout << name << " | ";
    // }
    // std::cout << std::endl << "---------------" << std::endl;


    for (auto temp : temperatureData.getColumn(4)) {
      temps.push_back(temp);
    } 

    void valuePair (int *f, unsigned index = 0) {
      Vec2f v; 
      v.x = *(f + index);
      v.y = *(f + 1 + index); 
      cout << v << endl;
    }



  //   float currentTemp = temps[0];
  //   if (temps[int(currentTime * TIME_MOD) % temps.size()] != temp) {
  //     if (temps[int(currentTime * TIME_MOD) % temps.size()] == 0) {
  //       std::cout << "FIRST ELEMENT OF THE ARRAY" << std::endl;
  //     }
  //     std::cout << "Current Temperature: " << temps[int(currentTime * TIME_MOD)] << std::endl;
  //     temp = temps[int(currentTime * TIME_MOD)];
  //   }
  // }


/*
  >> What every species needs to know... 
          >> How many individual flocks?                                                            | some number of vector<Flocks> see 122
          >> Where is "home" for each flock                                                         | a Vec3f that has an attraction force
          >> How many agents in every flock over time...                                            | flock[1].pop_ or flock[1].push
          >                                                                                         | requires moving tally of count average over time BY FLOCK... oof
          >> When adding new flock members... WHERE are they added... (center of the flock)         | NOT home, flockCenter Vec3f
          >> flock behaviors...
          >> environmental influence... 








*/


