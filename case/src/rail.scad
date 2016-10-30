/// THE RAIL 

include <../deps/nutsnbolts/cyl_head_bolt.scad>
include <settings.scad>
use <parametric_involute_gear_v5.0.scad>
use <spur_generator.scad>
use <clamp.scad>

RingThickness = 20; 
RingDiameter=140;

p = fit_spur_gears(RingNumberOfTeeth, DriverNumberOfTeeth, 180);

module groove(R, W) {
    difference() {
        cylinder(RingThickness/2, R, R, $fn = fn);
        cylinder(RingThickness/2, R - W, R - W, $fn = fn);
    }
}

module rail() {
   difference(){  
     gear (circular_pitch=p,
		gear_thickness = RingThickness,
		rim_thickness = RingThickness,
		hub_thickness = RingThickness,
	    number_of_teeth = RingNumberOfTeeth,
		circles=0);
     cylinder(RingThickness, RingDiameter, RingDiameter, $fn = fn);
     translate([-2*RingDiameter,0, 0]){
         cube([4*RingDiameter, 2*RingDiameter, RingThickness]);
     };
     groove(RingDiameter+22, GrooveWidth);
     translate([0, 0, RingThickness/2]) {
          groove(RingDiameter+12, GrooveWidth);
     }
  }
}


module arm() {
 
      difference() {
        translate([-ClampThickness/2, -RingDiameter, 0]) {
            cube([ClampThickness, RingDiameter, RingThickness]);
        };
       ghost_clamp();
        clamp_screw();
      }
}

module ghost_clamp() {
    translate([0, ClampDiameter /2 + ClampLength - ClampZone, 0]){
        rotate([0, 0, 180]) {
            union(){ 
                clamp_screw();
                clamp();
                
            }
        }
    }
}





module assembly() {
    union() {
        arm();
        rail();
    };
}



module half(){
    difference(){
        assembly();
        translate([ClampThickness/6, -RingDiameter*2, 0]){
            cube([RingDiameter*2, RingDiameter*2, RingThickness]);
        };
        translate([-ClampThickness, -RingDiameter / 3 * 2, RingThickness/2]){
            cube([ClampThickness*2, RingDiameter/3, RingThickness]);
        };   
    }
}

module left_rail(){
    difference(){
        half();
        holding_screw();
    }
}

module right_rail(){
    difference(){
        assembly();
        half();
        holding_screw();
    }
}

module holding_screw(){
         translate([0, -RingDiameter / 2, 0]) {
            translate([0, 0, RingThickness + 5]) {
               hole_through(name="M3", l=20, cl=0.1, h=10, hcl=0.4); 
            }
            translate([0, 0, 5]) {
                nutcatch_parallel("M3", l=5);
            }
            }
        
   
}
/// now we need to split the assembly & print it. 
//holding_screw();
//left_rail();
//right_rail();