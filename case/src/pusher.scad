include <settings.scad>
include <utils.scad>

include <nutsnbolts/cyl_head_bolt.scad>
use <clamp.scad>
    
 difference(){
       cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=fn);
       translate([5, 10, 0]) {
            cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=fn);
       }
       translate([0,-70, ClampThickness/2]) rotate([-90, 0, 0]) big_screw();
 }
   
 