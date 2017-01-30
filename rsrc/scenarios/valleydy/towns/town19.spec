@once-trap = 0
	sdf 19, 4
	msg 0, -1, -1
	pic 27, 4
	ex1 3, 1, -1
	ex2 15, -1, -1
	goto -1
@set-sector = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 2, -1
	ex2 5, 22, -1
	goto -1
@set-sector = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 2, -1
	ex2 9, 25, -1
	goto -1
@once-disp-msg = 3
	sdf 19, 0
	msg 1, 2, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 4
	sdf 19, 0
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
