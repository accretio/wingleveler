include <settings.scad>
include <utils.scad>

include <nutsnbolts/cyl_head_bolt.scad>


module attachment() {
    //translate([0,-100, 0]) rotate([-90, 0, 0]) big_screw();
    translate([-ClampThickness/2, -52, 0]) {
        rotate([0, 90, 0]) {
            difference(){
                cylinder(ClampThickness, ClampDiameter/2+ClampThickness/2, ClampDiameter/2+ClampThickness/2, $fn=fn);
                cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=fn);
                translate([0, -ClampDiameter/2, 0]) cube([ClampDiameter, ClampDiameter / 1.3, ClampThickness]);
                translate([0, -50, ClampThickness/2]) rotate([-90, 0, 0]) big_screw();
            } ;
        }   
    }; 
    difference() {
        cube([ClampThickness, ClampThickness * 3, ClampThickness * 2], center=true);
    //     cube([ClampThickness, ClampThickness, ClampThickness]);
         translate([0, 10, 0]){
             cube([2*ClampThickness, ClampThickness+Tolerance, ClampThickness+Tolerance], center=true);
         }
    }
   
}

attachment();