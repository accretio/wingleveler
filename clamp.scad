ClampThickness=30;
ClampDiameter=100;

include <polyScrewThread_r1.scad>


module screw() {
       hex_screw(15,4,55,100,1.5,2,24,8,0,0);
 
}
module clamp() {
    difference(){
        cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=100);
        cylinder(ClampThickness, ClampDiameter/2-ClampThickness/2, ClampDiameter/2-ClampThickness/2, $fn=100);
        translate([0, -ClampDiameter/4, 0]) cube([ClampDiameter, ClampDiameter/2, ClampThickness]);
         translate([0,-100, ClampThickness/2]) rotate([-90, 0, 0])screw();
   
     } ;
     translate([-ClampThickness/2, ClampDiameter/2-10, 0]) cube([ClampThickness, 100, ClampThickness]);
     
    
    
    
}


clamp();