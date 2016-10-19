ClampThickness=20;
ClampDiameter=30;
ClampLength=60;
ClampZone=20;
include <polyScrewThread_r1.scad>


module screw() {
       hex_screw(10,4,55,100,1.5,2,24,8,0,0);
 
}
module clamp() {
    difference(){
        cylinder(ClampThickness, ClampDiameter/2+ClampThickness/2, ClampDiameter/2+ClampThickness/2, $fn=100);
        cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=100);
        translate([0, -ClampDiameter/2, 0]) cube([ClampDiameter, ClampDiameter/1.2, ClampThickness]);
         translate([0,-100, ClampThickness/2]) rotate([-90, 0, 0])screw();
   
     } ;
     translate([-ClampThickness/2, ClampDiameter/2, 0]) {
         difference(){
             cube([ClampThickness, ClampLength, ClampThickness]);
             translate([0, ClampLength-ClampZone, 0])cube([ClampThickness/3, ClampLength, ClampThickness]);
             translate([ClampThickness - ClampThickness/3, ClampLength-ClampZone, 0]) cube([ClampThickness/3, ClampLength, ClampThickness]);
               translate([-40,ClampLength-ClampZone/2, ClampThickness/2]) rotate([90, 0, 90])screw();
       
             
         }
         
     };
     
     
     
    
    
    
}


clamp();