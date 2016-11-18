include <settings.scad>
use <rail.scad>
use <clamp.scad>
use <nema17.scad>
include <polyScrewThread_r1.scad>



/* Example 01.
 * Just a 100mm long threaded rod.
 *
 * screw_thread(15,   // Outer diameter of the thread
 *               4,   // Step, traveling length per turn, also, tooth height, whatever...
 *              55,   // Degrees for the shape of the tooth 
 *                       (XY plane = 0, Z = 90, btw, 0 and 90 will/should not work...)
 *             100,   // Length (Z) of the tread
 *            PI/2,   // Resolution, one face each "PI/2" mm of the perimeter, 
 *               0);  // Countersink style:
 *                         -2 - Not even flat ends
 *                         -1 - Bottom (countersink'd and top flat)
 *                          0 - None (top and bottom flat)
 *                          1 - Top (bottom flat)
 *                          2 - Both (countersink'd)
 */

PI=3.141592; 
RailLength=400;
RailDiameter=60; 
RodTol=0.2;
module rod() {
     difference(){
          translate([-ClampThickness, -ClampThickness/2, 0]) {
               cube([ClampThickness, RingThickness, RailLength ]);   
          }
          translate([-RailDiameter/2 - RodTol/2, 0, 0]) {
               screw_thread(RailDiameter+RodTol,10,55,RailLength,PI/4, 0);
          }
        
     }
}


GrooveDepth=5; 


module rails(Tol) {
     translate([RingThickness/2, 0, RailLength/2]) {
          rotate([0, 90, 0]) {
               translate([0, (ClampThickness + Tol)/2 - GrooveDepth/2 +10 , 0]) {
                    cube([RailLength, GrooveDepth + 20 , GrooveDepth - Tol], center= true);
                    //  cube([RailLength, 20 , 20], center= true);
               }
               translate([0, (ClampThickness + Tol)/2 - GrooveDepth/2 + 15 , 0]) {
            
                    cube([RailLength, 25 , 20], center= true);
               }
               translate([0, -(ClampThickness + Tol)/2 + GrooveDepth/2 - 15 , 0]) {
            
                    cube([RailLength, 25 , 20], center= true);
               }
               translate([0, -(ClampThickness + Tol)/2 + GrooveDepth/2 - 10 , 0]) {
                    cube([RailLength, GrooveDepth + 20, GrooveDepth-Tol], center= true);
               }
          }  
     }
}
module holder() {
     difference() {
          translate([RingThickness/2, 0, RailLength/2]) {
               rotate([0, 90, 0]) {              
                    cube([RailLength, ClampThickness, RingThickness], center= true);
         
               }
          }
          rails(0);
     }
}

RailClampLength=40;

module arm() {
     translate([RingThickness, 0, 0]) {
          difference() {
               translate([0, -ClampThickness/2, RailLength /2 - ClampThickness / 2]) {
                    cube([RailClampLength, ClampThickness, ClampThickness]);
               }
               translate([RailClampLength-ClampZone, -ClampThickness/2, RailLength /2 - ClampThickness / 2 + ClampThickness/3]) {
                    cube([ClampZone, ClampThickness, ClampThickness/3]);  
                    translate([0, -ClampThickness/2, 4]){
                         rotate([0, 90, 0]) {
                              clamp_screw(); 
                         }
                    }   
               }
          }
     }
}


//rod();
holder();
//arm();


// now the driver
DriverLength=40;
tol = 0.2;

module driver_gear(){
     translate([-RailDiameter/2, 0, 0]) {
          translate([3 - tol, 0, DriverLength/2]) {
               cube([2, 5, DriverLength], center=true);
          };
          difference(){
               screw_thread(RailDiameter,10,55,DriverLength,PI/4, 0);
               cylinder(DriverLength, 5/2 + tol , 5/2 + tol, $fn=fn);
          }
     }
}

//rails();
//driver_gear();  

PlateDepth=10; 
PlateRailDepth=40;
PlateWidth=70;
PlateHeight=130;

module m3_screw_for_nema() {
     rotate([90, 0, 0]) {
          // screw("M3x10", thread="modeled"); // screw M20x100 with thread
          translate([0, 0, 10]) hole_through(name="M3", l=10, cl=0.1, h=10, hcl=0.4);
     }
}

module nema_screws() {
     translate([31, 4, 0]) {
          m3_screw_for_nema();
     } 
    
     translate([0, 4, 0]) {
          m3_screw_for_nema();
     }
    
     translate([0, 4, 31]) {
          m3_screw_for_nema(); 
     }
    
     translate([31, 4, 31]) {
          m3_screw_for_nema();
     } 
}


module plate() {
    
     difference(){
          translate([-PlateHeight+ClampThickness, -PlateWidth/2, -PlateDepth]) {
               cube([PlateHeight, PlateWidth, PlateDepth]);
            
          }
          translate([-20, -(ClampThickness + RodTol)/2, -PlateDepth]) {
               cube([ClampThickness + RodTol + 20, ClampThickness + RodTol, PlateDepth]);
          }
          translate([-31/2 -RailDiameter/2, -31/2, 0]) {
               rotate([-90, 0, 0]) {
                    nema_screws();
               }
          }
          translate([-RailDiameter/2, 0, -10]) {
               cylinder(10, 16, 16);
          }
         
     }

// the little rails
     difference() {
          intersection(){
               translate([0, -PlateWidth/2, -PlateDepth]) {
                    cube([PlateWidth, PlateWidth, DriverLength + PlateDepth + 5]);
               }
               translate([0, 0, -DriverLength - 10]) {
                    rails(RodTol);
               }
          }
          plate_screws();
     }
     // the box
     difference(){
          translate([-PlateHeight+42.5, 0, (DriverLength + 5) / 2]) {
               difference() {
                    cube([45, PlateWidth, DriverLength + 5], center=true);
                    cube([35, PlateWidth-20, DriverLength + 5], center=true);
               }
          };
          plate_screws();
     }

}

module plate_screws() {
     // the screws to connect the parts
     translate([10, PlateWidth/2 - 12, DriverLength + 19]) {
          hole_through(name="M3", l=16, cl=0.1, h=10, hcl=0.4);
          translate([0, 0, -18]) {
               nutcatch_sidecut("M3", l=100, clk=0.4, clh=0.4, clsl=0.4);
          }
     }

      // the screws to connect the parts
     translate([10, -PlateWidth/2 + 12, DriverLength + 19]) {
          hole_through(name="M3", l=16, cl=0.1, h=10, hcl=0.4);
          translate([0, 0, -18]) {
               nutcatch_sidecut("M3", l=100, clk=0.4, clh=0.4, clsl=0.4);
          }
     }

       // the screws to connect the parts
     translate([-PlateHeight + 30, -PlateWidth/2 + 5, DriverLength + 19]) {
          hole_through(name="M3", l=16, cl=0.1, h=10, hcl=0.4);
          rotate([0, 0, -90]) {
               translate([0, 0, -18]) {
                    nutcatch_sidecut("M3", l=100, clk=0.4, clh=0.4, clsl=0.4);
               }
               }
     }

        // the screws to connect the parts
     translate([-PlateHeight + 30, PlateWidth/2 - 5, DriverLength + 19]) {
          hole_through(name="M3", l=16, cl=0.1, h=10, hcl=0.4);
          rotate([0, 0, 90]) {
               translate([0, 0, -18]) {
                    nutcatch_sidecut("M3", l=100, clk=0.4, clh=0.4, clsl=0.4);
               }
               }
     }
}

LeftPartWidth=30;

module left_part() {
     difference() {
     translate([0, 0, PlateHeight/2 - PlateDepth]) {
          difference(){
               translate([-PlateHeight+ClampThickness, -PlateWidth/2, -PlateDepth]) {
                    cube([PlateHeight, PlateWidth, PlateDepth]);
                    
               }
               translate([-20, -(ClampThickness + RodTol)/2, -PlateDepth]) {
                    cube([ClampThickness + RodTol + 20, ClampThickness + RodTol, PlateDepth]);
               }
          }
          
          intersection(){
               translate([0, -PlateWidth/2, -PlateDepth]) {
                    cube([PlateWidth, PlateWidth, PlateDepth]);
               }
               translate([0, 0, -DriverLength - 10]) {
                    rails(RodTol);
               } 
          }
          translate([-PlateHeight/2, 0, LeftPartWidth/2 - 5]) {
               difference() {
                    cube([PlateHeight - 40, PlateWidth-20, LeftPartWidth], center=true);
                    translate([5, 0, 0]) {
                         cube([PlateHeight - 45, PlateWidth-30, LeftPartWidth-10], center=true);
                    }
               }
          }
     }
     plate_screws();
   }  
     
}

//plate();
//left_part();

/*
translate([-RailDiameter/2, 0, -10]) { 
  nema17();
 
  }


*/
