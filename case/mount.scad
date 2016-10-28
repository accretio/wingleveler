// All sizes are in mm 
// Let's start by describing the plane

use <parametric_involute_gear_v5.0.scad>
use <spur_generator.scad>

YokeDiameter=50;
YokeLength=0;

PanelHeight=300;
PanelWidth=400;
PanelThickness=20;

YokePosition=0;
HornSpacing=120; 
HornHeight=120; 
HornDiameter=40;

module yoke() {
    union() {
        translate([0, YokePosition, 0]) {
            rotate([270, 0, 0]) {
                cylinder(YokeLength, YokeDiameter/2, YokeDiameter/2); 
            }
        } ;
        translate([-(HornSpacing + 2 * HornDiameter)/2, YokePosition + YokeLength, -YokeDiameter/2]) {
            cube(size=[HornSpacing + 2 * HornDiameter,
             HornDiameter, 
             YokeDiameter]); 
        } ;
        translate([-HornSpacing/2 - HornDiameter, YokePosition + YokeLength + HornDiameter/2, -YokeDiameter/2]) {
            cylinder(HornHeight, HornDiameter/2, HornDiameter/2);
        } ;
        translate([HornSpacing/2 + HornDiameter, YokePosition + YokeLength + HornDiameter/2, -YokeDiameter/2]) {
            cylinder(HornHeight, HornDiameter/2, HornDiameter/2);
        }
    }
}

module panel() {
    translate([-PanelWidth/2, -PanelThickness, -PanelHeight/3]) {
        cube(size=[PanelWidth, PanelThickness, PanelHeight]); 
    }
}

module airplane() {
    
    yoke();
   // panel();
}


// Now we describe the wing leveler.

ClampHeightBack=50;
ClampHeightFront=80;
ClampWidth=200;
ClampThickness=30;


module clamp() {
  cube(size=[ClampWidth, ClampThickness, ClampHeightBack]); 
  cube(size=[ClampWidth, 2 * ClampThickness + 2* PanelThickness, ClampThickness]); 
  translate([0, 2 * ClampThickness + PanelThickness, 0]){
      cube(size=[ClampWidth, ClampThickness, ClampHeightFront]); 
  }
}



ScrewLength=100;
ScrewRadius=10;
ScrewEdge=1;

include <polyScrewThread_r1.scad>; 

module screw() {
     translate([0, 2 * ClampThickness + 3 * PanelThickness, -PanelHeight/3 + 30 ]){
         rotate([90, 0, 0]){
            hex_screw(30,4,40, 2 * ClampThickness + 3 * PanelThickness,1,1,60,16,0,0);
        }
    }
}

RingDiameter=HornSpacing + 2*HornDiameter;
RingThickness=20;

// this is the stepper motor (https://github.com/jcrocholl/kossel)

// NEMA17 stepper motors.
motor_shaft_diameter = 5;
motor_shaft_radius = motor_shaft_diameter/2 + extra_radius;

// Placement for the NEMA17 stepper motors.
motor_offset = 44;
motor_length = 47;

module nema17() {
  // NEMA 17 stepper motor.
  difference() {
    union() {
      translate([0, 0, -motor_length/2]) intersection() {
        cube([42.2, 42.2, motor_length], center=true);
        cylinder(r=25.1, h=motor_length+1, center=true, $fn=60);
      }
      cylinder(r=11, h=4, center=true, $fn=32);
      cylinder(r=2.5, h=48, center=true, $fn=24);
    }
    for (a = [0:90:359]) {
      rotate([0, 0, a]) translate([15.5, 15.5, 0])
        cylinder(r=m3_radius, h=10, center=true, $fn=12);
    }
  }
}


Interestice=10;
PadThickness=10;
PadDiameter=40;
module ring() {
    translate([0, 0, 0]) { 
        rotate([90, 0, 0]) {
            difference() {
                cylinder(RingThickness, RingDiameter, RingDiameter);
                cylinder(RingThickness, YokeDiameter/2 + Interestice, YokeDiameter/2 + Interestice);
            }
        }
    }
    translate([-YokeDiameter/2 - PadDiameter/4, -RingThickness/2, -PadThickness/2]) {
        rotate([0, 0, 90]) {
            cylinder(PadThickness, PadDiameter/2, PadDiameter/2);
        }
    }   
    translate([YokeDiameter/2 + PadDiameter/4, -RingThickness/2, -PadThickness/2]) {
        rotate([0, 0, 90]) {
            cylinder(PadThickness, PadDiameter/2, PadDiameter/2);
        }
    }   
     translate([-PadThickness/2, -RingThickness/2, -PadDiameter]) {
        rotate([0, 90, 0]) {
            cylinder(PadThickness, PadDiameter/2, PadDiameter/2);
        }
    }  
    translate([-PadThickness/2, -RingThickness/2, PadDiameter]) {
        rotate([0, 90, 0]) {
            cylinder(PadThickness, PadDiameter/2, PadDiameter/2);
        }
    }  
   
}
BoxWidth=100;
BoxDepth=60;
BoxHeight=60;
module ap() {
    /* difference() {
        translate([-ClampWidth/2, - PanelThickness - ClampThickness, -PanelHeight/3 - ClampThickness]) {
            clamp();
        }
       // screw();
    }; */
  // screw();
    
    // now let's talk about how we articulate the stepper motors
    
    translate([0, PanelThickness, 0]) ring();
    translate([200, 0, 60]) rotate([0, 00, 90]) nema17();
    translate([PadDiameter/2, 10, -RingDiameter + BoxHeight - 20]) rotate([0, -90, 0]) nema17();
    translate([-BoxWidth/2, 0, -RingDiameter - BoxHeight/2]) {
        cube([BoxWidth, BoxDepth, BoxHeight]);
    }
}
// Let's display some stuff



// ap();

BoxWidth=200;
BoxDepth=100;
BoxHeight=160;

RingR1=180;
GrooveWidth=10;
RingR2 = RingR1 - 3 * GrooveWidth;

module groove(R) {
    difference() {
                    rotate([90, 0, 0]) {
                        cylinder(RingThickness/2, R, R);
                    } 
                    rotate([90, 0, 0]) {
                        cylinder(RingThickness/2, R - GrooveWidth, R - GrooveWidth);
                    }
                }
    
}


RingNumberOfTeeth=100;
DriverNumberOfTeeth=6;
p = fit_spur_gears(RingNumberOfTeeth, DriverNumberOfTeeth, 230);

module teeth() {
	
	// Simple Test:
	

gear (circular_pitch=p,
		gear_thickness = 12,
		rim_thickness = 15,
		hub_thickness = 17,
		circles=0,
		number_of_teeth = DriverNumberOfTeeth,
		rim_width = 2);

}


module ap2() {
    
     translate([0, YokePosition + YokeLength + HornDiameter/2 + 2*RingThickness, 0]) { 
        difference() {
            difference() {
                difference() {
                    rotate([90, 0, 0]) {
                        gear (circular_pitch=p,
		gear_thickness = RingThickness,
		rim_thickness = RingThickness,
		hub_thickness = RingThickness,
	    number_of_teeth = RingNumberOfTeeth,
		circles=0);
                        cylinder(RingThickness, RingDiameter, RingDiameter);
                    }
                    // this is the spot for the roller ring
                    groove(RingR1);
                };
                translate([0, -RingThickness/2, 0]) groove(RingR2);
            }   
            translate([-2 * RingDiameter, - 2 * RingDiameter, HornHeight/2])
                cube([4*RingDiameter, 4*RingDiameter, RingDiameter]);
        }
        /*
        translate([-BoxWidth/2, -BoxDepth/4 - RingThickness/2, -240]) {
          difference(){
              cube([BoxWidth, BoxDepth, BoxHeight]);
              translate([0, RingThickness - BoxDepth/4 + RingThickness , 15]){ 
                    cube([BoxWidth, RingThickness, BoxHeight]);
              }
              }
          }; 
           translate([-BoxWidth/2, 10, -205]) rotate([90, 0, 0]) { 
               nema17();
               translate([0, 0, 10]) {
                   gear (circular_pitch=p,
                        gear_thickness = 12,
		rim_thickness = 15,
		hub_thickness = 17,
		circles=0,
		number_of_teeth = DriverNumberOfTeeth,
		rim_width = 2);               
           }
       } */
    }
    
   
   
    
    
}

HolderHeight=40;
HolderThickness=10;
module ap3() {
    translate([50, -10, 20]) {
        cube([30, HolderThickness, HolderHeight]);
    
        }; 
    translate([50, -10, 20]) {
        cube([30, HolderThickness, HolderHeight]);
    
        }; 
    translate([-RingDiameter, 100, 0]) {
        cube([RingDiameter * 2, HolderThickness, HolderHeight]);
    
        }; 
    difference() {
            difference() {
                difference() {
                    rotate([-90, 0, 0]) {
                        gear (circular_pitch=p,
                        gear_thickness = RingThickness,
                        rim_thickness = RingThickness,
                        hub_thickness = RingThickness,
                        number_of_teeth = RingNumberOfTeeth,
                        circles=0);
                        cylinder(RingThickness, RingDiameter, RingDiameter);
                    }
                    // this is the spot for the roller ring
                    groove(RingR1);
                };
                translate([0, -RingThickness/2, 0]) groove(RingR2);
            }   
              }
} ;


ap3();

translate([0, -HornDiameter, 0]){
yoke();
}