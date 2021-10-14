@once-dlog = 0
	sdf 5, 0
	msg 0, -1, 1
	pic 104, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 1
	sdf 5, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 -1, -1, -1
	goto 8
@if-sdf = 2
	sdf 5, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 3, -1
	ex2 -1, -1, -1
	goto 8
@if-sdf = 3
	sdf 5, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto 8
@if-sdf = 4
	sdf 5, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 5, -1
	ex2 -1, -1, -1
	goto 8
@set-sdf = 5
	sdf 5, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 6
@once-dlog = 6
	sdf -1, -1
	msg 6, -1, 1
	pic 105, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@trans-ter = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 24, -1
	ex2 34, 33, -1
	goto 30
@disp-msg = 8
	sdf -1, -1
	msg 12, 13, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 9
	sdf 5, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 10
@if-spec-item = 10
	sdf -1, -1
	msg -1, 1, -1
	pic 104, 4
	ex1 0, 12, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 11
	sdf -1, -1
	msg 14, -1, 1
	pic 104, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 12
	sdf -1, -1
	msg 20, -1, 1
	pic 104, 1
	ex1 45, 13, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 13
	sdf 5, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once = 14
	sdf 5, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@if-spec-item = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 16, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 16
	sdf 5, 1
	msg 26, -1, 1
	pic 104, 1
	ex1 45, 1, -1
	ex2 -1, -1, -1
	goto -1
@once = 17
	sdf 5, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 21
@if-spec-item = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 22, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 22
	sdf 5, 3
	msg 32, -1, 1
	pic 104, 1
	ex1 45, 1, -1
	ex2 -1, -1, -1
	goto -1
@once = 23
	sdf 5, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 24
@if-spec-item = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 25, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 25
	sdf 5, 2
	msg 38, -1, 1
	pic 104, 1
	ex1 45, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 26
	sdf 5, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 7, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 27
	sdf 5, 6
	msg 44, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 28
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 29
	sdf 5, 7
	msg 46, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@trans-ter = 30
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 15, -1
	ex2 -1, -1, -1
	goto 31
@change-ter = 31
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 34, -1
	ex2 148, -1, -1
	goto 32
@change-ter = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 33, 24, -1
	ex2 148, -1, -1
	goto 34
@button-generic = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 26, -1
	ex2 6, 3, -1
	goto -1
@set-sdf = 34
	sdf 199, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
