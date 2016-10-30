include <../deps/nutsnbolts/cyl_head_bolt.scad>

include <settings.scad> 
use <nema17.scad>
use <rail.scad>
use <raspberry_pi_3.scad>


// the controller is moving on the rail 

BoxWidth= 80;
BoxHeight=80;
Padding=4;  // unused

BoxDepth=10;



module controller_back_plate() {
  
    
     
   intersection(){
    translate([0, Tolerance, RingDiameter + 75]) {
        rotate([90, 0, 0]) {
            groove(RingDiameter+22 - Tolerance/2, GrooveWidth-Tolerance);
        }
    }

    translate([-BoxWidth/2 + BoxThickness,  -BoxDepth - Tolerance, BoxThickness]) {
            cube([BoxWidth - 2*BoxThickness , RingThickness + Tolerance, BoxHeight - BoxThickness], center=false);
        }

}
    difference(){ 
        translate([0, BoxDepth/2, BoxHeight/2]) {
            cube([BoxWidth, BoxDepth, BoxHeight], center=true);
        }
        translate([0, 20, 31]) {
            rotate([90, 0, 00]) {
                cylinder(RingThickness*2, 16, 16, $fn = fn); 
            }
        }
        nema_screws();
        block_screws();
    } 
}

Tolerance = 2;
BoxThickness = 8; 

module controller_front_plate() {
    
   
   intersection(){
    translate([0, -(BoxDepth + 2*Tolerance), RingDiameter + 75]) {
        rotate([90, 0, 0]) {
            groove(RingDiameter+12, GrooveWidth-Tolerance);
        }
    }
     translate([-BoxWidth/2 + BoxThickness, - RingThickness - Tolerance, BoxThickness]) {
            cube([BoxWidth - 2*BoxThickness , RingThickness + Tolerance, BoxHeight - BoxThickness], center=false);
        }
}
    difference() {
        translate([-BoxWidth/2, - RingThickness - BoxDepth - Tolerance, 0]) {
            cube([BoxWidth, BoxDepth + RingThickness + Tolerance, BoxHeight], center=false);
        }
         translate([-BoxWidth/2 + 1.5*BoxThickness, - RingThickness - Tolerance, 1.5*BoxThickness]) {
            cube([BoxWidth - 3*BoxThickness , RingThickness + Tolerance, BoxHeight - BoxThickness], center=false);
        }
        translate([-BoxWidth/2 , - RingThickness - Tolerance, BoxHeight / 2 ]) {
            cube([BoxWidth, RingThickness + Tolerance, BoxHeight / 2 ], center=false);
        }
        block_screws();
    } 
     
     
}


module m3_screw_for_nema() {
     rotate([90, 0, 0]) {
       // screw("M3x10", thread="modeled"); // screw M20x100 with thread
        translate([0, 0, 10]) hole_through(name="M3", l=10, cl=0.1, h=10, hcl=0.4);
    }
}

module nema_screws() {
    translate([15, 4, 15]) {
       m3_screw_for_nema();
    } 
    
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
   
    
   controller_back_plate(); 
   //controller_front_plate(); 
  
}



module block_screw1() {
    rotate([90, 0, 0]) {
            //screw("M3x30", thread="modeled");
            translate([0, 0, 10]) hole_through(name="M3", l=35, cl=0.1, h=10, hcl=0.4);
            translate([0, 0, -25]) nutcatch_sidecut("M3", l=100, clk=0.1, clh=0.1, clsl=0.1);
        }
}

module block_screw2() {
    rotate([90, 0, 0]) {
            //screw("M3x30", thread="modeled");
            translate([0, 0, 10]) hole_through(name="M3", l=35, cl=0.1, h=10, hcl=0.4);
            rotate([0, 0, 180]) translate([0, 0, -25]) nutcatch_sidecut("M3", l=100, clk=0.1, clh=0.1, clsl=0.1);
        }
}

module block_screws() {
    translate([BoxWidth/2 - 0.75*BoxThickness, -RingThickness -Tolerance - BoxDepth + 10, 7]) {
       block_screw1(); 
    }
    
     translate([BoxWidth/2 - 0.75*BoxThickness, -RingThickness -Tolerance - BoxDepth + 10, 32]) {
         block_screw1(); 
    }
    
    translate([-BoxWidth/2 + 0.75*BoxThickness, -RingThickness -Tolerance - BoxDepth + 10, 32]) {
        block_screw2(); 
    }
    
     translate([-BoxWidth/2 + 0.75*BoxThickness, -RingThickness -Tolerance - BoxDepth + 10, 7]) {
        block_screw2(); 
    }
    
    
}


module pi_holder() {
     top();
}

module stepper_assembly() {
    translate([0, 0, -10]) {
            nema17();
    }
   /* gear (circular_pitch=p,
              gear_thickness = RingThickness,
		      rim_thickness = RingThickness,
		      hub_thickness = RingThickness,
		      circles=0,
		      number_of_teeth = DriverNumberOfTeeth,
		      rim_width = 0);  
    */
}

controller();
//stepper_assembly();

//controller();