include <../deps/nutsnbolts/cyl_head_bolt.scad>

include <settings.scad> 
include <nema17.scad>
include <rail.scad>
use <raspberry_pi_3.scad>

// the controller is moving on the rail 

BoxWidth= 80;
BoxHeight=80;
Padding=4;  // unused

BoxDepth=10;


module guide() {
    cylinder(RingThickness * 0.90, GrooveWidth * 0.45, GrooveWidth * 0.45, $fn = fn);
}


module controller_back_plate() {
    translate([0, 0, RingDiameter + 74]){
        rotate([90, 0, 0]) {
           //left_rail(); 
            translate([ 0, -RingDiameter - 22 + GrooveWidth /2 , -RingThickness /2]) {
                guide(); 
            }
        }
    }
    difference(){ 
        translate([0, BoxDepth/2, BoxHeight/2]) {
            cube([BoxWidth, BoxDepth, BoxHeight], center=true);
        }
        translate([0, 10, 31]) {
            rotate([90, 0, 00]) {
                cylinder(RingThickness, 16, 16, $fn = fn); 
            }
        }
        nema_screws();
    } 
}

Tolerance = 2;
BoxThickness = 5; 
module controller_front_plate() {
    
    translate([0, 0, RingDiameter + 74]){
        rotate([90, 0, 0]) {
            //left_rail(); 
            translate([ 0, -RingDiameter - 12 + GrooveWidth /2,  RingThickness/2 + 2]) {
                guide(); 
            }
        }
    } 
   
    difference() {
        translate([-BoxWidth/2, - RingThickness - BoxDepth - Tolerance, 0]) {
            cube([BoxWidth, BoxDepth + RingThickness + Tolerance, BoxHeight], center=false);
        }
         translate([-BoxWidth/2 + BoxThickness, - RingThickness - Tolerance, BoxThickness]) {
            cube([BoxWidth - 2*BoxThickness , RingThickness + Tolerance, BoxHeight - BoxThickness], center=false);
        }
        translate([-BoxWidth/2 , - RingThickness - Tolerance, BoxHeight / 2 ]) {
            cube([BoxWidth, RingThickness + Tolerance, BoxHeight / 2 ], center=false);
        }
    } 
     
      //bottom();
}


module m3_screw_for_nema() {
     rotate([90, 0, 0]) {
        screw("M3x10", thread="modeled"); // screw M20x100 with thread
        translate([0, 0, 10]) hole_through(name="M3", l=10, cl=0.1, h=10, hcl=0.4);
    }
}

module nema_screws() {
     translate([15, 4, 15]) {
       m3_screw_for_nema();
    }; 
    
    translate([-16, 4, 15]) {
        m3_screw_for_nema();
    }
    
    translate([-16, 4, 46]) {
       m3_screw_for_nema(); 
    }
    
    translate([15, 4, 46]) {
       m3_screw_for_nema();
    } 
}

module controller() { 
    //pi_holder();
   translate([0, 0, 31]) {
       rotate([90, 0, 0]){ 
            stepper_assembly();
        }
    } 
    
    controller_back_plate(); 
 //  controller_front_plate(); 
  
}

module pi_holder() {
     top();
}

module stepper_assembly() {
    translate([0, 0, -10]) {
            nema17();
    }
    gear (circular_pitch=p,
              gear_thickness = RingThickness,
		      rim_thickness = RingThickness,
		      hub_thickness = RingThickness,
		      circles=0,
		      number_of_teeth = DriverNumberOfTeeth,
		      rim_width = 0);  
}

//stepper_assembly();

controller();