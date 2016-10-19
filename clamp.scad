include <settings.scad>
include <utils.scad>

module clamp_screw() {
    translate([-ClampThickness/2, ClampDiameter/2, 0]) {
    translate([-40, ClampLength-ClampZone/2, ClampThickness/2]) rotate([90, 0, 90]) medium_screw();   
    }
}

module clamp() {
    difference(){
        cylinder(ClampThickness, ClampDiameter/2+ClampThickness/2, ClampDiameter/2+ClampThickness/2, $fn=fn);
        cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=fn);
        translate([0, -ClampDiameter/2, 0]) cube([ClampDiameter, ClampDiameter/1.2, ClampThickness]);
        translate([0,-100, ClampThickness/2]) rotate([-90, 0, 0]) big_screw();
     } ;
     difference() {
         translate([-ClampThickness/2, ClampDiameter/2, 0]) {
            difference(){
                cube([ClampThickness, ClampLength, ClampThickness]);
                translate([0, ClampLength-ClampZone, 0])cube([ClampThickness/3, ClampLength, ClampThickness]);
                translate([ClampThickness - ClampThickness/3, ClampLength-ClampZone, 0]) cube([ClampThickness/3, ClampLength, ClampThickness]);
             // clamp_screw();
            }          
        }
        clamp_screw();
    }
	 
};
    
clamp();