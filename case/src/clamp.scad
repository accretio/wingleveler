include <settings.scad>
include <utils.scad>

include <nutsnbolts/cyl_head_bolt.scad>

module clamp_screw() {
    rotate([0, 90, 0]) {
  translate([-ClampThickness/2, ClampLength - ClampZone/2, -5]) {
            translate([0, 0, 22]) {
               hole_through(name="M3", l=20, cl=0.1, h=10, hcl=0.4); 
            }
            translate([0, 0, -2]) {
                nutcatch_parallel("M3", l=5);
            }
            }
        }
}



module arm() {
     difference() {
         translate([-ClampThickness/2, C, 0]) {
            difference(){
                cube([ClampThickness, ClampLength, ClampThickness]);
                translate([0, ClampLength-ClampZone, 0])cube([ClampThickness/3, ClampLength, ClampThickness]);
                translate([ClampThickness - ClampThickness/3, ClampLength-ClampZone, 0]) cube([ClampThickness/3, ClampLength, ClampThickness]);
             
            }          
        }
       clamp_screw();
    }
}


module clamp() {
   //     translate([0,-40, ClampThickness/2]) rotate([-90, 0, 0]) big_screw();
    
   /* difference(){
        cylinder(ClampThickness, ClampDiameter/2+ClampThickness/2, ClampDiameter/2+ClampThickness/2, $fn=fn);
        cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=fn);
        translate([0, -ClampDiameter/2, 0]) cube([ClampDiameter, ClampDiameter/1.2, ClampThickness]);
        translate([0,-100, ClampThickness/2]) rotate([-90, 0, 0]) big_screw();
     } ;
     
     */
     
     // this is the main arm
     translate([-180/2, -ClampThickness/2, 0]) {
         cube([180, ClampThickness, ClampThickness]);
     }
     arm(); 
	 
};

//attachment();
clamp();
