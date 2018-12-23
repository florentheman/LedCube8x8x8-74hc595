difference()
{
    cube([160,140,30]); //basis vorm
	translate([2.5,2.5,-1])cube([155,135,27+1]); //binnen vorm
	
	translate([145,15,-1])cylinder(r=5, h=30+2); //aan uit switch
	translate([154,15,5])cube([5+2,5,5]); //voedings snoer

	translate([135,40,29])cube([1,10,2]); //F
	translate([140,40,29])cube([1,7,2]); //F
	translate([135,40,29])cube([15,1,2]); //F
	translate([150,50,29])cube([2,7,2]); //L
	translate([140,50,29])cube([10,1,2]); //L
	translate([150,60,29])cube([2,7,2]); //E
	translate([140,60,29])cube([2,7,2]); //E
	translate([145,60,29])cube([2,7,2]); //E
	translate([140,60,29])cube([10,1,2]); //E

	translate([150,70,29])cube([2,7,2]); //d
	translate([140,70,29])cube([2,7,2]); //d
	translate([140,70,29])cube([10,1,2]); //d
	translate([135,78,29])cube([15,1,2]); //d

	// LEDS
	translate([15,15,-1])cylinder(r=3, h=30+2);
	translate([30,15,-1])cylinder(r=3, h=30+2);
	translate([45,15,-1])cylinder(r=3, h=30+2);
	translate([60,15,-1])cylinder(r=3, h=30+2);
	translate([75,15,-1])cylinder(r=3, h=30+2);
	translate([90,15,-1])cylinder(r=3, h=30+2);
	translate([105,15,-1])cylinder(r=3, h=30+2);
	translate([120,15,-1])cylinder(r=3, h=30+2);

	translate([15,30,-1])cylinder(r=3, h=30+2);
	translate([30,30,-1])cylinder(r=3, h=30+2);
	translate([45,30,-1])cylinder(r=3, h=30+2);
	translate([60,30,-1])cylinder(r=3, h=30+2);
	translate([75,30,-1])cylinder(r=3, h=30+2);
	translate([90,30,-1])cylinder(r=3, h=30+2);
	translate([105,30,-1])cylinder(r=3, h=30+2);
	translate([120,30,-1])cylinder(r=3, h=30+2);

	translate([15,45,-1])cylinder(r=3, h=30+2);
	translate([30,45,-1])cylinder(r=3, h=30+2);
	translate([45,45,-1])cylinder(r=3, h=30+2);
	translate([60,45,-1])cylinder(r=3, h=30+2);
	translate([75,45,-1])cylinder(r=3, h=30+2);
	translate([90,45,-1])cylinder(r=3, h=30+2);
	translate([105,45,-1])cylinder(r=3, h=30+2);
	translate([120,45,-1])cylinder(r=3, h=30+2);

	translate([15,60,-1])cylinder(r=3, h=30+2);
	translate([30,60,-1])cylinder(r=3, h=30+2);
	translate([45,60,-1])cylinder(r=3, h=30+2);
	translate([60,60,-1])cylinder(r=3, h=30+2);
	translate([75,60,-1])cylinder(r=3, h=30+2);
	translate([90,60,-1])cylinder(r=3, h=30+2);
	translate([105,60,-1])cylinder(r=3, h=30+2);
	translate([120,60,-1])cylinder(r=3, h=30+2);

	translate([15,75,-1])cylinder(r=3, h=30+2);
	translate([30,75,-1])cylinder(r=3, h=30+2);
	translate([45,75,-1])cylinder(r=3, h=30+2);
	translate([60,75,-1])cylinder(r=3, h=30+2);
	translate([75,75,-1])cylinder(r=3, h=30+2);
	translate([90,75,-1])cylinder(r=3, h=30+2);
	translate([105,75,-1])cylinder(r=3, h=30+2);
	translate([120,75,-1])cylinder(r=3, h=30+2);

	translate([15,90,-1])cylinder(r=3, h=30+2);
	translate([30,90,-1])cylinder(r=3, h=30+2);
	translate([45,90,-1])cylinder(r=3, h=30+2);
	translate([60,90,-1])cylinder(r=3, h=30+2);
	translate([75,90,-1])cylinder(r=3, h=30+2);
	translate([90,90,-1])cylinder(r=3, h=30+2);
	translate([105,90,-1])cylinder(r=3, h=30+2);
	translate([120,90,-1])cylinder(r=3, h=30+2);

	translate([15,105,-1])cylinder(r=3, h=30+2);
	translate([30,105,-1])cylinder(r=3, h=30+2);
	translate([45,105,-1])cylinder(r=3, h=30+2);
	translate([60,105,-1])cylinder(r=3, h=30+2);
	translate([75,105,-1])cylinder(r=3, h=30+2);
	translate([90,105,-1])cylinder(r=3, h=30+2);
	translate([105,105,-1])cylinder(r=3, h=30+2);
	translate([120,105,-1])cylinder(r=3, h=30+2);

	translate([15,120,-1])cylinder(r=3, h=30+2);
	translate([30,120,-1])cylinder(r=3, h=30+2);
	translate([45,120,-1])cylinder(r=3, h=30+2);
	translate([60,120,-1])cylinder(r=3, h=30+2);
	translate([75,120,-1])cylinder(r=3, h=30+2);
	translate([90,120,-1])cylinder(r=3, h=30+2);
	translate([105,120,-1])cylinder(r=3, h=30+2);
	translate([120,120,-1])cylinder(r=3, h=30+2);

	//Gaatjes voor de min
	translate([70,20,-1])cylinder(r=2, h=30+2);
	translate([85,20,-1])cylinder(r=2, h=30+2);
	translate([100,20,-1])cylinder(r=2, h=30+2);
	translate([115,20,-1])cylinder(r=2, h=30+2);

	translate([70,115,-1])cylinder(r=2, h=30+2);
	translate([85,115,-1])cylinder(r=2, h=30+2);
	translate([100,115,-1])cylinder(r=2, h=30+2);
	translate([115,115,-1])cylinder(r=2, h=30+2);
}

