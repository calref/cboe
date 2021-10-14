@if-context = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 1
@move-party = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 12, -1
	ex2 1, -1, -1
	goto -1
@if-context = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 3
@move-party = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 25, -1
	ex2 1, -1, -1
	goto -1
@if-context = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 5
@move-party = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 23, -1
	ex2 1, -1, -1
	goto -1
@once-town-encounter = 6
	sdf 14, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 7
	sdf 14, 1
	msg 1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 8
	sdf 14, 2
	msg 2, -1, -1
	pic 0, 4
	ex1 3, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 9
	sdf 14, 3
	msg 3, -1, -1
	pic 0, 4
	ex1 4, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 10
	sdf 14, 4
	msg 4, 5, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 11
	sdf -1, -1
	msg 6, -1, 1
	pic 11, 4
	ex1 36, 12, -1
	ex2 -1, -1, -1
	goto -1
@flip-sdf = 12
	sdf 14, 5
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@move-party = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 26, -1
	ex2 1, -1, -1
	goto -1
@if-context = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 13
@disp-msg = 15
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@rect-place-field = 16
	sdf 100, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 20, -1
	ex2 14, 27, -1
	goto -1
@change-ter = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 10, -1
	ex2 0, -1, -1
	goto 18
@change-ter = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 10, -1
	ex2 0, -1, -1
	goto -1
@button-generic = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 28, -1
	ex2 15, 0, -1
	goto -1
@button-generic = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 19, -1
	ex2 15, 0, -1
	goto -1
@button-generic = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 2, -1
	ex2 15, 0, -1
	goto -1
@button-generic = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 28, 11, -1
	ex2 15, 0, -1
	goto -1
@if-sdf = 23
	sdf 15, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto -1
