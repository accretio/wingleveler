include <settings.scad>
include <polyScrewThread_r1.scad>


module screw(diameter) {
       hex_screw(diameter,4,55,100,1.5,2,24,8,0,0);
 
}


module big_screw() {
       screw(10);
}

module medium_screw() {
       screw(8);
}